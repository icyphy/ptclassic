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

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

        // get the value as a string
	StringList currentValue() const;

	// for use as a string in stars
	operator const char* () { return val;}

	// various ways to initialize the current value
	// The StringList form is nonconst because it calls consolidate()
	StringState& operator=(const char* newStr);
	StringState& operator=(StringList& newStrList);
	StringState& operator=(const State& newState);

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
