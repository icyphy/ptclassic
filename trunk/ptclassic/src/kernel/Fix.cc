static const char file_id[] = "Fix.cc";
#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

 Programmer:  A. Khazeni, J. Buck, B. Evans
 Date of creation: 2/24/93 
 Revisions:
	1/17/94  BLE	 cleaned up multiplication by zero
		 BLE	 added constant definitions for types of overflow
		 BLE	 added print out of overflow method in print method 
        1/23/94  jbuck   Major reworking!
	6/2/94   J.Weiss invented alternative format for precision strings:
			 "n/m" with n = number of fraction bits
				    m = total number of bits (word size)

This file contains the definitions of the functions declared in  
header file Fix.h. 

Fix type variables allow users to specify the number of bits and 
position of the binary point for the fixed-point representation of the 
variable. 

**********************************************************************/

#include "Fix.h"
#include "Error.h"
#include "type.h"
#include "PrecisionState.h"
#include <stream.h>
#include <std.h>
#include <string.h>
#include <math.h>
#include <minmax.h>
#include <ctype.h>

//////////////////////
// Default parameters
//////////////////////

const double WORD_SCALE         =  (double)(1L << FIX_BITS_PER_WORD);
const int    FIX_DEF_LENGTH     =  24;   // The default length in bits

static double twoRaisedTo[FIX_MAX_LENGTH+1];

// this class just initializes the table
struct FixInit {
    FixInit();
};

FixInit::FixInit() {
    double p2 = 1.0;
    for (int i = 0; i <= FIX_MAX_LENGTH; i++) {
	twoRaisedTo[i] = p2;
	p2 *= 2.0;
    }
}

// here is the dummy member to call the constructor.
static FixInit dummy;

/////////////////////////////
// Library Internal Functions
/////////////////////////////

// Given the word length and the position of the binary point, 
// find the maximum and the minimum values that can be 
// represented. 

inline double find_max(int ln, int ib)
{
    return twoRaisedTo[ib-1] - 1.0 / twoRaisedTo[ln - ib];
}

inline double find_min(int, int ib)
{
    return -twoRaisedTo[ib-1];
}

//////////////////////////////////////////////////////////
// Find the number of words (elements in array "Bits" of class Fix) 
// needed to represent a Fix variable given its length in bits. 

inline int find_words(int ln)
{
    return max ((ln + 15)/FIX_BITS_PER_WORD, 1);
}

//////////////////////////////////////////////////////////
// Find the number of bits needed to represent the integer "num" 

static int find_intBits(int num)
{
    int qt = num, count = 1; 
    if (num == 0) count = 0;
    while ((qt = int(qt/2)) != 0) count++;
    count++;					// Add 1 for the sign bit
    return count;
}

//////////////////////////////////////////////////////////
// replace bit array by its negative.  src and dest may be the same.

static void gen_complement(int nw, const uint16* srcbits, uint16* dstbits) {
    uint32 carry = 1;
    for (int i = nw-1; i >= 0; i--)
    {
	uint32 a = (uint16)(~srcbits[i]) + carry;// cast to prevent sign extension
	dstbits[i] = (uint16)a;
	carry = a >> FIX_BITS_PER_WORD;
    } 
}


// Given a double, store the bit pattern of its fixed-point  
// representation with a total length of "ln" and fraction
// bits "ib". The bit pattern is based on the 2's complement
// notation. 
// return 0 OK, Fix::err_ovf if value does not fit.
// if round is true, rounding is used.

void Fix::makeBits(double d, int round) {
    errors = 0;
    double x = d;
    double max_value = find_max(length, intBits);
    double min_value = find_min(length, intBits);
    // extract an extra bit if we are going to round
    int nwords = wordsIncludingRound(round);
    if ( x > max_value) {
	errors = err_ovf;
	x = max_value;
    }
    if (x < min_value) {
	errors = err_ovf;
	x = min_value;
    }
    int neg = (x < 0);
    if (neg) x = -x;
    int xshift = FIX_BITS_PER_WORD - intBits;
    if (xshift > 0) x *= twoRaisedTo[xshift];
    else x /= twoRaisedTo[-xshift];
    for (int i=0; i < nwords; i++)
    {
	Bits[i] = (uint16)x;
	x -= Bits[i];
	x *= WORD_SCALE;
    }
    if (x >= WORD_SCALE/2)  Bits[nwords-1]++;
    applyMask(round);
    if (neg) {
	gen_complement(4, Bits, Bits);
	applyMask(round);
    }
    return;
}

