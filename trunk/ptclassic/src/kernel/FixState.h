#ifndef _FixState_h
#define _FixState_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "State.h"
#include "Fix.h"
#include "PrecisionState.h"

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

 Programmer:  A. Khazeni (replaces old version)
 Revisions:

 State with Fix type

**************************************************************************/

///////////////////////////////////////////
// class  FixState
///////////////////////////////////////////

class FixState : public State
{
public:
        // constructor
        FixState();

        // parses initValue to set value
        void initialize();

        // the type
	const char* type() const; // { return "FIX";}

        // the value as a string
	StringList currentValue() const;

        // assignment from a Fix
        Fix& operator=(const Fix& rvalue) { return val = rvalue;}

	// assignment from a FixState
	FixState& operator=(const FixState& arg) {
		val = arg.val; return *this;
	}

        // casting to a Fix
        operator Fix() { return val;}
	// convert to double (not done as cast because of ambiguity)
	double asDouble() const { return double(val);}

	// Return the precision of the Fix values;
	// the precision may contain symbolic expressions if it has been set by
	// the setPrecision() method; otherwise it will only consist of integer
	// constants.
	Precision precision() const;

	// Explicitly set the precision;
	// if overrideable is FALSE the symbolic expressions can not be redefined
	// in the future.
	void setPrecision(const Precision&, int overrideable=TRUE);

	// set init value
	void setInitValue(const Fix& arg);

	// this redeclaration is required by the "hiding rule".  Yuk!
	void setInitValue(const char* s) { State::setInitValue(s);}

	// class identification
	int isA(const char*) const;
	const char* className() const {return "FixState";}

	State* clone () const;//  {return new FixState;}

private:
	// the actual data
	Fix val;

	// symbolic representation of the fixed-point precision
	char *symbolic_length, *symbolic_intBits;
	// flag that indicates whether the symbolic representation is changeable
	int overrideablePrecision;
};


#endif
