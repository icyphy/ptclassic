#ifndef _ComplexState_h
#define _ComplexState_h 1

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
 Date of creation: 6/15/90
 Revisions:

 State with Complex type

**************************************************************************/

///////////////////////////////////////////
// class  ComplexState
///////////////////////////////////////////

class ComplexState : public State
{
public:
        // constructor
        ComplexState() { val = 0;}

        // parses initValue to set value
        void initialize();

        // the type
	const char* type() const; // { return "COMPLEX";}

        // the value as a string
	StringList currentValue() const;

        // assignment from a Complex
        Complex& operator=(const Complex& rvalue) { return val = rvalue;}

        // casting to a Complex
        operator Complex() { return val;}

	// set init value
	void setInitValue(const Complex& arg);

	// this redeclaration is required by the "hiding rule".  Yuk!
	void setInitValue(const char* s) { State::setInitValue(s);}

	// class identification
	int isA(const char*) const;
	const char* className() const {return "ComplexState";}

	State* clone () const;//  {return new ComplexState;}

private:
	// the actual data
	Complex val;
};


#endif
