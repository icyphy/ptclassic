#ifndef _Fix_h
#define _Fix_h 1

#ifdef __GNUG__
#pragma interface
#endif

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  A. Khazeni, J. Buck
 Date of creation: 2/24/93 
 Revisions:
        1/17/94  added constant definitions for types of overflow
	1/20/94  major revisions: each Fix now keeps track of errors,
	         many bugs fixed
	1/21/94  now twos-complement only
	6/2/94	 invention of a second precision format "fracbits/length"

This header file declares the class type Fix and its supporting 
functions. 

Fix type variables allow users to specify the number of bits and 
position of the binary point for the fixed-point representation of the 
variable.  There are two ways to specify the precision:

Method 1: a pair of integer arguments, specifying the length in bits
          and "intBits", the number of bits to the left of the binary point.
	  The sign bit counts as one of the intBits, so this number must
	  be at least one.

Method 2: As a string like "3.2", or more generally "m.n", where m is
          intBits and n is the number to the right of the binary point.
	  Thus length is m+n.

Method 3: A string like "24/32" which means 24 fraction bits from a total
	  length of 32.  This format is more convenient when using precision
	  states, because the word length often remains constant while the
	  number of fraction bits changes with the normalization being used.

Arithmetic is done in such a way that operations like + do not overflow,
as a rule, unless the result cannot be represented with all FIX_MAX_LENGTH
bits left of the binary point.  Overflow occurs on assignment to a Fix
that has its format set, if the new result cannot fit.

The binary point must occur somewhere within the bits, or just to the
right of it.

By default, the effect of overflow is to saturate the result (use the
largest or smallest representable number) and set the overflow flag.
Overflow flags are passed along by the arithmetic operations + - * and /
to results, so you can keep track of whether any overflows have occured
anywhere by checking only the final result.  If either argument has overflow
set, the result has it set as well.  The ovf_occurred() method gives
access to the flag.

**************************************************************************/

class ostream;
class Precision;
typedef unsigned short uint16;

#if defined(PTALPHA)
typedef unsigned int uint32;
typedef int int32;
#else
typedef unsigned long  uint32;
#endif // PTALPHA

typedef unsigned char  uchar;
typedef long int32;

const int WORDS_PER_FIX = 4;
const int FIX_BITS_PER_WORD = 16;
const int FIX_MAX_LENGTH = WORDS_PER_FIX * FIX_BITS_PER_WORD;

//////////////////////////////
// Class Fix
//////////////////////////////

class Fix { 
public:
    // codes for overflow types
    enum { ovf_saturate = 0,
	   ovf_zero_saturate = 1,
	   ovf_wrapped = 2,
	   ovf_warning = 3,
	   ovf_n_types = 4
   };

    // error fields
    enum errorcode { err_ovf = 1, err_dbz = 2, err_invalid = 4 };

    // truncation-rounding codes, for backward compatibility
    enum mask { mask_truncate = 0, mask_truncate_round = 1 };

    // length in bits
    int len() const { return length; }

    // bits to right of binary point
    int intb() const { return intBits; }

    // bits to left of binary point
    int fracb() const { return length - intBits;}
    // obsolete
    int precision() const { return fracb(); }

    // return overflow type (one of the codes above)
    int overflow() const { return ovf_type;}

    // set the overflow type
    void set_overflow(int value) { ovf_type = value;}

    // set the rounding type: true for rounding, false for truncation
    void set_rounding(int value) { roundFlag = (value != 0);}

    // set the rounding type: synonym for backward compatibility
    void Set_MASK(int value) { set_rounding(value);}

    // return rounding mode (1 for rounding, 0 for truncation)
    int roundMode() const { return roundFlag;}

    // return TRUE for negative, false for + or 0
    int signBit() const { return (Bits[0] & 0x8000) != 0;}

    // return TRUE iff zero value
    int is_zero() const;

    // set to zero (equivalent to assigning zero)
    void setToZero();

    // max and min values representable in this format
    double max() const;
    double min() const;

    // value as a double
    double value() const;

    // discard precision information and zero
    void initialize();

    // set overflow using a name
    void set_ovflow (const char*);

///////////////////////////////////
// Constructors

    // create with unspecified precision
    Fix() { initialize();}

    // create a Fix with specified precision and zero value.
    Fix(int length, int intbits);
    Fix(const char * precisionString);
    Fix(const Precision&);

    // create a Fix with default precision
    Fix(double value);

    // create a Fix from the double with specified precision
    // We always round to the nearest Fix.
    Fix(int length, int intbits, double value);
    Fix(const char * precisionString, double value);
    Fix(const Precision&, double value);

