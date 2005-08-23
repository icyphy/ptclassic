#ifndef _FloatState_h
#define _FloatState_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "State.h"

/**************************************************************************
Version identification:
@(#)ScalarState.hP	2.11	03/26/98

Copyright (c) 1990-1994 The Regents of the University of California.
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
 Date of creation: 6/15/89
 Revisions:

 State with double type

 IMPORTANT!!!!  If the name of this file is not ScalarState.hP, DO NOT
 EDIT IT!  The files IntState.h and FloatState.h are both generated from
 a template file by means of the "genclass" program.

**************************************************************************/

///////////////////////////////////////////
// class  FloatState
///////////////////////////////////////////


class FloatState : public State
{
public:
        // constructor
        FloatState();

        // parses initValue to set value
        void initialize();

        // the type
	const char* type() const; // { return "FLOAT";}

        // the value as a string
	StringList currentValue() const;

        // assignment from a double
        double operator=(double rvalue) { return val = rvalue;}

	// assignment between two FloatStates
        FloatState& operator=(const FloatState&  rhs)
        { this->val=rhs.val; return *this;}

        // casting to a double
        operator double() const { return val;}

	// set initvalue, with a double argument
	void setInitValue(double);

	// this redeclaration is required by the "hiding rule".  Yuk!
	void setInitValue(const char* s) { State::setInitValue(s);}

	// class identification
	int isA(const char*) const;
	const char* className() const {return "FloatState";}

	State* clone () const; //  {return new FloatState;}

protected:
	double val;
};	


#endif
