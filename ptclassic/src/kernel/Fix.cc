static const char file_id[] = "Fix.cc";
#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1993 The Regents of the University of California.
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

 Programmer:  A. Khazeni 
 Date of creation: 2/24/93 
 Revisions:


This file contains the definitions of the functions declared in  
header file Fix.h. 

Fix type variables allow users to specify the number of bits and 
position of the binary point for the fixed-point representation of the 
variable. 

**********************************************************************/

#include "Fix.h"
#include "Error.h"
#include <stream.h>
#include <std.h>
#include <string.h>
#include <math.h>

//////////////////////
// Default parameters
//////////////////////

const int    BITS_PER_WORD      =  16;
const int    FIX_DEF_LENGTH     =  24;   // The default length in bits
const int    FIX_MAX_LENGTH     =  WORDS_PER_FIX * BITS_PER_WORD;
mask_func_pointer Fix::MASK     =  Fix::mask_truncate;

/////////////////////////////
// Library Internal Functions
/////////////////////////////

// Given the word length and the position of the binary point, 
// find the maximum and the minimum values that can be 
// represented. 

static double find_max(int ln, int ib)
{
  double r = 0.0, l = 0.0;
  for (int i=0; i <(ib - 1); i++) r += pow(2.0,i);
  int fb = ln - ib;
  for (int j= -1; j >=(-fb); j--) l += pow(2.0,j);
  r += l;
  return r;
}

static double find_min(int ln, int ib)
{
  double max,r;
  max = find_max(ln, ib);
  r = (-(max)) - pow(2.0,-(ln - ib));
  return r;
}

//////////////////////////////////////////////////////////
// Find the number of words (elements in array "Bits" of class Fix) 
// needed to represent a Fix variable given its length in bits. 

static int find_words(int ln)
{
  int n = ln + 15;
  n = n / BITS_PER_WORD;
  if ( n <= 0) n = 1;
  return n;
}

//////////////////////////////////////////////////////////
// Find the number of bits needed to represent the integer "num" 

static int find_intBits(int num)
{
   int qt = num, count = 1; 
   if (num == 0) count = 0;
   while ((qt = int(qt/2)) != 0) count++;
   count += 1;             // For the sign bit
   return count;
}

//////////////////////////////////////////////////////////
// Given a double, store the bit pattern of its fixed-point  
// representation with a total length of "ln" and fraction
// bits "ib". The bit pattern is based on the 2's complement
// notation. 

void Fix::make_bit_pattern(int ln, int ib, double d, uint16* bits)
{
  double x = d;
  double max_value = find_max(ln, ib);
  double min_value = find_min(ln, ib);
  int NO_OF_WORDS = find_words(ln);

  if ( x > max_value || x < min_value)
    Error::abortRun("Error : overflow in declaration (construction)");
  else {
     if (x < 0) x += pow(2.0, ib);    //equivalent of taking 2's complement
     x *= pow(2.0,(BITS_PER_WORD - ib));
     for (int i=0; i < NO_OF_WORDS; i++)
     {
        bits[i] = (uint16)x;
        x -= bits[i];
        x *= 65536;
     }
     if (x >= 32768)  bits[NO_OF_WORDS-1]++; }
  (*MASK)(ln, bits);
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
  mbits = mbits - (offset * BITS_PER_WORD);

  for (int i=0; i < WORDS_PER_FIX; i++) {
    if (nbits >= 0){
       if (i < offset) tempbits[i] = 0;
       else            tempbits[i] = bits[i - offset]; }
    else {
       if (i < (WORDS_PER_FIX - offset)) tempbits[i] = bits[i + offset]; 
       else            tempbits[i] = 0; } 
  }

  for (i=0; i < WORDS_PER_FIX; i++)
  {
    if (nbits >= 0) {                               //shift bits to the right
       temp16_1 = tempbits[i] >> mbits;
       temp16_2 = tempbits[i] << (BITS_PER_WORD - mbits);
       bits[i]  = temp16_1 + temp16_3;              
       temp16_3 = temp16_2; }
    else {                                          //shift bits to the left
       int j    = (WORDS_PER_FIX - 1) - i;         
       temp16_1 = tempbits[j] << mbits;
       temp16_2 = tempbits[j] >> (BITS_PER_WORD - mbits);
       bits[j]  = temp16_1 + temp16_3;            
       temp16_3 = temp16_2; }                     
  }
}

