/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/16/90

The KnownBlock class.

This class provides a list of all known blocks.  There is a separate
list for each domain (corresponding to blocks that may be run by
a specific scheduler).

This class also provides a list of all known schedulers.

The idea is that each star or galaxy that is "known to the system"
should add an instance of itself to the known list by code something
like

static MyType proto;
static KnownBlock entry(proto,"MyType");

Then the static method KnownBlock::clone(name) can produce a new
instance of class name.  Similarly, schedulers do the exact same
thing:

static MySched proto;
static KnownBlock entry(proto,"MyDomain");

*******************************************************************/
#ifndef _KnownBlock_h
#define  _KnownBlock_h 1

#include "Block.h"
#include "Galaxy.h"
#include "StringList.h"

class Scheduler;

class KnownListEntry {
	friend class KnownBlock;
	Block* b;
	KnownListEntry *next;
};

const int NUMDOMAINS = 10;

class KnownBlock {
private:
// The known block lists, one per domain
	static KnownListEntry* allBlocks[NUMDOMAINS];
// The set of domain names
	static const char* domainNames[NUMDOMAINS];
// A scheduler for each domain
	static Scheduler* allSchedulers[NUMDOMAINS];

	static int currentDomain;
	static int numDomains;

	static StringList nameListForDomain(int idx);
	static int domainIndex (const char* dom, int addIfNotFound = 0);
	static int domainIndex (Block& block);
	static KnownListEntry* findEntry (const char*, KnownListEntry*);
public:
// The constructor takes a block and a name, and adds a corresponding
// entry to the known list.
	KnownBlock (Block &block, const char* name);

// Alternate constructor, creates a scheduler entry.
	KnownBlock (Scheduler& sched, const char* domain);

// The find method returns a pointer the appropriate block in
// the current domain.
	static Block* find (const char* name);

// The clone method takes a string, finds the appropriate block in
// the current domain, and returns a clone of that block.
	static Block* clone (const char* name);

// The newSched method takes a string and returns a scheduler of the
// given type.
	static Scheduler* newSched (const char* name);

// Argument-less version gives the one for the current domain.
	static Scheduler* newSched ();

// Return the names of known blocks in the current domain.
	static StringList nameList();

// Return the names of knownn blocks in the given domain.
	static StringList nameList (const char* domain);

// Return the current domain name.
	static const char* domain() { return domainNames[currentDomain];}

// Change the current domain.  Return TRUE if it worked, FALSE for bad name.
	static int setDomain (const char* newDom);

};

#endif
