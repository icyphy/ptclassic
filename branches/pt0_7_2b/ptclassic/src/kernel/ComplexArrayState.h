#ifndef _ComplexArrayState_h
#define _ComplexArrayState_h 1

#include "State.h"
#include "ComplexState.h"
#include "KnownState.h"


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
protected:
	int	nElements;
	Complex	*val;
public:
	// Constructor
	ComplexArrayState () {nElements = 0; val = 0;}

	// Constructor
	ComplexArrayState (int size) { val = new Complex [nElements = size];}

	// Constructor
	ComplexArrayState (int size, Complex& fill_value) 
				{ val = new Complex [nElements = size];
				Complex * top = &(val[nElements]);
				Complex * t = val;
				while (t < top) *t++ = fill_value; }

	// Destructor
	~ComplexArrayState () {delete[nElements] val;}

	// Assignment operator
	ComplexArrayState &	operator = (ComplexArrayState & v) {
				if (this != &v) {
				delete[nElements] val;
				val  = new Complex [nElements = v.nElements];
				Complex* top = &(val[nElements]);
				Complex* t = val;
				Complex* u = v.val;
				while (t < top) *t++ = *u++;
				}
				return *this;
			}

	// Size
	int size() { return nElements;}

	// Array element 
	Complex & operator [] (int n) {
				return val[n];
			}

	// The type
	char* type() { return "ComplexArray";}

        // the value as a string
        StringList currentValue() { 
		StringList s; 
		s =  "\n";
		for(int i = 0; i<size(); i++) 
		{s = i; 
		s = "	("; 
		s = val[i].real();
		s = ","; 
		s = val[i].imag(); 
		s= ")\n";} 
		return s;}

	// Parse initValue to set Value
	void initialize();

	// Parse element
	ParseToken evalExpression(Tokenizer&, Block*);

	// clone method
	virtual State* clone() {return new ComplexArrayState;}
};

#endif