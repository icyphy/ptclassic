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
#include "KnownBlock.h"
#include "Output.h"
#include <string.h>
#include <std.h>
#include "Scheduler.h"
#include "Domain.h"

extern Error errorHandler;

// private class for entries on known lists

class KnownListEntry {
	friend class KnownBlock;
	Block* b;
	int onHeap;
	KnownListEntry *next;
};

int KnownBlock :: currentDomain = 0;
int KnownBlock :: numDomains = 0;

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
	errorHandler.error("Too many distinct domains");
	exit (1);
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
	}

	// otherwise create a new entry
	else {
		KnownListEntry* nkb = new KnownListEntry;
		nkb->b = &block;
		nkb->next = allBlocks[idx];
		nkb->onHeap = isOnHeap;
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
		char* sub;
		while ((sub = next++) != 0) {
			int ix = domainIndex(sub);
			KnownListEntry* en = findEntry(type, allBlocks[ix]);
			if (en) return en->b;
		}
	}
	return e ? e->b : NULL;
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
	errorHandler.error (msg);
	return 0;
}

// Produce a scheduler matching the named domain
Scheduler*
KnownBlock::newSched(const char* name) {
	return &Domain::named(name)->newSched();
}

Scheduler*
KnownBlock::newSched() {
	return &Domain::named(domainNames[currentDomain])->newSched();
}

// Function to set the domain.
int
KnownBlock::setDomain (const char* newDom) {
	int idx = domainIndex (newDom);
	if (idx < 0) {
		errorHandler.error ("Unknown domain: ", newDom);
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

