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

static const MyType proto;
static KnownState entry(proto,"MyType");

Then the static method KnownBlock::clone(name) can produce a new
instance of class name.
*******************************************************************/
#ifndef _KnownState
#define  _KnownState 1

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "State.h"
#include "Block.h"

class KnownState {
	friend class KnownStateOwner;

// The known state list.  It's a pointer only so we can control when
// the constructor is called.

        static StateList *allStates;
	static StateList *allGlobals;
        static int numStates;
	static int numGlobals;
public:

// The constructor takes a state and a name, and adds a corresponding
// entry to the known list.
        KnownState (State &state, const char* name);

// This constructor takes a state, a name, and a value, and adds a
// corresponding entry to the global symbol list.
	KnownState (State& state, const char* name, const char* value);

// The find method returns a pointer the appropriate state in
// allstates.
        static const State* find (const char* type);

// The lookup method returns a pointer to the appropriate state in
// allGlobals.
	static const State* lookup (const char* name);

// The clone method takes a string, finds the appropriate state in
// allstates, and returns a clone of that block.
        static State* clone (const char* type);

// Return the names of all the known state types.
        static StringList nameList();

// Return the number of known states.
        static int nKnown() { return numStates;}
};


#endif
