#ifndef _StringArrayState_h
#define _StringArrayState_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "State.h"

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
	const char* operator [] (int n) {
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
