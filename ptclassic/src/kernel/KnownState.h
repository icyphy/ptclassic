/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda 
 Date of creation: 5/26/90

The KnownState class.

This class provides a list of all known states.
The data structure used to reference the list may be
changed.

The idea is that each star or galaxy that is "known to the system"
should add an instance of itself to the known list by code something
like

static MyType proto;
static KnownState entry(proto,"MyType");

Then the static method KnownBlock::clone(name) can produce a new
instance of class name.
*******************************************************************/
#ifndef _KnownState
#define  _KnownState 1

#include "StringList.h"
#include "State.h"
#include "Block.h"

class KnownState {

// The known state list.  It's a pointer only so we can control when
// the constructor is called.

        static StateList *allStates;
        static int numStates;
public:

// The constructor takes a state and a name, and adds a corresponding
// entry to the known list.
        KnownState (State &state, const char* name);

// The find method returns a pointer the appropriate state in
// allstates.
        static State* find (const char* name);

// The clone method takes a string, finds the appropriate state in
// allstates, and returns a clone of that block.
        static State* clone (const char* name);

// Return the names of all the known states.
        static StringList nameList();

// Return the number of known states.
        static int nKnown() { return numStates;}
};


#endif