// Arithmatic shift. (same as shift_bit_pattern, only saves the sign bit)

static void ashift_bit_pattern(int nbits, uint16* bits)
{
   int sign = bits[0] & 0x8000;
   if (nbits >= 0) {
      if (sign == 0)
         shift_bit_pattern(nbits, bits);
      else {
         for (int i = 1; i <= nbits; i++)
         {
           shift_bit_pattern(1, bits);
           bits[find_words(i) - 1] = bits[find_words(i) - 1] | 0x8000;
         }
      }
   }
   else {
      shift_bit_pattern(nbits, bits);
      if (sign)
         bits[0] = 0x8000 | bits[0];
      else
         bits[0] = 0x7fff & bits[0];
   }
}

///////////////////////
// Auxiliary functions
///////////////////////

int    Fix::signBit() const { int n = Bits[0] & 0x8000; return n;}
int    Fix::words() const { return find_words(length);}
double Fix::max() const { return find_max(length, intBits);}
double Fix::min() const 
{ 
  if (fixtype()) 
    return 0.0;
  else
    return find_min(length, intBits);
}

double Fix::value() const
{
  double d = 0.0;
  if (len() == 0) return d;
  for ( int i = words()-1; i >= 0; i-- )
  {
     d += Bits[i];
     d *= 1./65536.;
  }
  d *= pow(2.0,intb());
  return d <= max() ? d : d - (pow(2.0,intb()));
}

void Fix::initialize()
{
  for (int i=0; i < WORDS_PER_FIX; i++) Bits[i] = 0;
  length  = 0;
  intBits = 0;
  format  = 0;
  ovflow = 0;
}

void Fix::set_ovflow(const char *p)
{
  if (strcmp(p,"saturate") == 0) ovflow = 0;
  else if (strcmp(p,"zero_saturate") == 0) ovflow = 1;
  else if (strcmp(p,"wrapped") == 0) ovflow = 2;
  else if (strcmp(p,"warning") == 0) ovflow = 3;
  else
   Error::abortRun(": not a valid overflow handling method for the class Fix");
}

////////////////
// Constructors
////////////////

// constructor : makes an uninitialized Fix object 

Fix::Fix()
{
  for (int i=0; i < WORDS_PER_FIX; i++) Bits[i] = 0; 
  length  = 0; 
  intBits = 0;  
  format  = 0;
  ovflow = 0;
}

////////////////////////////////////////////////////////////////
// constructor : makes a Fix object with length "ln",
// and the number of integer bits "ib".

Fix::Fix(int ln, int ib)
{
  if (ln <= 0 || ln > FIX_MAX_LENGTH) 
    Error::abortRun("Fix : illegal length in declaration");
  else if (ib <= 0 || ib > ln) 
    Error::abortRun("Fix : illegal number of range bits in declaration");
  for (int j=0; j < WORDS_PER_FIX; j++) Bits[j] = 0; 
  length  = ln;
  intBits = ib;  
  format  = 0;
  ovflow = 0;
}

////////////////////////////////////////////////////////////////
// constructor : makes an unsigned Fix object with length "ln",
// and the number of integer bits "ib".

Fix::Fix(int ln, int ib, const char* p)
{
  if (strcmp(p,"unsigned") != 0) 
    Error::abortRun("Fix : illegal format in declaration");
  else if (ln <= 0 || ln > FIX_MAX_LENGTH) 
    Error::abortRun("Fix : illegal length in declaration");
  else if (ib < 0 || ib > ln) 
    Error::abortRun("Fix : illegal number of range bits in declaration");
  for (int j=0; j < WORDS_PER_FIX; j++) Bits[j] = 0; 
  length  = ln + 1;
  intBits = ib + 1;  
  format  = 1;
  ovflow = 0;
}

////////////////////////////////////////////////////////////////
// constructor : makes a Fix object out of the double "d" with 
// the default length of 24 bits.

Fix::Fix(const double& d)
{
  if (d == 0.0) {
    for (int i=0; i < WORDS_PER_FIX; i++) Bits[i] = 0;
    length  = 0; intBits = 0; }
  else {
    intBits = find_intBits(int(d));
    length  = FIX_DEF_LENGTH;
    if (intBits > length ) intBits = length;
    make_bit_pattern(length, intBits, d, Bits); }

  format = 0;
  ovflow = 0;
}

