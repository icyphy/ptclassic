static const char file_id[] = "KnownState.cc";
/******************************************************************
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

 Programmer:  I. Kuroda
 Date of creation: 5/26/90

Methods for the KnownState class.  See KnownState.h for a full
description.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "KnownState.h"
#include "Error.h"
#include <std.h>
#include <ctype.h>

StateList *KnownState::allStates;       // the list of state types
StateList *KnownState::allGlobals;	// the list of global state values

int KnownState::numStates = 0;          // define the number of state types
int KnownState::numGlobals = 0;		// and the number of global values

// Constructor.  Adds a state to the known list

KnownState::KnownState (State &state, const char* name) {
// on the first call, create the known state list.
// It's done this way to get around the order-of-static-constructors problem.
        if (numStates == 0) {
                LOG_NEW; allStates = new StateList;
	}
        numStates++;
// set my name and add to the list
        state.setState (name,NULL,"");
        allStates->put (state);
}

KnownState::KnownState (State& state, const char* name, const char* value) {
	if (numGlobals == 0) {
		LOG_NEW; allGlobals = new StateList;
	}
	numGlobals++;
	state.setState (name,NULL,value);
	state.initialize();
	allGlobals->put (state);
}

const State*
KnownState::find(const char* type) {
/* convert specified type to uppercase */
	char upcaseType[32], *t = upcaseType, c;
	while ((c = *type++) != 0) {
		if (islower(c)) *t++ = toupper(c);
		else *t++ = c;
	}
	*t = 0;
        return numStates == 0 ? NULL : allStates->stateWithName(upcaseType);
}

const State*
KnownState::lookup(const char* name) {
	return numGlobals == 0 ? NULL : allGlobals->stateWithName(name);
}

// The main cloner.  This method gives us a new state of the named
// type, by asking the matching state on the list to clone itself.

State *
KnownState::clone(const char* type) {
        const State *p = find(type);
        if (p) return p->clone();
// If we get here, we don't know the state.  Report error, return NULL.
        Error::error("KnownState::clone: unknown state name: ",type)
;
        return 0;
}

// Return known list as text, separated by linefeeds
StringList
KnownState::nameList () {
        StringList s;
        if (numStates > 0) {
		StateListIter next(*allStates);
                for (int i=numStates; i>0; i--) {
                        State* t = next++;
                        s += t->name();
                        s += "\n";
                }
        }
        return s;
}

// Here is a short, standard global symbol list.  By putting this
// here we always get them.
#include "IntState.h"
#include "FloatState.h"

static FloatState pi;
KnownState k_pi(pi,"PI","3.14159265358979323846");

const char one[] = "1";
const char zero[] = "0";

static IntState true;
KnownState k_true(true,"TRUE",one);

static IntState yes;
KnownState k_yes(yes,"YES",one);

static IntState false;
KnownState k_false(false,"FALSE",zero);

static IntState no;
KnownState k_no(no,"NO",zero);

// the following discards the states at the end.
class KnownStateOwner {
public:
	// constructor only here because of g++ bug -- it won't call
	// the destructor otherwise.
	KnownStateOwner () {}
	~KnownStateOwner () {
		LOG_DEL; delete KnownState::allStates;
		LOG_DEL; delete KnownState::allGlobals;
	}
};

static KnownStateOwner kso;
