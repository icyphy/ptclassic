#ifndef _ComplexArrayState_h
#define _ComplexArrayState_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "State.h"

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

 Programmer:  I. Kuroda and J. T. Buck
 Date of creation: 6/8/90
 Revisions:

 State  with Complex type

**************************************************************************/

///////////////////////////////////////////
// class  ComplexArrayState
///////////////////////////////////////////

class ComplexArrayState : public State
{
public:
	// Constructor
	ComplexArrayState () {nElements = 0; val = 0;}

	// alternate constructor: size
	ComplexArrayState (int size);

	// alternate constructor: size and fill value
	ComplexArrayState (int size, const Complex& fill_value) ;

	// Destructor
	~ComplexArrayState ();

	// Size
	int size() const;

	// Array element 
	Complex & operator [] (int n) {
		return val[n];
	}

	// The type
	const char* type() const; // { return "ComplexArray";}

	// class identification
	int isA(const char*) const;
	const char* className() const;
	int isArray() const;

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
	Complex	*val;
};

#endif
