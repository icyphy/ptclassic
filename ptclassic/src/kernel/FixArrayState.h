#ifndef _FixArrayState_h
#define _FixArrayState_h 1

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

 Programmer: A. Khazeni 
 Date of creation: 2/24/93 
 Revisions:

 State  with Fix type

**************************************************************************/

///////////////////////////////////////////
// class  FixArrayState
///////////////////////////////////////////

class FixArrayState : public State
{
public:
	// Constructor
	FixArrayState ();

	// alternate constructor: size
	FixArrayState (int size);

	// alternate constructor: size and fill value
	FixArrayState (int size, const Fix& fill_value) ;

	// Destructor
	~FixArrayState ();

	// Size
	int size() const;

	// Array element 
	Fix & operator [] (int n) {
		return val[n];
	}

	// The type
	const char* type() const; // { return "FixArray";}

	// class identification
	int isA(const char*) const;
	const char* className() const;
	int isArray() const;

	// Return the precision of the Fix values;
	// the precision may contain symbolic expressions if it has been set by
	// the setPrecision() method; otherwise it will only consist of integer
	// constants.
	// It is assumed that all array elements have the same precision;
	// this is usually the case unless the precision of an individual element 
	// is changed using the return value of the [] operator (not recommended)
	Precision precision() const;

	// Explicitly set the precision;
	// if overrideable is FALSE the symbolic expressions can not be redefined
	// in the future.
	void setPrecision(const Precision&, int overrideable=TRUE);

        // the value as a string
        StringList currentValue() const;

	// Parse initValue to set Value
	void initialize();

	// Truncate/extend to new size
	void resize(int);

	// Parse element
	ParseToken evalExpression(Tokenizer&);

	// clone method
	virtual State* clone() const;
protected:
	int	nElements;
	Fix	*val;

	// symbolic representation of the fix-point precision
	char *symbolic_length, *symbolic_intBits;
	// flag that indicates whether the symbolic representation is changeable
	int overrideablePrecision;
};

#endif