//////////////////////////////////////////////////////////
// Shift the bit pattern stored in "bits" by "nbits". Shift
// right if positive, shift left otherwise.

static void shift_bit_pattern(int nbits, uint16* bits)
{ 
    uint16 temp16_1, temp16_2, temp16_3 =0;
    uint16 tempbits[WORDS_PER_FIX];

    int mbits = abs(nbits);
    int offset = find_words(mbits);
    offset--;
    mbits = mbits - (offset * FIX_BITS_PER_WORD);

    for (int i=0; i < WORDS_PER_FIX; i++) {
	if (nbits >= 0){
	    if (i < offset) tempbits[i] = 0;
	    else            tempbits[i] = bits[i - offset];
	}
	else {
	    if (i < (WORDS_PER_FIX - offset)) tempbits[i] = bits[i + offset]; 
	    else            tempbits[i] = 0;
	} 
    }

    for (i=0; i < WORDS_PER_FIX; i++)
    {
	if (nbits >= 0) {	//shift bits to the right
	    temp16_1 = tempbits[i] >> mbits;
	    temp16_2 = tempbits[i] << (FIX_BITS_PER_WORD - mbits);
	    bits[i]  = temp16_1 + temp16_3;              
	    temp16_3 = temp16_2;
	}
	else {			//shift bits to the left
	    int j    = (WORDS_PER_FIX - 1) - i;         
	    temp16_1 = tempbits[j] << mbits;
	    temp16_2 = tempbits[j] >> (FIX_BITS_PER_WORD - mbits);
	    bits[j]  = temp16_1 + temp16_3;            
	    temp16_3 = temp16_2;
	}
    }
}

// Arithmatic shift. (same as shift_bit_pattern, only saves the sign bit)

static void ashift_bit_pattern(int nbits, uint16* bits)
{
    if (nbits == 0) return;
    int sign = bits[0] & 0x8000;
    shift_bit_pattern(nbits, bits);
    if (nbits >= 0) {
	if (sign) {
	    int offset = 0;
	    // we must set the high nbits bits to 1 now.
	    if (nbits >= FIX_BITS_PER_WORD) {
		offset = nbits/FIX_BITS_PER_WORD;
		for (int i = 0; i < offset; i++)
		    bits[i] = 0xffff;
		nbits = nbits % FIX_BITS_PER_WORD;
	    }
	    // now set the nbits high bits of bits[offset] to 1.
	    uint16 mask = 0xffff << (FIX_BITS_PER_WORD - nbits);
	    bits[offset] |= mask;
	}
    }
    else {
	// left shift, must preserve sign.
	if (sign)
	    bits[0] = 0x8000 | bits[0];
	else
	    bits[0] = 0x7fff & bits[0];
    }
}

///////////////////////
// Auxiliary functions
///////////////////////

// return true if equal to zero.

int Fix::is_zero() const {
    if (invalid()) return FALSE;
    if (length == 0) return TRUE;
    int nwords = words();
    for (int i = 0; i < nwords-1; i++)
	if (Bits[i] != 0) return FALSE;
    // need to mask the last word.
    int m = length & 0x0f;
    uint16 bitmask = (m == 0 ? 0xffff : (uint16)(0xffff0000L >> m));
    return (Bits[nwords-1] & bitmask) == 0;
}

// max legal value
double Fix::max() const { return find_max(length, intBits);}

// min legal value
double Fix::min() const 
{ 
    return find_min(length, intBits);
}

// return the value as a double.  Note that accuracy is limited, since
// a Fix can have more precision than a double can.

