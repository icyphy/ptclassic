#ifndef _Attribute_h
#define _Attribute_h 1
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

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
