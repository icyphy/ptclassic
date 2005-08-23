/******************************************************************
Version identification:
@(#)KnownBlock.h	2.16 02/05/97

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  J. T. Buck
 Date of creation: 3/16/90

The KnownBlock class.

This class provides a list of all known blocks.  There is a separate
list for each domain (corresponding to blocks that may be run by
a specific scheduler).

Each star or galaxy that is defined in C++ code (as opposed to being
built up by an interpreter) should add an instance of itself to the known
list by code something like

static MyType proto;
static RegisterBlock entry(proto,"MyType");

The RegisterBlock constructor is just a call to KnownBlock::addEntry.

Dynamically constructed block types, such as interpreted galaxies,
are added to the known list with a direct call to KnownBlock::addEntry.
These cases should always supply an appropriate definition-source
string so that conflicting block type definitions can be detected.

Once a knownlist entry exists, the static method KnownBlock::clone(name, dom)
can produce a new instance of the named class in the named domain.
clone copies everything, makeNew just makes a new one.  Recursive
clone attempts are detected to avoid a crash in the event of a self-
referential galaxy definition.

KnownBlock::defaultDomain() returns the default domain name.  This is
not used internally for anything; it is just set to the first domain
seen during the building of known lists.

validDomain() returns TRUE if the argument is a valid domain.

We no longer have the concept of the "current domain".  We used to use
the "current domain" and repeatedly changed it; this is now going away
since it is troublesome, especially with multiple threads or with an X
interface where the flow of control is hard to follow.

KnownBlock assigns a sequential serial number to each definition or
redefinition of a known block type.  This can be used, for example,
to determine whether a galaxy has been compiled more recently than
any of its constituent blocks.

KnownBlock also keeps track of the source of the definition of every
known block type.  This allows compile.c to determine whether an Oct
facet needs to be recompiled (without the source information, different
facets that have the same base name could be mistaken for each other).
This also allows us to generate some helpful warning messages when
a block name is accidentally re-used.  The source location information
is currently rather crude for everything except Oct facets, but that's
good enough to generate a useful warning in nearly all cases.

*******************************************************************/
#ifndef _KnownBlock_h
#define  _KnownBlock_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Block.h"
#include "StringList.h"
#include "Linker.h"

// private class for entries on known lists

class KnownListEntry {
	friend class KnownBlock;
	friend class KnownBlockIter;
	Block* b;
	long serialNumber;	// serial# of this knownblock definition
	int onHeap;		// T => *b should be freed
	int dynLinked;		// T => star defined by dynamically linked code
	const char* definitionSource;
	// definitionSource is:
	// NULL for a built-in block type (anything defined by C++ code);
	// hashstring'ed pathname of defining Oct facet for a compiled galaxy;
	// or a special case constant string, currently one of
	// "dynamically linked star" or "ptcl defgalaxy command"

	int cloneInProgress;	// true while trying to clone this block
	KnownListEntry *next;

	// serial# last assigned
	static long lastSerialNumber;

public:
	KnownListEntry(Block* bl, int oh, const char* ds, KnownListEntry* n) :
		b(bl), serialNumber(++lastSerialNumber),
		onHeap(oh), dynLinked(Linker::isActive()),
		definitionSource(ds), cloneInProgress(0), next(n) {}
	~KnownListEntry ();
	void redefine(Block* bl, int oh, const char* ds);
};

class KnownBlock {
	friend class KnownBlockIter;
public:
// The constructor takes a block and a name, and adds a corresponding
// entry to the known list.
	KnownBlock (Block &block, const char* name) {
		addEntry (block, name, 0, NULL);
	}

// This function actually adds the block to the list.  If onHeap is true,
// the block will be destroyed when the entry is removed or replaced from
// the list.  definitionSource should be NULL for any block type defined
// by C++ code, or a hashstring'ed path name for a block defined by an
// identifiable file (such as an Oct facet), or a special case constant
// string for other cases such as the ptcl defgalaxy command.
	static void addEntry (Block &block, const char* name, int onHeap,
			      const char* definitionSource);

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

// If there is a known block of the given name and domain,
// return TRUE and pass back its definition source string.
	static int isDefined (const char* type, const char* dom,
			      const char* &definitionSource);

// Return true if the named block is dynamically linked.
	static int isDynamic (const char* type, const char* dom);
	
// Look up a KnownBlock definition by name and return its serial number.
// Returns 0 iff no matching definition exists.
	static long serialNumber (const char* name, const char* dom);

// Given a block, find the matching KnownBlock definition,
// and return its serial number (or 0 if no matching definition exists).
	static long serialNumber (Block& block);

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
	static KnownListEntry* findEntry (const char* name, const char* dom);
	static KnownListEntry* findEntry (Block& block);
	static void noMatch(const char* type, const char* dom);
	static void recursiveClone(const char* type, const char* dom);
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
	inline const Block* next() {
	  if (pos) {
	    const Block* b = pos->b;
	    pos = pos->next;
	    return b;
	  } else return 0;
	}
	inline const Block* operator++(POSTFIX_OP) { return next();}
	void reset();
};

#endif