////////////////////////////////////////////////////////////////
// constructor : makes a Fix object out of a double
// with length "ln" and number of integer bits "ib".

Fix::Fix(int ln, int ib, const double& d)
{  
  if (ln <= 0 || ln > FIX_MAX_LENGTH) 
    Error::abortRun("Fix : illegal length in declaration");
  else if (ib <= 0 || ib > ln) 
    Error::abortRun("Fix : illegal number of range bits in declaration");
  make_bit_pattern(ln, ib, d, Bits);
  length  = ln;
  intBits = ib;  
  format = 0;
  ovflow = 0;
}

////////////////////////////////////////////////////////////////
// constructor : makes an unsigned Fix object out of a double
// with length "ln" and number of integer bits "ib".

Fix::Fix(int ln, int ib, const double& d, const char* p)
{  
  if (strcmp(p,"unsigned") != 0) 
    Error::abortRun("Fix : illegal format in declaration");
  else if (d < 0)
    Error::abortRun("Fix : negative argument in declaration of unsigned Fix");
  else if (ln <= 0 || ln > FIX_MAX_LENGTH) 
    Error::abortRun("Fix : illegal length in declaration");
  else if (ib < 0 || ib > ln) 
    Error::abortRun("Fix : illegal number of range bits in declaration");
  make_bit_pattern((ln + 1), (ib + 1), d, Bits);
  length  = ln + 1;
  intBits = ib + 1;  
  format = 1;
  ovflow = 0;
}

////////////////////////////////////////////////////////////////
// constructor : copy constructor

Fix::Fix(const Fix& x)
{  
  for (int i=0; i < WORDS_PER_FIX; i++) Bits[i] = x.Bits[i]; 
  length  = x.length;
  intBits = x.intBits;  
  format = x.format;
  ovflow = x.ovflow;
}

////////////////////////////////////////////////////////////////
// constructor : makes a new Fix object out of another
// Fix object with new length and number of integer bits.

Fix::Fix(int ln, int ib, const Fix& x)
{
  if (ln <= 0 || ln > FIX_MAX_LENGTH) 
    Error::abortRun("Fix : illegal length in declaration");
  else if (ib <= 0 || ib > ln) 
    Error::abortRun("Fix : illegal number of range bits in declaration");

  double max_value = find_max(ln, ib);
  double min_value = find_min(ln, ib);
  int intBits_diff = ib - x.intBits;

  for (int j=0; j < WORDS_PER_FIX; j++) Bits[j] = x.Bits[j]; 
  length  = ln;
  intBits = ib;  
  format  = x.format;
  ovflow = x.ovflow;

  if (x.value() <= max_value && x.value() >= min_value) {  
     ashift_bit_pattern(intBits_diff, Bits);
     (*MASK)(length, Bits); }
  else 
     overflow_handler(*this, x);
}

/////////////////////
// Casting functions 
/////////////////////

Fix :: operator int () const {return int(value());}
Fix :: operator float () const {return float(value());}
Fix :: operator double () const {return value();}

///////////////////////
// Assignment operators
///////////////////////

Fix& Fix::operator = (const Fix& x)
{ 
  if (x.signBit() && fixtype())
    Error::abortRun("Fix: assignment of a negative to an unsigned Fix object");

  int intBits_diff = intBits - x.intBits;
  for (int j=0; j < WORDS_PER_FIX; j++) Bits[j] = x.Bits[j]; 
  if (length == 0) {         // make an identical copy of x if uninitialized 
     length = x.length;
     intBits = x.intBits;
     ovflow = x.ovflow; 
     format = x.format; }
  else if(x.value() <= max() && x.value() >= min()) {  
     ashift_bit_pattern(intBits_diff, Bits); 
     (*MASK)(length, Bits); }
  else 
     overflow_handler(*this, x);

  return *this;
}

