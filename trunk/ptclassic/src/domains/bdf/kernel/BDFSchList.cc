static const char file_id[] = "BDFSchList.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
 Date of creation: 1/8/91

 Auxiliary classes for the BDF Scheduler.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "BDFSchList.h"
#include "Geodesic.h"
#include "Scheduler.h"
#include "BDFScheduler.h"

// spaces is simply a large number of spaces.
const char spaces[] =
"                                                                        ";

const int TABSIZE = 4;

inline static const char* tab(int n) {
	return spaces + sizeof(spaces) - 1 - TABSIZE*n;
};

void BDFSList::append(BDFSNode* b, int sdf) {
	if (!sdf) allSDF = FALSE;
	if (first == 0) {
		first = last = b;
		b->next = b->prev = 0;
	}
	else {
		last->next = b;
		b->prev = last;
		b->next = 0;
		last = b;
	}
	count++;
}

BDFSList :: ~BDFSList() {
	BDFSNode* p = first;
	while (p) {
		BDFSNode* t = p;
		p = p->next;
		delete t;
	}
}

StringList BDFSList::print(int indent) {
	StringList out;
	BDFSListIter next(*this);
	BDFSNode* b;
	while ((b = next++) != 0)
		out += b->print(indent);
	return out;
}

StringList BDFStarNode::print(int indent) {
	StringList out = tab(indent);
	out += star.fullName();
	out += "\n";
	return out;
}

StringList BDFTestNode::print(int indent) {
	StringList out;
	if (ifTrue.size() == 0 && ifFalse.size() == 0) return out;
	const char* myTab = tab(indent);
	const char* notOp = ifTrue.size() ? "" : "!";
	out += myTab;
	out += "if (";
	out += notOp;
	out += port.fullName();
	out += ") {\n";
	if (ifTrue.size()) out += ifTrue.print(indent+1);
	else out += ifFalse.print(indent+1);
	out += myTab;
	out += "}\n";
	if (ifTrue.size() == 0 || ifFalse.size() == 0) return out;
	out += myTab;
	out += "else {\n";
	out += ifFalse.print(indent+1);
	out += myTab;
	out += "}\n";
	return out;
}

// not really a do-nothing: declaring it here prevents multiple copies
BDFTestNode::~BDFTestNode() {}

// the canMoveOver predicate.
int BDFSNode::canMoveOver() { return FALSE;}

int BDFTestNode::canMoveOver() {
	return ifTrue.pureSDF() && ifFalse.pureSDF();
}

// fancy insertion of an SDF star -- move backwards across tests that
// have only pure SDF stars on their branches.
void BDFSList::fancyInsert(BDFStarNode* s) {
	if (size() == 0)
		append (s, 1);
	else {
		BDFSNode * t = last;
		// skip backward over as many positions as we can
		while (t && t->canMoveOver()) t = t->prev;
		if (t) {
			// insert new node after t
			s->prev = t;
			s->next = t->next;
			t->next = s;
			if (s->next) s->next->prev = s;
			else last = s;
		}
		else {
			// put new node first in list
			s->prev = 0;
			s->next = first;
			first->prev = s;
			first = s;
		}
	}
	count++;
}

void BDFTreeNode::add(Star& star, const BoolTerm &t, int sdf, int writesBools) {
	const BoolSignal * q = t.list();
	if (!q) {
		BDFStarNode* s = new BDFStarNode(star,writesBools);
		if (sdf) where.fancyInsert(s);
		else {
			prune();
			where.append(s, sdf);
		}
		return;
	}
	if (decis) {
		BoolTerm nt(t);
		if (nt.elim(BoolSignal(*decis,0))) {
			ifTrue->add(star, nt, sdf, writesBools);
			return;
		}
		if (nt.elim(BoolSignal(*decis,1))) {
			ifFalse->add(star, nt, sdf, writesBools);
			return;
		}
		// no match: terminate this part of tree
		prune();
	}

// we must add a decision block if we get here.
	decis = &q->p;
	BDFTestNode* tes = new BDFTestNode(*(PortHole*)decis);
	ifTrue = new BDFTreeNode(tes->ifTrue);
	ifFalse = new BDFTreeNode(tes->ifFalse);
	BoolTerm nt(t);
	nt.elim(*q);
	where.append(tes, sdf);
	if (q->neg()) ifFalse->add(star, nt, sdf, writesBools);
	else ifTrue->add(star, nt, sdf, writesBools);
}

void BDFTreeNode::prune() {
	delete ifTrue;
	delete ifFalse;
	ifTrue = ifFalse = 0;
	decis = 0;
}

// destructor: note that it is recursive.
BDFTreeNode::~BDFTreeNode() {
	delete ifTrue;
	delete ifFalse;
}

// See how we run

void BDFStarNode::run(BDFScheduler& sch) {
	star.run();
	if (writesBooleans)
		sch.saveBooleans(star);
}

void BDFTestNode::run(BDFScheduler& sch) {
	int test = sch.getBoolValue(port);
	if (test) ifTrue.run(sch);
	else ifFalse.run(sch);
}

void BDFSList::run(BDFScheduler& sch) {
	BDFSListIter next(*this);
	BDFSNode* n;
	while ((n = next++) && !sch.haltRequested())
		n->run(sch);
}
