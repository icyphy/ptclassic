#ifndef _ComplexState_h
#define _ComplexState_h 1

#include "State.h"
#include "KnownState.h"

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
        char* type(); // { return "COMPLEX";}

        // the value as a string
	StringList currentValue();

        // assignment from a Complex
        Complex operator=(const Complex& rvalue) { return val = rvalue;}

        // casting to a Complex
        operator Complex() { return val;}

	// Evaluate expression  in string
	ParseToken evalExpression(Tokenizer& lexer, Block* blockIAmIn);	

	State* clone ();//  {return new ComplexState;}

private:
	friend class ComplexArrayState;

	Complex val;
};	


#endif