Fix Fix::operator =  (const double& d)
{
  if (fixtype() && d < 0)
    Error::abortRun("Fix: assignment of a negative to an unsigned Fix object");
  else if (length == 0) {
    if (d == 0.0) return *this; 
    intBits = find_intBits(int(d));
    length  = FIX_DEF_LENGTH;
    if (intBits > length ) intBits = length;
    make_bit_pattern(length, intBits, d, Bits); }
  else if (d <= max() && d >= min())   
    make_bit_pattern(length, intBits, d, Bits); 
  else
    overflow_handler(*this, d);

  return *this;
}

////////////////////////
// Arithmetic operators
////////////////////////

void complement(Fix& x)
{
  uint32 carry = 1;

  if (!x.fixtype())
  {  
    for (int i=x.words()-1 ; i >= 0; i-- )
    {
      uint32 a = ~x.Bits[i] + carry;
      x.Bits[i] = (uint16)a;
      carry = a >> 16;
    } 
  }
}

Fix operator + (const Fix& x, const Fix& y)
{  
  uint32 sum = 0, carry = 0;
  int larger_length  = x.length  <= y.length  ? y.length  : x.length;
  int larger_intBits = x.intBits <= y.intBits ? y.intBits : x.intBits;
  int new_length  = larger_length  + 1;
  int new_intBits = larger_intBits + 1;

  Fix z(new_length, new_intBits);
  uint16 XBits[WORDS_PER_FIX], YBits[WORDS_PER_FIX];
  for (int j=0; j < WORDS_PER_FIX; j++) {
    XBits[j] = x.Bits[j]; YBits[j] = y.Bits[j];} 
  ashift_bit_pattern((new_intBits - x.intBits), XBits);  // allign x
  ashift_bit_pattern((new_intBits - y.intBits), YBits);  // allign y

  for (int i=WORDS_PER_FIX - 1; i >= 0; i-- )
    {
      sum = carry + (uint32)(XBits[i]) + (uint32)(YBits[i]);
      carry = sum >> BITS_PER_WORD;
      z.Bits[i] = (uint16)sum;
    }
  return z;
}

Fix operator - (const Fix& x, const Fix& y)
{  
  uint32 sum = 0, carry = 0;
  int larger_length  = x.length  <= y.length  ? y.length  : x.length;
  int larger_intBits = x.intBits <= y.intBits ? y.intBits : x.intBits;
  int new_length  = larger_length  + 1;
  int new_intBits = larger_intBits + 1;

  Fix z(new_length, new_intBits);
  uint16 XBits[WORDS_PER_FIX], YBits[WORDS_PER_FIX];
  for (int j=0; j < WORDS_PER_FIX; j++) {
    XBits[j] = x.Bits[j]; YBits[j] = y.Bits[j];} 
  ashift_bit_pattern((new_intBits - x.intBits), XBits);  // allign x
  ashift_bit_pattern((new_intBits - y.intBits), YBits);  // allign y

  for (int i=WORDS_PER_FIX - 1; i >= 0; i-- )
    {
      sum = carry + (uint32)(XBits[i]) - (uint32)(YBits[i]);
      carry = sum >> BITS_PER_WORD;
      z.Bits[i] = (uint16)sum;
    }
  return z;
}

Fix operator * (const Fix& x, const Fix& y)
{  
  Fix X(x), Y(y), z;
  if (x.length == 0 || y.length == 0) return z; 
  int new_length  = x.length  + y.length;
  int new_intBits = x.intBits + y.intBits - 1;
  if (x.signBit()) complement(X);
  if (y.signBit()) complement(Y);

  z = Fix(new_length, new_intBits);

  for (int i=x.words() - 1; i >= 0; i-- )
  {
     uint32 carry = 0;
     for (int j=y.words() - 1; j >= 0; j-- )
     {
         int k = i + j + 1;
         uint32 a = (uint32)X.Bits[i] * (uint32)Y.Bits[j];
         uint32 b = ((a << 1) & 0xffff) + carry;
         if (k < z.words()) {
            b += z.Bits[k];
            z.Bits[k] = (uint16)b; }
         if (k < z.words() + 1)
            carry = (a >> (BITS_PER_WORD-1)) + (b >> BITS_PER_WORD);
     }
     z.Bits[i] = (uint16)carry;
  }
  if (x.signBit() != y.signBit()) complement(z);
  return z;
}


