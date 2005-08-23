#ifndef _IntState_h
#define _IntState_h 1

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

 State with int type

 IMPORTANT!!!!  If the name of this file is not ScalarState.hP, DO NOT
 EDIT IT!  The files IntState.h and FloatState.h are both generated from
 a template file by means of the "genclass" program.

**************************************************************************/

///////////////////////////////////////////
// class  IntState
///////////////////////////////////////////


class IntState : public State
{
public:
        // constructor
        IntState();

        // parses initValue to set value
        void initialize();

        // the type
	const char* type() const; // { return "INT";}

        // the value as a string
	StringList currentValue() const;

        // assignment from a int
        int operator=(int rvalue) { return val = rvalue;}

	// assignment between two IntStates
        IntState& operator=(const IntState&  rhs)
        { this->val=rhs.val; return *this;}

        // casting to a int
        operator int() const { return val;}

	// set initvalue, with a int argument
	void setInitValue(int);

	// this redeclaration is required by the "hiding rule".  Yuk!
	void setInitValue(const char* s) { State::setInitValue(s);}

	// class identification
	int isA(const char*) const;
	const char* className() const {return "IntState";}

	State* clone () const; //  {return new IntState;}

protected:
	int val;
};	


#endif
