#ifndef _StringState_h
#define _StringState_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "State.h"
#include "KnownState.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: I. Kuroda
 Date of creation: 5/19/90
 Revisions:

 State with String type 

**************************************************************************/


///////////////////////////////////////////
// class  StringState
///////////////////////////////////////////

class StringState : public State
{
public:
        // constructor
        StringState();

        // parses initValue to set value
        void initialize();

	// don't redefine type(), baseclass version returns "STRING"

        // the value as a string
	StringList currentValue() const;

	// for use as a string in stars
	operator const char* () { return val;}

	// class identification
	int isA(const char*) const;
	const char* className() const; // {return "StringState";}
	const char* type() const;

	State* clone () const; // {return new StringState;}

	// test for empty string
	int null() const { return val == 0 || *val == 0;}

	~StringState ();
private:
	char* val;
};	


#endif