    // create a Fix, specifying the bits precisely.
    // the first word of bits is the most significant.
    // from the "bits" argument: Fix("2.14", bits) will
    // only reference bits[0], for example.

    Fix(const char * precisionString, uint16* bits);

    // copy constructor: make exact duplicate.
    Fix(const Fix&);

    // copy value from Fix arg with new precision
    Fix(int length, int intbits, const Fix&);

///////////////////////////////////
// Assignment operators

    // assignment operator.  If *this does not have precision set, it is
    // copied, otherwise value is converted from existing precision.
    Fix&          operator =  (const Fix&);

    // assignment from double 
    Fix&          operator =  (double arg) {
	return *this = Fix(arg);
    }

///////////////////////////////////
// Arithmetic operators

    friend Fix    operator +  (const Fix&, const Fix&);
    friend Fix    operator -  (const Fix&, const Fix&);
    friend Fix    operator *  (const Fix&, const Fix&);
    friend Fix    operator *  (const Fix&, int);
    friend Fix    operator *  (int, const Fix&);
    friend Fix    operator /  (const Fix&, const Fix&);
    friend Fix    operator - (const Fix&); // unary minus

    Fix&          operator += (const Fix&);
    Fix&          operator -= (const Fix&);
    Fix&          operator *= (const Fix&);
    Fix&          operator *= (int);
    Fix&          operator /= (const Fix&);

///////////////////////////////////
// Comparison functions
    // return -1 if a<b, 0 if a==b, 1 if a>b
    friend int compare (const Fix& a, const Fix& b);

///////////////////////////////////
// access to errors
    int     ovf_occurred() const { return (errors & err_ovf) != 0;}
    int     invalid() const { return (errors & (err_dbz|err_invalid)) != 0;}
    int     dbz() const { return (errors & err_dbz) != 0;}

    void    clear_errors() { errors = 0; }
///////////////////////////////////
// other operators

    // convert to integer, truncating towards zero.
    operator int () const;

    // convert to float or double -- exact result where possible
    operator float () const { return float(value());}
    operator double () const { return value();}

    // debug-style printer
    friend void	    printFix(const Fix&);

    // replace arg by its negative
    void complement();

    // print on ostream in form (value, precision)
    friend ostream& operator<<(ostream&, const Fix&);

// utility functions.  Obsolete.
    static int  get_intBits(const char *p);
    static int  get_length(const char *p);

private:
    // Internal representation

    uint16 Bits[WORDS_PER_FIX];   // The bit pattern that stores the value 
    uchar  length;                // # of significant bits 
    uchar  intBits;               // # of bits to the left of the binary point 
    uchar  ovf_type;		  // fields specifying overflow type
    uchar  errors;		  // indicates whether overflow or errors have
				  // occurred in computing this value.
    uchar  roundFlag;		  // round on assignment if true

    // create bit pattern from the double value, using existing
    // length and intBits, rounding if round is nonzero.
    void makeBits(double value, int round);

    // apply truncation or rounding.
    void applyMask(int round);

    // treat types of overflow
    void overflow_handler(int resultSign, int shift);

    // parse a precision argument.  Return TRUE if valid else FALSE.
    int setPrecision(const char* precision);

    // # of words in internal representation
    int words() const {
	return (length <= FIX_BITS_PER_WORD ? 1 :
		(int)(length + FIX_BITS_PER_WORD - 1) / FIX_BITS_PER_WORD);
    }

    // # words in internal representation, possibly including an extra
    // bit for use before rounding
    int wordsIncludingRound(int round) const {
	int lpr = length + round;
	return (lpr <= FIX_BITS_PER_WORD ? 1 :
		(lpr + FIX_BITS_PER_WORD - 1) / FIX_BITS_PER_WORD);
    }

};

// comparision operators are in terms of compare.  compare checks all
// bits if the formats are the same, otherwise it converts to double,
// which only has 53 bits of precision on IEEE machines.

inline int operator == (const Fix& a, const Fix& b) {
    return compare(a, b) == 0;
}

inline int operator != (const Fix& a, const Fix& b) {
    return compare(a, b) != 0;
}

inline int operator < (const Fix& a, const Fix& b) {
    return compare(a, b) < 0;
}

inline int operator <= (const Fix& a, const Fix& b) {
    return compare(a, b) <= 0;
}

inline int operator > (const Fix& a, const Fix& b) {
    return compare(a, b) > 0;
}

inline int operator >= (const Fix& a, const Fix& b) {
    return compare(a, b) >= 0;
}

#endif
