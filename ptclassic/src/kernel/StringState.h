#ifndef _StringState_h
#define _StringState_h 1

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
        StringState() : val(0) {}

        // parses initValue to set value
        void initialize();

	// don't redefine type(), baseclass version returns "STRING"

        // the value as a string
	StringList currentValue() const;

	// for use as a string in stars
	operator const char* () { return val;}

	State* clone () const; // {return new StringState;}

private:
	const char* val;
};	


#endif
