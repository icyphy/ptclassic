#ifndef _ComplexArrayState_h
#define _ComplexArrayState_h 1

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