double Fix::value() const
{
    double d = 0.0;
    uint16 bitbuf[WORDS_PER_FIX];
    const uint16* bits = Bits;
    if (signBit()) {
	gen_complement(4, Bits, bitbuf);
	bits = bitbuf;
    }
    if ( length == 0 ) return d;
    for ( int i = words() - 1; i >= 0; i-- )
    {
	d += bits[i];
	d *= 1./WORD_SCALE;
    }
    double final = twoRaisedTo[intBits];
    d *= final;
    return signBit() ? -d : d;
}

// set the value to zero.  Any errors are cleared.
void Fix::setToZero()
{
    for (int i=0; i < WORDS_PER_FIX; i++) Bits[i] = 0;
    errors = 0;
}

void Fix::initialize()
{
    setToZero();
    length  = 0;
    intBits = 0;
    ovf_type = ovf_saturate;
}

static char *OverflowDescriptions[] =
	{ "saturate", "zero_saturate", "wrap", "warn" };

void Fix::set_ovflow(const char *p)
{
    int found = FALSE;
    for ( int i = 0; i < ovf_n_types; i++ ) {
	if ( strcasecmp(p, OverflowDescriptions[i]) == 0 ) {
	    found = TRUE;
	    set_overflow(i);
	    break;
	}
    }
    // For backward compatibility, support "wrapped" and "warning"
    if ( strcasecmp(p, "wrapped") == 0) {
	found = TRUE;
	set_overflow(ovf_wrapped);
    } else {
	if ( strcasecmp(p, "warning") == 0) {
	   found = TRUE;
	   set_overflow(ovf_warning);
	}
    }
    if ( ! found ) {
	errors |= err_invalid;
    }
}

////////////////
// Constructors
////////////////

////////////////////////////////////////////////////////////////
// constructor : makes a Fix object with length "ln",
// and the number of integer bits "ib".

Fix::Fix(int ln, int ib)
{
    setToZero();
    if (ln <= 0 || ln > FIX_MAX_LENGTH || ib < 0 || ib > ln) {
	errors = err_invalid;
    }
    else {
	length  = ln;
	intBits = ib;  
    }
    ovf_type = ovf_saturate;
    roundFlag = FALSE;
}

// same but use precision string/precision structure.

Fix::Fix(const char* precision)
{
    setToZero();
    if (!setPrecision(precision)) errors = err_invalid;
    ovf_type = ovf_saturate;
    roundFlag = FALSE;
}

Fix::Fix(const Precision& p)
{
    setToZero();
    if (p.len() <= 0 || p.len() > FIX_MAX_LENGTH || p.intb() < 0 || p.intb() > p.len()) {
	errors = err_invalid;
    }
    else {
	length  = p.len();
	intBits = p.intb();  
    }
    ovf_type = ovf_saturate;
    roundFlag = FALSE;
}

////////////////////////////////////////////////////////////////
// constructor : make a Fix, specifying bits exactly.
Fix::Fix(const char* precision, uint16* bits) {
    ovf_type = ovf_saturate;
    roundFlag = FALSE;
    if (!setPrecision(precision)) {
	errors = err_invalid;
	setToZero();
    }
    else {
	for (int i = words() - 1; i >= 0; i--)
	    Bits[i] = bits[i];
	applyMask(FALSE);
    }
}

////////////////////////////////////////////////////////////////
// constructor : makes a Fix object out of the double "d" with 
// the default length of 24 bits.

Fix::Fix(double d)
{
  if (d == 0.0) initialize();
  else {
    intBits = find_intBits(int(d));
    length  = FIX_DEF_LENGTH;
    if (intBits > length ) intBits = length;
    makeBits(d, TRUE);
    ovf_type = ovf_saturate;
    roundFlag = FALSE;
  }
}

////////////////////////////////////////////////////////////////
// constructor : makes a Fix object out of a double
// with length "ln" and number of integer bits "ib".
// We always round.

Fix::Fix(int ln, int ib, double d)
{  
    if (ln <= 0 || ln > FIX_MAX_LENGTH || ib < 0 || ib > ln) {
	initialize();
	errors = err_invalid;
    }
    else {
	length  = ln;
	intBits = ib;  
	makeBits(d, TRUE);
	ovf_type = ovf_saturate;
    }
    roundFlag = FALSE;
}

