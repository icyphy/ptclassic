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

Then the static method KnownBlock::clone(name, dom) can produce a new
instance of the named class in the named domain.  As for blocks,
clone copies everything, makeNew just makes a new one.

KnownBlock::defaultDomain() returns the default domain name.  This is
not used internally for anything; it is just set to the first domain
seen during the building of known lists.

validDomain() returns TRUE if the argument is a valid domain.

We no longer have the concept of the "current domain".  We used to use
the "current domain" and repeatedly changed it; this is now going away
since it is troublesome, especially with multiple threads or with an X
interface where the flow of control is hard to follow.

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

// The find method returns a pointer the appropriate block in the domain
	static const Block* find (const char* name, const char* dom);

// The clone method takes a string, finds the appropriate block in
// the domain "dom" (the default domain if dom is null), and returns
// a clone of that block.
// makeNew just calls the constructor for the class and does not necessarily
// copy states.
	static Block* clone (const char* name, const char* dom);
	static Block* makeNew (const char* name, const char* dom);

// Return the names of known blocks in the given domain.
	static StringList nameList (const char* domain);

// Return the default domain name.
	static const char* defaultDomain();
// Change the default domain, return FALSE if invalid domain.
	static int setDefaultDomain(const char* newval);
// see if a domain is valid
	static int validDomain(const char* dom);

// Return true if the named block is dynamically linked.
	static int isDynamic (const char* type, const char* dom);
	
private:
	// code for default domain
	static int defaultDomainCode;

	// # of domains
	static int numDomains;

	// internal support functions.
	static StringList nameListForDomain(int idx);
	static int domainIndex (const char* dom, int addIfNotFound = 0);
	static int domainIndex (Block& block);
	static KnownListEntry* findEntry (const char*, KnownListEntry*);
	static void noMatch(const char* type, const char* dom);
};

// An iterator that steps through the knownlist for a particular domain

class KnownBlockIter {
private:
	KnownListEntry* pos;
	int idx;
public:
	// argument is the domain to use for the knownlist: default
	// is default domain.
	KnownBlockIter(const char* dom = 0);
	const Block* next();
	const Block* operator++() { return next();}
	void reset();
};

#endif
