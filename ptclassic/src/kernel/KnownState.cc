/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda
 Date of creation: 5/26/90

Methods for the KnownState class.  See KnownState.h for a full
description.

*******************************************************************/
#include "KnownState.h"
#include "Output.h"
#include "string.h"
#include "std.h"

extern Error errorHandler;

StateList *KnownState::allStates;       // define the static member

int KnownState::numStates = 0;          // define the number of states

// Constructor.  Adds a state to the known list

KnownState::KnownState (State &state, const char* name) {
// on the first call, create the known state list.
// It's done this way to get around the order-of-static-constructors problem.
        if (numStates == 0)
                allStates = new StateList;
        numStates++;
// set my name and add to the list
        state.setState (name,NULL,"");
        allStates->put (&state);
}

State*
KnownState::find(const char* type) {
        return numStates == 0 ? NULL : allStates->stateWithName(type);
}

// The main cloner.  This method gives us a new state of the named
// type, by asking the matching state on the list to clone itself.

State *
KnownState::clone(const char* type) {
        State *p = find(type);
        if (p) return p->clone();
// If we get here, we don't know the state.  Report error, return NULL.
        errorHandler.error("KnownState::clone: unknown state name: ",type)
;
        return 0;
}

// Return known list as text, separated by linefeeds
StringList
KnownState::nameList () {
        StringList s;
        if (numStates > 0) {
                allStates->reset();
                for (int i=numStates; i>0; i--) {
                        State& t = (*allStates)++;
                        s += t.readName();
                        s += "\n";
                }
        }
        return s;
}