// same but use precision string/precision structure.

Fix::Fix(const char* precision, double d)
{
    if (!setPrecision(precision)) {
	setToZero();
	errors = err_invalid;
    }
    else {
	makeBits(d, TRUE);
	ovf_type = ovf_saturate;
	roundFlag = FALSE;
    }
}

Fix::Fix(const Precision& p, double d)
{  
    if (p.len() <= 0 || p.len() > FIX_MAX_LENGTH || p.intb() < 0 || p.intb() > p.len()) {
	initialize();
	errors = err_invalid;
    }
    else {
	length  = p.len();
	intBits = p.intb();  
	makeBits(d, TRUE);
	ovf_type = ovf_saturate;
    }
    roundFlag = FALSE;
}

////////////////////////////////////////////////////////////////
// constructor : copy constructor

Fix::Fix(const Fix& x)
{  
  for (int i=0; i < WORDS_PER_FIX; i++) Bits[i] = x.Bits[i]; 
  length  = x.length;
  intBits = x.intBits;  
  ovf_type = x.ovf_type;
  errors = x.errors;
  roundFlag = x.roundFlag;
}

////////////////////////////////////////////////////////////////
// constructor : makes a new Fix object out of another
// Fix object with new length and number of integer bits.

Fix::Fix(int ln, int ib, const Fix& x)
{
    if (ln <= 0 || ln > FIX_MAX_LENGTH || ib <= 0 || ib > ln) {
	initialize();
	errors = err_invalid;
	return;
    }

    double max_value = find_max(ln, ib);
    double min_value = find_min(ln, ib);
    int intBits_diff = ib - x.intBits;

    for (int j=0; j < WORDS_PER_FIX; j++) Bits[j] = x.Bits[j]; 
    length  = ln;
    intBits = ib;  
    ovf_type = x.ovf_type;
    errors = x.errors;
    roundFlag = x.roundFlag;

    if (x.value() <= max_value && x.value() >= min_value) {  
	ashift_bit_pattern(intBits_diff, Bits);
	applyMask(roundFlag);
    }
    else 
	overflow_handler(x.signBit(),intBits_diff);
}

/////////////////////
// Casting functions 
/////////////////////

// extract int part directly if it fits in one word, else use double cast.
// truncate toward zero.

Fix :: operator int () const {
    if (intBits <= FIX_BITS_PER_WORD) {
	if (signBit() == 0) return Bits[0] >> (FIX_BITS_PER_WORD - intBits);
	else {
	    long bits = (1L<<FIX_BITS_PER_WORD) - Bits[0];
	    bits >>= (FIX_BITS_PER_WORD - intBits);
	    return -bits;
	}
    }
    else return int(value());
}

///////////////////////
// Assignment operators
///////////////////////

Fix& Fix::operator = (const Fix& x)
{ 
    int intBits_diff = intBits - x.intBits;
    for (int j=0; j < WORDS_PER_FIX; j++) Bits[j] = x.Bits[j]; 
    errors = x.errors;
    if (length == 0) {         // make an identical copy of x if uninitialized 
	length = x.length;
	intBits = x.intBits;
	ovf_type = x.ovf_type;
	roundFlag = x.roundFlag;
    }
    else if(x.value() <= max() && x.value() >= min()) {  
	ashift_bit_pattern(intBits_diff, Bits); 
	applyMask(roundFlag);
    }
    else 
	overflow_handler(x.signBit(),intBits_diff);
    return *this;
}

////////////////////////
// Arithmetic operators
////////////////////////

void Fix::complement()
{
    gen_complement(words(), Bits, Bits);
}