Fix operator * (const Fix& x, int n)
{  
  int32 a, carry = 0;
  Fix z;
  if (x.length == 0) return z;

  z = x;
  for (int i= x.words() - 1; i >= 0; i-- )
  {
     a = (int32)(uint32)x.Bits[i] * n + carry;
     z.Bits[i] = (uint16)a;
     carry = a >> BITS_PER_WORD;
  }
  if ((x.value()*n) > x.max() || (x.value()*n) < x.min())
     overflow_handler(z, (x.value()*n));
  return z;
}

Fix operator * (int n, const Fix& x) { return (x * n); }

Fix operator /  (const Fix& x, const Fix& y) 
{ 
  double temp = 0.0;
  int new_length  = x.length  + y.length;
  int new_intBits = x.intBits + y.intBits;
 
  if (y.value() != 0)
    temp = x.value()/y.value();
  else
    Error::abortRun("Fix: division by zero");
  Fix z(new_length, new_intBits, temp);

  return z;
}

Fix  Fix::operator += (const Fix& x)
{    *this = *this + x;
     return *this;
}

Fix  Fix::operator -= (const Fix& x)
{    *this = *this - x;
     return *this;
}

Fix  Fix::operator *= (const Fix& x)
{    *this = *this * x;
     return *this;
}

Fix  Fix::operator *= (int n)
{    *this = *this * n;
     return *this;
}

Fix  Fix::operator /= (const Fix& x) 
{    *this = *this / x;
     return *this;
}

////////////////////
// bitwise operators
////////////////////

Fix operator & (const Fix& x, const Fix& y)
{
   if ((x.length != y.length) && (x.intBits != y.intBits))
    Error::abortRun("Fix: incompatible precision parameters for operands in &");
   Fix r = x;
   for (int i=WORDS_PER_FIX - 1; i >= 0; i-- )
      r.Bits[i] = x.Bits[i] & y.Bits[i];
   return r;
}

Fix operator | (const Fix& x, const Fix& y)
{
   if ((x.length != y.length) && (x.intBits != y.intBits))
    Error::abortRun("Fix: incompatible precision parameters for operands in |");
   Fix r = x;
   for (int i=WORDS_PER_FIX - 1; i >= 0; i-- )
      r.Bits[i] = x.Bits[i] | y.Bits[i];
   return r;
}

Fix operator ~ (const Fix& x)
{   
    Fix r = x;
    for (int i=0; i < x.words(); i++) r.Bits[i] = ~r.Bits[i];
    Fix::mask_truncate(r.length, r.Bits);
    return r;
}

Fix operator << (const Fix& x, int n)
{   
    Fix r = x;
    ashift_bit_pattern(-n, r.Bits);
    if (n < r.intBits) r.intBits -= n;
    else               r.intBits  = 1;
    Fix::mask_truncate(r.length, r.Bits);
    return r;
}

Fix operator >> (const Fix& x, int n)
{   
    Fix r = x;
    ashift_bit_pattern(n, r.Bits);
    if (n <= (r.length - r.intBits)) r.intBits += n;
    else                             r.intBits  = r.length;
    Fix::mask_truncate(r.length, r.Bits);
    return r;
}

Fix Fix::operator <<= (int n)
{   
    ashift_bit_pattern(-n, Bits);
    if (n < intBits) intBits -= n;
    else             intBits  = 1;
    mask_truncate(length, Bits);
    return *this;
}
    
Fix  Fix::operator >>= (int n)
{   
    ashift_bit_pattern(n, Bits);
    if (n <= (length - intBits)) intBits += n;
    else                         intBits  = length;
    mask_truncate(length, Bits);
    return *this;
}

///////////////////////
// Comparison functions
///////////////////////

int Fix::operator == (const Fix& x)
{
    return value() == x.value();
}
int Fix::operator != (const Fix& x)
{
    return value() != x.value();
}
int Fix::operator <  (const Fix& x)
{
    return value() < x.value();
}
int Fix::operator <= (const Fix& x)
{
    return value() <= x.value();
}
int Fix::operator >  (const Fix& x)
{
    return value() > x.value();
}
int Fix::operator >= (const Fix& x)
{  
    return value() >= x.value();
}

////////////////////////
// Sign change operators
////////////////////////

Fix& Fix::operator +  () { return *this; }
Fix& Fix::operator -  () { complement(*this); return *this;}

////////////////////
// printing function
////////////////////

