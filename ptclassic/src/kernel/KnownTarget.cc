/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 9/25/91

 Methods for KnownTarget, which is responsible for keeping the list
 of targets.

**************************************************************************/
#include "Linker.h"
#include "Target.h"
#include "KnownTarget.h"
#include "KnownBlock.h"
#include "miscFuncs.h"

class KnownTargetEntry {
	friend class KnownTarget;
	friend class KnownTargetIter;
	Target* targ;
	int onHeap;
	int dynLinked;
	KnownTargetEntry *next;
};

static KnownTargetEntry* allTargets;

// Find a target on the known list (internal form)
// Find a known list entry
static KnownTargetEntry*
KnownTarget::findEntry (const char* name) {
	KnownTargetEntry* l = allTargets;
	while (l) {
		if (strcmp (name, l->targ->readName()) == 0)
			break;
		l = l->next;
	}
	return l;
}

// Find and return a target
const Target*
KnownTarget::find(const char* type) {
	KnownTargetEntry * e = findEntry (type);
	return e ? e->targ : NULL;
}

// return TRUE if indicated name refers to a dynamically linked target.
int
KnownTarget::isDynamic(const char* type) {
	KnownTargetEntry* e = findEntry (type);
	if (!e) return FALSE;
	return e->dynLinked;
}

// The main cloner.  This method gives us a new block of the named
// type, by asking the matching block on the list to clone itself.

Target*
KnownTarget::clone(const char* type) {
	const Target* p = find(type);
	if (p) return p->cloneTarget();
	StringList msg = "No target named '";
	msg += type;
	msg += "' in the KnownTarget list";
	Error::abortRun (msg);
	return 0;
}

// Add a target to the known list

void KnownTarget::addEntry (Target& target, const char* name, int isOnHeap) {
	// set my name
	target.setBlock (name, NULL);

	// see if defined; if so, replace
	KnownTargetEntry* kb = findEntry (target.readName());
	if (kb) {
		// delete the target if it was on the heap
		if (kb->onHeap) delete kb->targ;
		kb->targ = &target;
		kb->onHeap = isOnHeap;
		kb->dynLinked = Linker::isActive();
	}

	// otherwise create a new entry
	else {
		KnownTargetEntry* nkb = new KnownTargetEntry;
		nkb->targ = &target;
		nkb->next = allTargets;
		nkb->onHeap = isOnHeap;
		nkb->dynLinked = Linker::isActive();
		allTargets = nkb;
	}
}

// find all targets that work for the given block
int KnownTarget::getList (const Block& b, const char** names, int nMax) {
	KnownTargetEntry* l = allTargets;
	int n = 0;
	while (l && n < nMax) {
		Target& curTarg = *l->targ;
		if (b.isA(curTarg.starType())) {
			names[n] = curTarg.readName();
			n++;
		}
		l = l->next;
	}
	return n;
}

// find all targets that work for the given domain.
// uses KnownBlock to get a block that works with the domain.
// CAREFUL: does this sometimes give too broad an answer?  It might
// if the first star on the domain list is more flexible than
// the others.

int KnownTarget::getList (const char* dom, const char** names, int nMax) {
	KnownBlockIter iter(dom);
	const Block* b = iter++;
	if (b == 0) return 0;
	return getList(*b, names, nMax);
}

const char* KnownTarget :: defaultName (const char* dom) {
	if (dom == 0) dom = KnownBlock::domain();
	static char buf[72];
	strcpy (buf, "default-");
	strcat (buf, dom);
	return buf;
}

// KnownTargetIter methods

void KnownTargetIter::reset() {
	pos = allTargets;
}

const Target* KnownTargetIter::next() {
	if (pos) {
		const Target* t = pos->targ;
		pos = pos->next;
		return t;
	}
	else return 0;
}