Fix operator + (const Fix& x, const Fix& y)
{  
  uint32 sum = 0, carry = 0;
  // # to right of decimal point (fractional bits) = max of the two
  int bitsright = max (x.length-x.intBits, y.length-y.intBits);
  int new_intBits = min(max(x.intBits,y.intBits) + 1, FIX_MAX_LENGTH);
  int new_length = min(bitsright + new_intBits, FIX_MAX_LENGTH);

  Fix z(new_length, new_intBits);
  uint16 XBits[WORDS_PER_FIX], YBits[WORDS_PER_FIX];
  for (int j=0; j < WORDS_PER_FIX; j++) {
    XBits[j] = x.Bits[j]; YBits[j] = y.Bits[j];} 
  ashift_bit_pattern((new_intBits - x.intBits), XBits);  // allign x
  ashift_bit_pattern((new_intBits - y.intBits), YBits);  // allign y

  for (int i=WORDS_PER_FIX - 1; i >= 0; i-- )
    {
      sum = carry + (uint32)(XBits[i]) + (uint32)(YBits[i]);
      carry = sum >> FIX_BITS_PER_WORD;
      z.Bits[i] = (uint16)sum;
    }
  z.ovf_type = x.ovf_type;
  z.errors = x.errors | y.errors;
  // overflow can happen if result has intBits = 64.  Check:
  // signs of args same and sign of result different
  int xsign = x.signBit();
  if (new_intBits == FIX_MAX_LENGTH &&
      xsign == y.signBit() &&
      xsign != z.signBit())
      z.overflow_handler(xsign,0);
  return z;
}

Fix operator - (const Fix& x, const Fix& y)
{
    Fix tmp(y);
    tmp.complement();
    return x + tmp;
}

Fix operator * (const Fix& x, const Fix& y)
{
  // Rewrote  if (x.length == 0 || y.length == 0) return z;
  Fix z;
  int new_length  = min(x.length  + y.length - 1, FIX_MAX_LENGTH);
  int ideal_intBits = x.intBits + y.intBits - 1;
  int new_intBits = min(ideal_intBits, FIX_MAX_LENGTH);
  z = Fix(new_length, new_intBits);
  if (x.is_zero() || y.is_zero()) {
      z.setToZero();
      return z;
  }

  Fix X(x), Y(y);
  if (x.signBit()) X.complement();
  if (y.signBit()) Y.complement();
  int post_scale = ideal_intBits - new_intBits;

  // we do the following optional scale_down to guarantee that the
  // multiplication cannot overflow.  If it is needed, the scale_up
  // at the end may cause an overflow though.

  if (post_scale != 0) {
      // here we effectively scale down X and Y.
      // we will scale up later.
      if ((int)(x.intBits) > post_scale) X.intBits = x.intBits - post_scale;
      else {
	  X.intBits = 1;
	  Y.intBits = new_intBits - 2;
      }
  }

  for (int i = x.words() - 1; i >= 0; i-- )
  {
     uint32 carry = 0;
     for (int j = y.words() - 1; j >= 0; j-- )
     {
         int k = i + j + 1;
         uint32 a = (uint32)X.Bits[i] * (uint32)Y.Bits[j];
         uint32 b = ((a << 1) & 0xffff) + carry;
         if (k < z.words()) {
            b += z.Bits[k];
            z.Bits[k] = (uint16)b; }
         if (k < z.words() + 1)
            carry = (a >> (FIX_BITS_PER_WORD-1)) + (b >> FIX_BITS_PER_WORD);
     }
     z.Bits[i] = (uint16)carry;
  }
  if (x.signBit() != y.signBit()) z.complement();
  z.errors = x.errors | y.errors;
  // if post_scale is set, we must now shift up z by that many bits.
  // it may not fit.  We do the shift by assigning to another Fix
  // with a smaller intBits, and then re-adjusting.  The overflow, if
  // any, is then detected.
  if (post_scale > 0) {
      Fix scale_z(FIX_MAX_LENGTH, FIX_MAX_LENGTH - post_scale, z);
      scale_z.intBits = FIX_MAX_LENGTH;
      return scale_z;
  }
  else return z;
}


Fix operator * (const Fix& x, int n)
{  
  int32 a, carry = 0;
  Fix z(x);
  if ( (x.length == 0) || (x.value() == 0.0) || (n == 0) ) {
    z.setToZero();
    return z;
  }

  for (int i = x.words() - 1; i >= 0; i-- ) {
     a = (int32)(uint32)x.Bits[i] * n + carry;
     z.Bits[i] = (uint16)a;
     carry = a >> FIX_BITS_PER_WORD;
  }
  if ((x.value()*n) > x.max() || (x.value()*n) < x.min()) {
      int nsign = (n < 0);
      z.overflow_handler(x.signBit() ^ nsign,0);
  }
  z.errors = x.errors;
  return z;
}