void printFix(const Fix& x)        // print contents
{   
  printf("Bits(hex) = ");
  for (int i=0; i<WORDS_PER_FIX; i++)
    printf("%04X ",x.Bits[i]);
  printf("\n");
  cout <<"length    = "<< x.len() << endl;
  cout <<"intBits   = "<< x.intb() << endl;
  cout <<"format    = "<< x.fixtype() << endl;
  cout <<"ovflow    = "<< x.overflow() << endl;
  cout <<"value     = "<< x.value() <<endl;
}

ostream& operator << (ostream& s, const Fix& x)
{
  s <<"("<< x.value() << ", "<<x.intb()<<"."<<(x.len()-x.intb())<<")"<<endl;
  return s;
}

////////////////////
// masking functions
////////////////////

void Fix::mask_truncate (int ln, uint16* bits)
{
  int NO_OF_WORDS = find_words(ln);
  for (int i = NO_OF_WORDS; i < WORDS_PER_FIX; i++) bits[i] = 0;
  int m = ln & 0x0f;
  if (m)  bits[NO_OF_WORDS-1] &= ( 0xffff0000 >> m );
}

////////////////////////////////////////////////////////////////
void Fix::mask_truncate_round (int ln, uint16* bits)
{
  int NO_OF_WORDS = find_words(ln);
  for (int i = NO_OF_WORDS; i < WORDS_PER_FIX; i++) bits[i] = 0;

  int m = ln & 0x0f;

  uint32 sum = 0, carry = 0;
  if (m){
    for ( int i=NO_OF_WORDS - 1; i >= 0; i-- )
    {
      sum = carry + (uint32 )bits[i] + (0x00000001 << (BITS_PER_WORD - m - 1));
      carry = sum >> 16;
      bits[i] = (uint16)sum;
    }
    bits[NO_OF_WORDS-1] &= (0xffff0000 >> m);}
}

////////////////////
// overflow handlers 
////////////////////

void overflow_handler (Fix& l, const Fix& r)
{
  if (l.overflow() == 0) {                   // saturate
     if (!l.fixtype()) {                     // signed
        if (r.signBit()) {
          l.Bits[0] = 0x8000;
          for (int i=1; i < l.words(); i++)  l.Bits[i] = 0; }
        else {
          l.Bits[0] = 0x7fff;
          for (int i=1; i < l.words(); i++)  l.Bits[i] = 0xffff; }
     }
     else {                                  //unsigned
        if (r.signBit()) 
          for (int i=0; i < l.words(); i++)  l.Bits[i] = 0; 
        else {
          l.Bits[0] = 0x7fff;
          for (int i=1; i < l.words(); i++)  l.Bits[i] = 0xffff; }
     }
     Fix::mask_truncate(l.length, l.Bits); }

  else if (l.overflow() == 1)                // zero_saturate
     for (int i=0; i < WORDS_PER_FIX; i++)  l.Bits[i] = 0; 

  else if (l.overflow() == 2) {              // wrapped
     if (!l.fixtype()) {                     // signed
        if (!r.signBit()) {
          l.Bits[0] = 0x8000;
          for (int i=1; i < l.words(); i++)  l.Bits[i] = 0; }
        else {
          l.Bits[0] = 0x7fff;
          for (int i=1; i < l.words(); i++)  l.Bits[i] = 0xffff; }
     }
     else {                                  // unsigned
        if (!r.signBit()) 
          for (int i=0; i < l.words(); i++)  l.Bits[i] = 0; 
        else {
          l.Bits[0] = 0x7fff;
          for (int i=1; i < l.words(); i++)  l.Bits[i] = 0xffff; }
     }
     Fix::mask_truncate(l.length, l.Bits); }

  else if (l.overflow() == 3) {             // warning
     Error::abortRun(" Fix : overflow error"); }

  else
     Error::abortRun(": overflow handling method not defined");
}

void overflow_handler (Fix& l, const double& d)
{
  Fix r(FIX_MAX_LENGTH, find_intBits(int(d)), d);
  overflow_handler (l, r);
}

///////////////////////////////////
// Parameter manipulation functions
///////////////////////////////////

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

mask_func_pointer Fix::Set_MASK(mask_func_pointer new_func) {
  mask_func_pointer old_func = MASK;
  MASK = new_func;
  return old_func;
}
