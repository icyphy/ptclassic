#ifndef _Attribute_h
#define _Attribute_h 1
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer: J. T. Buck 
 Date of creation: 6/12/91
 Revisions:

 Attributes, for use in controlling bitmasks in States and PortHoles.

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

typedef unsigned long bitWord;

// Class Attribute is used to specify a set of bits that must be on
// and a set of bits that must be off in a bitWord.

// Attribute objects are initialized like structs, as in
// Attribute foo = { bitsOn, bitsOff };
// to avoid order-of-constructor problems.

struct Attribute {
// these should be treated as private
	bitWord bitsOn;
	bitWord bitsOff;
public:
	// Specify the combination of two attributes.
	// This combination means that requirements of both attributes
	// must be satisfied.  Hence, it really should be called an "and"
	// operation.  The name reflects what happens to the masks.
	Attribute& operator |= (const Attribute& a) {
		bitsOn |= a.bitsOn;
		bitsOff |= a.bitsOff;
		return *this;
	}

	// Specify the combination of two attributes.
	// This combination means that requirements of either attribute
	// must be satisfied.  Hence, it really should be called an "or"
	// operation.  The name reflects what happens to the masks.
	Attribute& operator &= (const Attribute& a) {
		bitsOn &= a.bitsOn;
		bitsOff &= a.bitsOff;
		return *this;
	}

	// evaluate an attribute given a default value.  Note that
	// in the event of a conflict, bitsOff wins.
	bitWord eval(bitWord defaultVal) const {
		return (defaultVal | bitsOn) & ~bitsOff;
	}

	// evaluate an attribute and add it, backwards.
	// to implement clearAttribute function.

	bitWord clearAttribs(bitWord defaultVal) const {
		return (defaultVal | bitsOff) & ~bitsOn;
	}

	// retrieve fields.
	bitWord on() const { return bitsOn;}
	bitWord off() const { return bitsOff;}
};

// This combination means that requirements of both attributes
// must be satisfied.  Hence, it really should be called an "and"
// operation.  The name reflects what happens to the masks.
inline Attribute operator | (const Attribute& a, const Attribute& b) {
	Attribute t = a;
	return t |= b;
}

// This combination means that requirements of either attribute
// must be satisfied.  Hence, it really should be called an "or"
// operation.  The name reflects what happens to the masks.
inline Attribute operator & (const Attribute& a, const Attribute& b) {
	Attribute t = a;
	return t &= b;
}
#endif
