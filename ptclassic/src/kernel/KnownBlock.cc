static const char file_id[] = "KnownBlock.cc";
/******************************************************************
Version identification:
@(#)KnownBlock.cc	2.25 12/08/97

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

Methods for the KnownBlock class.  See KnownBlock.h for a full
description.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "KnownBlock.h"
#include "Error.h"
#include <std.h>
#include "Domain.h"
#include "Star.h"
#include "Wormhole.h"


KnownListEntry::~KnownListEntry () {
	if (onHeap) { LOG_DEL; delete b;}
	LOG_DEL; delete next;
}

inline
void KnownListEntry::redefine (Block* bl, int oh, const char* ds) {
	// Delete the old block if it was on the heap
	if (onHeap) { LOG_DEL; delete b;}
	// and install the new data
	b = bl;
	serialNumber = ++lastSerialNumber;
	onHeap = oh;
	dynLinked = Linker::isActive();
	definitionSource = ds;
}


// declarations for static variables.  I would rather use class
// static than file static for the arrays, but g++ has bugs.

long KnownListEntry :: lastSerialNumber = 0L;
static KnownListEntry* allBlocks[NUMDOMAINS];
static const char* domainNames[NUMDOMAINS];
int KnownBlock :: defaultDomainCode = 0;
int KnownBlock :: numDomains = 0;

// Special class to clean up at end.
class KnownListOwner {
public:
	KnownListOwner() {}
	~KnownListOwner() {
		for (int i = 0; i < NUMDOMAINS; i++) {
			LOG_DEL; delete allBlocks[i];
		}
	}
};

static KnownListOwner knownListOwner;

static const char msg[] =
"No domains can be located by Ptolemy; something is wrong with your\n"
"installation of either Ptolemy (no domains were included, perhaps?) or\n"
"of your C++ compiler and linker (if global constructors aren't being called\n"
"properly, we also get this symptom).  Exiting...\n";

// Function to abort program if there are no known domains.
static void bombNoDomains() {
	Error::error(msg);
	exit(1);
}
		
// KnownBlock methods

const char* KnownBlock::defaultDomain()  {
	if (numDomains == 0) bombNoDomains();
	return domainNames[defaultDomainCode];
}

int KnownBlock::setDefaultDomain(const char* newDom) {
	int idx = domainIndex(newDom,0);
	if (idx >= 0) {
		defaultDomainCode = idx;
		return TRUE;
	}
	Error::abortRun("No such domain: ",newDom);
	return FALSE;
}

int KnownBlock::validDomain(const char* newDom) {
	return newDom && domainIndex(newDom,0) >= 0;
}
	
// This function looks up a domain.  It returns -1 if not found and
// we specified no adding.  For a null argument, we always return -1.

int KnownBlock::domainIndex (const char* myDomain, int addIfNotFound) {
        int i;
	if (myDomain == 0) return -1;
	for (i = 0; i < numDomains; i++) {
		if (strcmp (domainNames[i], myDomain) == 0)
			return i;
	}
	if (addIfNotFound) {
		if (numDomains == NUMDOMAINS) {
			Error::abortRun("Too many distinct domains");
			return -1;		
		}
		else {
			i = numDomains;
			numDomains++;
			domainNames[i] = myDomain;
			return i;
		}
	}
	else return -1;
}

// Look up the domain index of a block.

inline int KnownBlock::domainIndex (Block& block) {
	return domainIndex (block.domain(), TRUE);
}

// Add a block to the appropriate known list

static const char dynLinkedStar[] = "dynamically linked star";

void KnownBlock::addEntry (Block& block, const char* name, int onHeap,
			   const char* definitionSource) {
	// set my name
	block.setBlock (name, NULL);

	// For a star definition, check to see if it's coming from
	// dynamically linked code.  We do NOT use Linker::isActive()
	// because it lies about permlinks.
	if (definitionSource == NULL && Linker::doingDynLink())
		definitionSource = dynLinkedStar;

	// get domain index.  If undefined use lastDomain
	int idx = domainIndex (block);

	// see if defined; if so, replace
	KnownListEntry* kb = findEntry (block.name(), allBlocks[idx]);
	if (kb) {
		// Issue a warning message if it seems appropriate
		const char* oldDef = kb->definitionSource;
		if (oldDef == NULL && definitionSource == NULL) {
		  StringList msg = "Multiple built-in definitions for block named '";
		  msg << name << "' in domain '" << domainNames[idx] << "'";
		  Error::warn (msg);
		  // note: is it safe to use Error::warn during startup???
		}
		else if (oldDef == NULL) {
		  StringList msg = "Built-in star named '";
		  msg << name << "' in domain '" << domainNames[idx] << "'";
		  msg << " is being overridden by " << definitionSource;
		  Error::warn (msg);
		}
		// needn't use strcmp since variable strings will be hashed
		else if (oldDef != definitionSource) {
		  StringList msg = "Star/galaxy named '";
		  msg << name << "' in domain '" << domainNames[idx] << "'";
		  msg << " was defined by " << oldDef;
		  msg << " but is being overridden by ";
		  msg << (definitionSource ? definitionSource : "built-in star");
		  Error::warn (msg);
		}
		// Install the new definition
		kb->redefine(&block, onHeap, definitionSource);
	}

	// otherwise create a new entry
	else {
		LOG_NEW; KnownListEntry* nkb =
			new KnownListEntry(&block,onHeap,definitionSource,
					   allBlocks[idx]);
		allBlocks[idx] = nkb;
	}
}

// Find a known list entry within a single search chain
KnownListEntry*
KnownBlock::findEntry (const char* name, KnownListEntry* l) {
	while (l) {
		if (strcmp (name, l->b->name()) == 0)
			break;
		l = l->next;
	}
	return l;
}

// Find a known list entry among all blocks valid for named domain
KnownListEntry*
KnownBlock::findEntry (const char* name, const char* dom) {
	int domidx = domainIndex(dom);
	// if no such domain, return 0.
	if (domidx < 0) return 0;
	// search the list for this domain
	KnownListEntry* e = findEntry (name, allBlocks[domidx]);
	if (e) return e;
	// try the lists for any subdomains
	Domain* dp = Domain::named(domainNames[domidx]);
	StringListIter next(dp->subDomains);
	const char* sub;
	while ((sub = next++) != 0) {
	  int ix = domainIndex(sub);
	  e = findEntry(name, allBlocks[ix]);
	  if (e) return e;
	}
	return 0;
}

const Block*
KnownBlock::find(const char* type, const char* dom) {
	KnownListEntry* e = findEntry (type, dom);
	return e ? e->b : (Block*)NULL;
}

// Look up a KnownBlock definition by name and return its definition source.
// Returns TRUE if found, FALSE if no matching definition exists.
// note we do NOT consider blocks in subdomains.
int
KnownBlock::isDefined(const char* type, const char* dom,
		      const char* &definitionSource) {
	int domidx = domainIndex(dom);
	if (domidx < 0) return FALSE;
	KnownListEntry* e = findEntry (type, allBlocks[domidx]);
	if (!e) return FALSE;
	definitionSource = e->definitionSource;
	return TRUE;
}

// return TRUE if indicated name refers to a dynamically linked block.
// note we do NOT consider blocks in subdomains.
int
KnownBlock::isDynamic(const char* type, const char* dom) {
	int domidx = domainIndex(dom);
	if (domidx < 0) return 0;
	KnownListEntry* e = findEntry (type, allBlocks[domidx]);
	if (!e) return FALSE;
	return e->dynLinked;
}

// Look up a KnownBlock definition by name and return its serial number.
// Returns 0 iff no matching definition exists.
long
KnownBlock::serialNumber (const char* name, const char* dom) {
	KnownListEntry* e = findEntry (name, dom);
	return e ? e->serialNumber : 0L;
}


// Find the knownlist entry matching a block, if there is one.
// Note that this won't necessarily find an exact copy of the block.
// For example, the given block might be an obsolete version of a
// dynamically-linked star.  We will find the new entry that superseded it.
// This is exactly what we want for serial number comparisons.

// ISSUE: assuming that the className matches the name under which the
// block is registered really isn't very safe.  Is there a better way?
// Maybe KnownBlock should insist on registering blocks by their classnames,
// rather than allowing the registered name to be given separately.

KnownListEntry*
KnownBlock::findEntry (Block& block) {
	int idx = domainIndex (block);
	// If the block is a wormhole, we need to use the classname of
	// the inside galaxy; the combination would be under that name.
	// (className of a wormhole does not return anything useful.)
	const char * cname;
	if (block.isItWormhole())
	  cname = block.asStar().asWormhole()->insideGalaxy().className();
	else
	  cname = block.className();
	// Stars generally have classNames starting with the domain name,
	// and are registered under the rest of the className.  So if the
	// className starts with the domain name, try the suffix first.
	const char * dname = block.domain();
	int domlength = strlen(dname);
	KnownListEntry* kb = NULL;
	if (strncmp(cname, dname, domlength) == 0)
	  kb = findEntry (cname + domlength, allBlocks[idx]);
	// Try the whole classname --- the right thing for InterpGalaxies.
	if (!kb)
	  kb = findEntry (cname, allBlocks[idx]);
	return kb;
}


// Given a block, find the matching KnownBlock definition,
// and return its serial number (or 0 if no matching definition exists).
long
KnownBlock::serialNumber (Block& block) {
	KnownListEntry* e = findEntry (block);
	return e ? e->serialNumber : 0L;
}

// The main cloner.  This method gives us a new block of the named
// type, by asking the matching block on the list to clone itself.
// Guard against infinite recursion due to circular galaxy definitions.

Block*
KnownBlock::clone(const char* type, const char* dom) {
	if (numDomains == 0) bombNoDomains();
	KnownListEntry* e = findEntry (type, dom);
	if (e) {
		if (e->cloneInProgress)
			recursiveClone(type,dom);
		else {
			e->cloneInProgress = 1;
			Block* p = e->b->clone();
			e->cloneInProgress = 0;
			return p;
		}
	}
	else
		noMatch(type,dom);
	return 0;
}

void KnownBlock::noMatch(const char* type,const char* dom) {
	int domidx = domainIndex(dom);
	if (domidx < 0) {
		Error::abortRun("No such domain: ", dom);
	}
	else {
		StringList msg = "No star/galaxy named '";
		msg << type << "' in domain '" << dom << "'";
		Error::abortRun (msg);
	}
}

void KnownBlock::recursiveClone(const char* type,const char* dom) {
	StringList msg = "Recursive definition of star/galaxy named '";
	msg << type << "' in domain '" << dom << "'";
	Error::abortRun (msg);
}

// Like clone except states are not duplicated, we just get default values

Block*
KnownBlock::makeNew(const char* type, const char* dom) {
	if (numDomains == 0) bombNoDomains();
	KnownListEntry* e = findEntry (type, dom);
	if (e) {
		if (e->cloneInProgress)
			recursiveClone(type,dom);
		else {
			e->cloneInProgress = 1;
			Block* p = e->b->makeNew();
			e->cloneInProgress = 0;
			return p;
		}
	}
	else
		noMatch(type,dom);
	return 0;
}

// Return known list for domain index i as text, separated by linefeeds

StringList
KnownBlock::nameListForDomain (int idx) {
	KnownListEntry* l = allBlocks[idx];
	StringList s;
	while (l) {
	  s << l->b->name() << "\n";
	  l = l->next;
	}
	return s;
}

// Return name list for given domain.
StringList
KnownBlock::nameList (const char* dom) {
	int idx = domainIndex (dom);
	if (idx < 0) return "";
	return nameListForDomain (idx);
}

// KnownBlockIter methods

void KnownBlockIter::reset() {
	if (idx >= 0)
		pos = allBlocks[idx];
	else
		pos = 0;
}

KnownBlockIter::KnownBlockIter (const char* dom) {
	idx = KnownBlock :: domainIndex (dom, 0);
	reset();
}



