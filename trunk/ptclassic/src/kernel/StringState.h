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
	const char* readClassName() const; // {return "StringState";}

	State* clone () const; // {return new StringState;}

	~StringState ();
private:
	char* val;
};	


#endif
