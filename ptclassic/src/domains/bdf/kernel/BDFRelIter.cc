/**************************************************************************
Version identification:
$Id$

Copyright (c) 1993 The Regents of the University of California.
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

Programmer:  J. Buck

This file defines methods for an iterator for finding all BDFClustPorts
that are the same as, or the complement of, a given BDFClustPort, using
BDF_SAME and BDF_COMPLEMENT relations.

**************************************************************************/
static const char file_id[] = "BDFRelIter.cc";

#include "BDFClustPort.h"
#include "BDFCluster.h"
#include "BDFRelIter.h"
#include <iostream.h>

// work list has entries with a BDFClustPort* and a relation.
struct BDFRelWork {
	BDFClustPort* port;
	BDFRelation rel;
	BDFRelWork* link;
	BDFRelWork(BDFClustPort* pp,BDFRelation r, BDFRelWork* l=0)
	: port(pp), rel(r), link(l) {}
};

// init: pos = me, port-queue and cluster-list empty, and then far() is queued.
BDFClustPortRelIter::BDFClustPortRelIter(BDFClustPort& me) :
start(&me), workStack(0)
{
	init();
}

void BDFClustPortRelIter::init() {
	pos = start;
	curRel = BDF_SAME;
	visited.put(pos);
	pushFar();
}

// debug support.
static int enableReport = FALSE;

void relIterReport(int flag) {
	enableReport = flag;
}

static void report(BDFClustPort*s, BDFClustPort*p, BDFRelation r) {
	if (!enableReport) return;
	if (p == 0) {
		cerr << "Iter: no more matches for "
		     << s->name() << "\n";
	}
	else {
		cerr << "Iter: " << p->name()
		     << (r == BDF_SAME ? " same as " : " reverse of ")
		     << s->name() << "\n";
	}
}

BDFClustPort* BDFClustPortRelIter::next(BDFRelation& relToOrig) {
	if (!pos) return 0;
	BDFRelation r = pos->relType();
	if (SorC(r)) {
		pos = pos->assoc();
		if (r == BDF_COMPLEMENT) curRel = reverse(curRel);
		if (!visited.member(pos)) {
			pushFar();
			relToOrig = curRel;
			visited.put(pos);
			report(start,pos,curRel);
			return pos;
		}
	}
	popFar();
	relToOrig = curRel;
	report(start,pos,curRel);
	return pos;
}

// this method sets up to process the far() of the current location
// later, provided that far() should be considered the same and has
// not yet been visited.

void BDFClustPortRelIter::pushFar() {
	if (!pos) return;
	BDFClustPort* p = pos->far();
	if (p == 0 || p->numTokens() != 0 || visited.member(p)) return;
	LOG_NEW; workStack = new BDFRelWork(p,curRel,workStack);
}

// this method gets back a far() pointer whose processing has been postponed
// until later.  Skip any that have already been visited.

void BDFClustPortRelIter::popFar() {
	int v;
	if (workStack) {
		// pop stack until we find a pos that has not been visited.
		do {
			pos = workStack->port;
			curRel = workStack->rel;
			BDFRelWork* t = workStack->link;
			LOG_DEL; delete workStack;
			workStack = t;
			v = visited.member(pos);
		} while (workStack && v);
		// if the last pos we popped was visited, return 0.
		// otherwise mark it as visited and return it.
		if (v) pos = 0;
		else visited.put(pos);
	}
	else pos = 0;
}

void BDFClustPortRelIter::clearHistory() {
	while (workStack) popFar();
	visited.initialize();
}



