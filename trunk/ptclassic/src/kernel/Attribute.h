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

typedef unsigned long bitWord;

class Attribute {
private:
	bitWord bitsOn;
	bitWord bitsOff;
public:
	// constructor
	Attribute(bitWord on, bitWord off) : bitsOn(on), bitsOff(off) {}

	// specify the combination of two attributes
	Attribute& operator |= (const Attribute& a) {
		bitsOn |= a.bitsOn;
		bitsOff |= a.bitsOff;
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

inline Attribute operator | (const Attribute& a, const Attribute& b) {
	Attribute t(a);
	return t |= b;
}
#endif
