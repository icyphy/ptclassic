/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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
};

// declarations for static variables.  I would rather use class
// static than file static for the arrays, but g++ has bugs.

static KnownListEntry* allBlocks[NUMDOMAINS];
static const char* domainNames[NUMDOMAINS];
int KnownBlock :: currentDomain = 0;
int KnownBlock :: numDomains = 0;

const char* KnownBlock::domain()  { return domainNames[currentDomain];}

// This function looks up a domain.  It returns -1 if not found and
// we specified no adding.

int KnownBlock::domainIndex (const char* myDomain, int addIfNotFound) {

	for (int i = 0; i < NUMDOMAINS; i++) {
		if (domainNames[i] == 0) {
			if (addIfNotFound) {
				domainNames[i] = myDomain;
				return i;
			}
			else return -1;
		}
		else if (strcmp (domainNames[i], myDomain) == 0)
			return i;
	}
	Error::abortRun("Too many distinct domains");
	return -1;
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
	KnownListEntry* kb = findEntry (block.readName(), allBlocks[idx]);
	if (kb) {
		// delete the block if it was on the heap
		if (kb->onHeap) delete kb->b;
		kb->b = &block;
		kb->onHeap = isOnHeap;
		kb->dynLinked = Linker::isActive();
	}

	// otherwise create a new entry
	else {
		KnownListEntry* nkb = new KnownListEntry;
		nkb->b = &block;
		nkb->next = allBlocks[idx];
		nkb->onHeap = isOnHeap;
		nkb->dynLinked = Linker::isActive();
		allBlocks[idx] = nkb;
	}
}

// Find a known list entry
KnownListEntry*
KnownBlock::findEntry (const char* name, KnownListEntry* l) {
	while (l) {
		if (strcmp (name, l->b->readName()) == 0)
			break;
		l = l->next;
	}
	return l;
}

const Block*
KnownBlock::find(const char* type) {
	// search the list for this domain
	KnownListEntry* e = findEntry (type, allBlocks[currentDomain]);
	if (!e) {
		// try the lists for any subdomains
		Domain* dp = Domain::named(domainNames[currentDomain]);
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
KnownBlock::isDynamic(const char* type) {
	KnownListEntry* e = findEntry (type, allBlocks[currentDomain]);
	if (!e) return FALSE;
	return e->dynLinked;
}

// The main cloner.  This method gives us a new block of the named
// type, by asking the matching block on the list to clone itself.

Block*
KnownBlock::clone(const char* type) {
	const Block* p = find(type);
	if (p) return p->clone();
	StringList msg = "No star/galaxy named '";
	msg += type;
	msg += "' in domain '";
	msg += domainNames[currentDomain];
	msg += "'";
	Error::abortRun (msg);
	return 0;
}

// Function to set the domain.
int
KnownBlock::setDomain (const char* newDom) {
	int idx = domainIndex (newDom);
	if (idx < 0) {
		Error::error ("Unknown domain: ", newDom);
		return FALSE;
	}
	currentDomain = idx;
	return TRUE;
}

// comparison function for qsort.  Done this way to shut up the compiler;
// this is the right thing for qsort.
static int compar(void* a, void* b) {
	const char** ca = (const char**)a;
	const char** cb = (const char**)b;
	return strcmp (*ca, *cb);
}

// Return known list for domain index i as text, separated by linefeeds
// Names are printed in sorted order.
StringList
KnownBlock::nameListForDomain (int idx) {
	StringList s;
	KnownListEntry* l = allBlocks[idx];
	// count entries in list
	int n = 0;
	while (l) {
		n++;
		l = l->next;
	}
	const char** table = new const char*[n];
	l = allBlocks[idx];
	for (int i = 0; i < n; i++) {
		table[i] = l->b->readName();
		l = l->next;
	}
	qsort (table, n, sizeof (char*), compar);
	for (i = 0; i < n; i++) {
		s += table[i];
		s += "\n";
	}
	delete table;
	return s;
}

// Return known list as text, separated by linefeeds
StringList
KnownBlock::nameList () {
	return nameListForDomain (currentDomain);
}

// Same as above, but the user specifies the domain.
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
	if (dom)
		idx = KnownBlock :: domainIndex (dom, 0);
	else 
		idx = KnownBlock :: currentDomain;
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


