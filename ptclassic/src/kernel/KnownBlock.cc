static const char file_id[] = "KnownBlock.cc";
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
int KnownBlock :: currentDomain = 0;
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

const char* KnownBlock::domain()  {
	if (numDomains == 0) bombNoDomains();
	return domainNames[currentDomain];
}

// This function looks up a domain.  It returns -1 if not found and
// we specified no adding.

int KnownBlock::domainIndex (const char* myDomain, int addIfNotFound) {
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
	if (numDomains == 0) bombNoDomains();
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

Block*
KnownBlock::makeNew(const char* type) {
	if (numDomains == 0) bombNoDomains();
	const Block* p = find(type);
	if (p) return p->makeNew();
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
	LOG_NEW; const char** table = new const char*[n];
	l = allBlocks[idx];
	for (int i = 0; i < n; i++) {
		table[i] = l->b->name();
		l = l->next;
	}
	qsort (table, n, sizeof (char*), compar);
	for (i = 0; i < n; i++) {
		s += table[i];
		s += "\n";
	}
	LOG_DEL; delete table;
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


