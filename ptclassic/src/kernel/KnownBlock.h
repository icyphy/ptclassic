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

The idea is that each star or galaxy that is "known to the system"
should add an instance of itself to the known list by code something
like

static MyType proto;
static KnownBlock entry(proto,"MyType");

Then the static method KnownBlock::clone(name) can produce a new
instance of the named class in the current domain.  As for blocks,
clone copies everything, makeNew just makes a new one.

*******************************************************************/
#ifndef _KnownBlock_h
#define  _KnownBlock_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Block.h"
#include "StringList.h"

class KnownListEntry;

class KnownBlock {
	friend class KnownBlockIter;
private:
	static int currentDomain;
	static int numDomains;

	static StringList nameListForDomain(int idx);
	static int domainIndex (const char* dom, int addIfNotFound = 0);
	static int domainIndex (Block& block);
	static KnownListEntry* findEntry (const char*, KnownListEntry*);
public:
// The constructor takes a block and a name, and adds a corresponding
// entry to the known list.
	KnownBlock (Block &block, const char* name) {
		addEntry (block, name, 0);
	}

// This function actually adds the block to the list.  If onHeap is true,
// the block will be destroyed when the entry is removed or replaced from
// the list.
	static void addEntry (Block &block, const char* name, int onHeap);

// The find method returns a pointer the appropriate block in
// the current domain.
	static const Block* find (const char* name);

// The clone method takes a string, finds the appropriate block in
// the current domain, and returns a clone of that block.
// makeNew just calls the constructor for the class and does not necessarily
// copy states.
	static Block* clone (const char* name);
	static Block* makeNew (const char* name);

// Return the names of known blocks in the current domain.
	static StringList nameList();

// Return the names of knownn blocks in the given domain.
	static StringList nameList (const char* domain);

// Return the current domain name.
	static const char* domain();

// Change the current domain.  Return TRUE if it worked, FALSE for bad name.
	static int setDomain (const char* newDom);

// Return true if the named block is dynamically linked.
	static int isDynamic (const char* type);
	
};

// An iterator that steps through the knownlist for a particular domain

class KnownBlockIter {
private:
	KnownListEntry* pos;
	int idx;
public:
	KnownBlockIter(const char* dom = 0);
	const Block* next();
	const Block* operator++() { return next();}
	void reset();
};

#endif