Fix operator * (int n, const Fix& x) { return (x * n); }

// This operation cheats and does a floating point division.
// Precision is the same as that of the divisor.

Fix operator /  (const Fix& x, const Fix& y) 
{ 
  double temp;
  int new_length  = x.length;
  int new_intBits = x.intBits;
  int dbz = FALSE;
  if ((temp = y.value()) != 0) {
    temp = x.value()/temp;
    new_intBits = find_intBits(int(temp));
  }
  else {
    dbz = TRUE;
  }
  Fix t(new_length, new_intBits, temp);
  t.errors = x.errors | y.errors;
  if (dbz) t.errors |= Fix::err_dbz;
  return t;
}

Fix& Fix::operator += (const Fix& x)
{    *this = *this + x;
     return *this;
}

Fix&  Fix::operator -= (const Fix& x)
{    *this = *this - x;
     return *this;
}

Fix&  Fix::operator *= (const Fix& x)
{    *this = *this * x;
     return *this;
}

Fix&  Fix::operator *= (int n)
{    *this = *this * n;
     return *this;
}

Fix&  Fix::operator /= (const Fix& x) 
{    *this = *this / x;
     return *this;
}

///////////////////////
// Comparison functions
///////////////////////

// return -1 if a<b, 0 if a==b, 1 if a>b.
// Comparison is accurate to full scale if formats are the same.
// otherwise we compare double values.

int compare(const Fix& a, const Fix& b) {
    int signDiff = b.signBit() - a.signBit();
    if (signDiff != 0) return signDiff;
    if (a.intBits == b.intBits && a.length == b.length) {
	int w = a.words();
	for (int i = 0; i < w; i++) {
	    int diff = a.Bits[i] - b.Bits[i];
	    if (diff < 0) return -1;
	    if (diff > 0) return 1;
	}
	return 0;
    }
    else {
	double diff = a.value() - b.value();
	if (diff < 0) return -1;
	return (diff > 0);
    }
}

////////////////////////
// unary minus
////////////////////////

Fix operator - (const Fix& x) { 
  Fix tmp(x);
  tmp.complement();
  return tmp;
}

////////////////////
// printing function
////////////////////

void printFix(const Fix& x)        // print contents
{   
  cout << "Bits(hex) = ";
  for ( int i = 0; i < WORDS_PER_FIX; i++ ) {
    char bitbuf[8];
    sprintf(bitbuf, "%04X ", x.Bits[i]);
    cout << bitbuf;
  }
  cout << "\nlength = " << x.len() << ", ";
  cout << "intBits = " << x.intb() << ", ";
  cout << (x.roundMode() ? "round, " : "truncate, ");
  cout << "errors = ";
  if (x.invalid()) cout << "bad params, ";
  else if (x.dbz()) cout << "divide by zero, ";
  else if (x.ovf_occurred()) cout << "overflow occurred, ";
  else cout << "none, ";
  char *overflowStr;
  int overflowType = x.overflow();
  if ( overflowType < 0 || overflowType >= Fix::ovf_n_types )
    overflowStr = "not valid";
  else
    overflowStr = OverflowDescriptions[overflowType];
  cout << "overflow  = " << overflowStr << "\n";

  cout << "value     = " << x.value() << "\n";
}

ostream& operator << (ostream& s, const Fix& x)
{
    if (x.invalid()) {
	s << "(Invalid:";
	if ((x.errors & Fix::err_dbz) != 0) s << " DBZ";
	if ((x.errors & Fix::err_invalid) != 0) s << " Bad Params";
	s << ")";
    }
    else {
	s << "(" << x.value() << ", "
	  << x.intb() << "." << x.precision() << ")";
    }
    return s;
}

// restrict result to the correct number of bits, optionally applying rounding

