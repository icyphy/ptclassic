#ifndef _Fix_h
#define _Fix_h 1

#ifdef __GNUG__
#pragma interface
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
        1/17/94  added constant definitions for types of overflow

This header file declares the class type Fix and its supporting 
functions. 

Fix type variables allow users to specify the number of bits and 
position of the binary point for the fixed-point representation of the 
variable. 

**************************************************************************/

class ostream;
typedef unsigned short uint16;
typedef unsigned long  uint32;
typedef unsigned char  uchar;
typedef long int32;
typedef void (*mask_func_pointer)(int, uint16*);

#define WORDS_PER_FIX   4         // maximum size of the array Bits
extern const int   FIX_MAX_LENGTH;

//  Types of overflow--- do not change (constants are indices into an array)
#define FIX_OVERFLOW_SATURATE		0
#define FIX_OVERFLOW_ZERO_SATURATE	1
#define FIX_OVERFLOW_WRAPPED		2
#define FIX_OVERFLOW_WARNING		3
#define TYPES_OF_FIX_OVERFLOW		4


//////////////////////////////
// Class Fix
//////////////////////////////

class Fix { 
private:

    uint16 Bits[WORDS_PER_FIX];   // The bit pattern that stores the value 
    uchar  length;                // # of significant bits 
    uchar  intBits;               // # of bits to the left of the binary point 
    uchar  format;                // 2's complement if 0, unsigned if 1.
    uchar  ovflow;                // overflow characteristic
    static mask_func_pointer MASK;// masking function.
    // internal function.
    static void make_bit_pattern(int,int,double,uint16*);
public:

    int len() const { return length; }
    int intb() const { return intBits; }
    int fixtype() const { return format; }
    int overflow() const { return ovflow; }

    int signBit() const;
    int words() const;
    double max() const;
    double min() const;
    double value() const;
    void initialize();
    void set_ovflow (const char*);

///////////////////////////////////
// Constructors

    Fix();
    Fix(int, int);
    Fix(int, int, const char*);
    Fix(const double&);
    Fix(int, int, const double&);
    Fix(int, int, const double&, const char*);
    Fix(const Fix&);
    Fix(int, int, const Fix&);

///////////////////////////////////
// Assignment operators

    Fix&          operator =  (const Fix&);
    Fix           operator =  (const double&);

///////////////////////////////////
// Arithmetic operators

    friend Fix    operator +  (const Fix&, const Fix&);
    friend Fix    operator -  (const Fix&, const Fix&);
    friend Fix    operator *  (const Fix&, const Fix&);
    friend Fix    operator *  (const Fix&, int);
    friend Fix    operator *  (int, const Fix&);
    friend Fix    operator /  (const Fix&, const Fix&);
    Fix           operator += (const Fix&);
    Fix           operator -= (const Fix&);
    Fix           operator *= (const Fix&);
    Fix           operator *= (int);
    Fix           operator /= (const Fix&);

///////////////////////////////////
// bitwise operators

    friend Fix     operator &  (const Fix&, const Fix&);
    friend Fix     operator |  (const Fix&, const Fix&);
    friend Fix     operator ~  (const Fix&);
    friend Fix     operator << (const Fix&, int);
    friend Fix     operator >> (const Fix&, int);
    Fix            operator <<=(int);
    Fix            operator >>=(int);

///////////////////////////////////
// Comparison functions

    int     operator == (const Fix&);
    int     operator != (const Fix&);
    int     operator <  (const Fix&);
    int     operator <= (const Fix&);
    int     operator >  (const Fix&);
    int     operator >= (const Fix&);

///////////////////////////////////
// other operators

    Fix&           operator +  ();
    Fix&           operator -  ();

    operator int () const;
    operator float () const;
    operator double () const;

    friend void	    printFix(const Fix&);	     //print contents
    friend void     overflow_handler(Fix&, const Fix&);
    friend void     overflow_handler(Fix&, const double&);
    friend void     complement(Fix&);

// utility functions
    static int  get_intBits(const char *p);
    static int  get_length(const char *p);

// possibilities for mask_truncate
    static void mask_truncate(int, uint16*);
    static void mask_truncate_round(int, uint16*);

    static mask_func_pointer Set_MASK(mask_func_pointer);
};

ostream& operator<<(ostream&, const Fix&);
#endif
