#ifndef _StringArrayState_h
#define _StringArrayState_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "State.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 9/28/92
 Revisions:

 State with array-of-strings type 

**************************************************************************/


///////////////////////////////////////////
// class  StringArrayState
///////////////////////////////////////////

class StringArrayState : public State
{
public:
        // constructors and destructor
        StringArrayState() : nElements(0), val(0) {}
	StringArrayState(const StringArrayState&);
	~StringArrayState();

	// assignment
	StringArrayState& operator=(const StringArrayState&);

        // parses initValue to set value
        void initialize();

        // return the parameter type (for use in GUI, interpreter)
        const char* type() const;

        // the value as a string
	StringList currentValue() const;

	// Size
	int size() const; // { return nElements;}

	// Array element 
	const char* & operator [] (int n) {
		return val[n];
	}

	// class identification
	int isA(const char*) const;
	const char* className() const; // {return "StringArrayState";}
	int isArray() const;

	State* clone () const; // {return new StringArrayState;}

	// return the element number if the string appears in my current
	// value, -1 if it does not.
	int member(const char*) const;

private:
	// destructor body
	void zap();
	// copy constructor body
	void copy(const StringArrayState&);

	char** val;
	int nElements;
};	


#endif