void Fix::applyMask (int round)
{
// nwords may need to include an extra bit for rounding
    int nwords = wordsIncludingRound(round);
    for (int i = nwords; i < WORDS_PER_FIX; i++) Bits[i] = 0;
    int m = length & 0x0f;
    if (!m && !round) return;
    if (round) {
	uint32 sum = (uint32)Bits[nwords-1] + (1 << (FIX_BITS_PER_WORD-m-1));
	Bits[nwords-1] = (uint16) sum;
	uint32 carry;
	for ( int i=nwords - 2; i >= 0; i-- ) {
	    carry = sum >> FIX_BITS_PER_WORD;
	    sum = carry + (uint32)Bits[i];
	    Bits[i] = (uint16)sum;
	}
    }
    Bits[nwords-1] &= (uint16) ( 0xffff0000L >> m );
}
 
////////////////////
// overflow handler
////////////////////

// "this" is the result that has been just computed, that
// has an overflow.  As a rule, its low order bits will be correct,
// but they may not be aligned correctly.
//
// The first argument is the sign bit of the full precision result,
// that is, 1 for negative, 0 for positive.
//
// The second argument is the alignment error in the bits that are
// provided.

void Fix:: overflow_handler (int rsign, int shift)
{
    errors |= err_ovf;
    int i;
    switch (overflow()) {

    case ovf_zero_saturate:		// zero saturate
	for (i=0; i < WORDS_PER_FIX; i++)
	    Bits[i] = 0;
	break;

    case ovf_warning:			// warning
	Error::warn(" Fix : overflow error");
	/* fall through -- also do the saturate */

    case ovf_saturate:			// saturate
	if (rsign) {
	    Bits[0] = 0x8000;
	    for (int i=1; i < words(); i++)  Bits[i] = 0; }
	else {
	    Bits[0] = 0x7fff;
	    for (int i=1; i < words(); i++)  Bits[i] = 0xffff;
	}
	applyMask(FALSE);
	break;

    case ovf_wrapped:			// wrapped
	// Up to version 0.5.1, this did the wrong thing,
	// giving a the largest number of the opposite sign. EAL.
	shift_bit_pattern(shift,Bits);
	applyMask(FALSE);
	break;

    default:
	Error::abortRun(": overflow handling method not defined");
    }
}

///////////////////////////////////
// Parameter manipulation functions
///////////////////////////////////

// parse the precision string and set intBits and length.
// no other fields are changed.  Return TRUE if OK, otherwise
// FALSE.

int Fix::setPrecision(const char * prec_spec)
{
    // JW: undefine precision if empty string is given
    if (!*prec_spec) {
	length = intBits = 0;
	return TRUE;
    }

    if (!isdigit(*prec_spec))
	return FALSE;

    int first = atoi(prec_spec);

    while (*prec_spec && isdigit(*prec_spec))
	prec_spec++;
    if ((*prec_spec != '.' && *prec_spec != '/') || !isdigit(prec_spec[1]))
	return FALSE;

    int flag = (*prec_spec == '.');

    prec_spec++;
    int second = atoi(prec_spec);

    // note that we do not check for extra garbage at the end of prec_spec.

    if (flag)  // "intBits.fracBits"
	intBits = first,
	length  = second + intBits;
    else       // "fracBits/length"
	length  = second,
	intBits = length - first;

    if ((intBits <= 0) || (length < intBits) || (length > FIX_MAX_LENGTH))
	return FALSE;
    else return TRUE;
}
   
// Extract the integer part of character string "p"

int Fix::get_intBits(const char *p)
{
   int intBits = atoi(p);
   return intBits;
}

//////////////////////////////////////////////////////////
// Given a const character specifying the number of bits to the
// left and right of the binary point (i.e. example : 2.14, meaning
// that 2 bits to the left and 14 bits to the right of binary point),
// find the total word length.

int Fix::get_length(const char *p)
{
   char rdigits[6];
   int i = 0;
   int intBits = get_intBits(p);   //extract integer part of "p"
   while (*(p + i) != '.') i++;    //count # of digits to the left of BP
   strcpy (rdigits, (p + i + 1));  //read the right digits into string "rdigits"
   int frcBits = atoi(rdigits);    //cast "rdigits" into an integer
   int length = intBits + frcBits; //find total length
   return length;
}


