#ifndef _ComplexState_h
#define _ComplexState_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "State.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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
