static const char file_id[] = "KnownBlock.cc";
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
#include "Linker.h"

// private class for entries on known lists

class KnownListEntry {
	friend class KnownBlock;
	friend class KnownBlockIter;
	Block* b;
	int onHeap;
	int dynLinked;
	KnownListEntry *next;
public:
	KnownListEntry(Block* bl,int oh, KnownListEntry* n) :
		b(bl), onHeap(oh), dynLinked(Linker::isActive()), next(n) {}
	~KnownListEntry ();
};

KnownListEntry::~KnownListEntry () {
	if (onHeap) { LOG_DEL; delete b;}
	LOG_DEL; delete next;
}

// declarations for static variables.  I would rather use class
// static than file static for the arrays, but g++ has bugs.

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
"No domains can be located by Ptolemy; something is wrong with your\n\
installation of either Ptolemy (no domains were included, perhaps?) or\n\
of your C++ compiler and linker (if global constructors aren't being called\n\
properly, we also get this symptom).  Exiting...\n";

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
	if (myDomain == 0) return -1;
	for (int i = 0; i < numDomains; i++) {
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

void KnownBlock::addEntry (Block& block, const char* name, int isOnHeap) {
	// set my name
	block.setBlock (name, NULL);

	// get domain index.  If undefined use lastDomain
	int idx = domainIndex (block);

	// see if defined; if so, replace
	KnownListEntry* kb = findEntry (block.name(), allBlocks[idx]);
	if (kb) {
		// delete the block if it was on the heap
		if (kb->onHeap) { LOG_DEL; delete kb->b;}
		kb->b = &block;
		kb->onHeap = isOnHeap;
		kb->dynLinked = Linker::isActive();
	}

	// otherwise create a new entry
	else {
		LOG_NEW; KnownListEntry* nkb =
			new KnownListEntry(&block,isOnHeap,allBlocks[idx]);
		allBlocks[idx] = nkb;
	}
}

// Find a known list entry
KnownListEntry*
KnownBlock::findEntry (const char* name, KnownListEntry* l) {
	while (l) {
		if (strcmp (name, l->b->name()) == 0)
			break;
		l = l->next;
	}
	return l;
}

const Block*
KnownBlock::find(const char* type, const char* dom) {
	int domidx = domainIndex(dom);
// if no such domain, return 0.
	if (domidx < 0) return 0;
	// search the list for this domain
	KnownListEntry* e = findEntry (type, allBlocks[domidx]);
	if (!e) {
		// try the lists for any subdomains
		Domain* dp = Domain::named(domainNames[domidx]);
		StringListIter next(dp->subDomains);
		const char* sub;
		while ((sub = next++) != 0) {
			int ix = domainIndex(sub);
			KnownListEntry* en = findEntry(type, allBlocks[ix]);
			if (en) return en->b;
		}
	}
	return e ? e->b : NULL;
}

// return TRUE if indicated name refers to a dynamically linked block.
int
KnownBlock::isDynamic(const char* type, const char* dom) {
	int domidx = domainIndex(dom);
	if (domidx < 0) return 0;
	KnownListEntry* e = findEntry (type, allBlocks[domidx]);
	if (!e) return FALSE;
	return e->dynLinked;
}

// The main cloner.  This method gives us a new block of the named
// type, by asking the matching block on the list to clone itself.

Block*
KnownBlock::clone(const char* type, const char* dom) {
	if (numDomains == 0) bombNoDomains();
	const Block* p = find(type,dom);
	if (p) return p->clone();
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

Block*
KnownBlock::makeNew(const char* type, const char* dom) {
	if (numDomains == 0) bombNoDomains();
	const Block* p = find(type,dom);
	if (p) return p->makeNew();
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

const Block* KnownBlockIter::next() {
	if (pos) {
		const Block* b = pos->b;
		pos = pos->next;
		return b;
	}
	else return 0;
}


