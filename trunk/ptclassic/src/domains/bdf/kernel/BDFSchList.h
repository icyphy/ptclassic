#ifndef _BDFSchList_h
#define _BDFSchList_h 1
/******************************************************************
Version identification:
@(#)BDFSchList.h	2.7	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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
 Date of creation: 2/1/91

 Auxiliary classes for use with the BDF Scheduler.   This is basically
 the data structure that stores the schedule, with methods to run it.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "BDFBool.h"
#include "Star.h"
#include "PortHole.h"

class BDFSList;
class BDFScheduler;

class BDFTreeNode {
	BDFSList& where;
	const PortHole* decis;
	BDFTreeNode* ifTrue;
	BDFTreeNode* ifFalse;
	void prune();
public:
	BDFTreeNode(BDFSList& w) : where(w), decis(0), ifTrue(0), ifFalse(0){}
	~BDFTreeNode();
	void add(Star&, const BoolTerm&, int, int);
};

class BDFSNode {
	friend class BDFSList;
	friend class BDFSListIter;
	BDFSNode* prev;
	BDFSNode* next;
public:
	BDFSNode() : prev(0), next(0) {}
	virtual StringList print(int) = 0;
	virtual int canMoveOver();
	virtual void run(BDFScheduler&) = 0;
	virtual ~BDFSNode() {}
};

class BDFStarNode : public BDFSNode {
	Star& star;
	int writesBooleans;
public:
	BDFStarNode(Star& s, int w) : star(s), writesBooleans(w) {}
	void run(BDFScheduler&);
	StringList print(int);
};

class BDFSList {
	friend class BDFSListIter;
	BDFSNode* first;
	BDFSNode* last;
	int count;
	int allSDF;
public:
	BDFSList() : first(0), last(0), count(0), allSDF(1) {}
	void append(BDFSNode* b, int sdf);
	void fancyInsert(BDFStarNode* s);
	int size() const { return count;}
	int pureSDF() const { return allSDF;}
	void run(BDFScheduler&);
	StringList print(int);
	~BDFSList();
};

class BDFSListIter {
public:
	BDFSListIter(const BDFSList& bl) : ref(bl) {
		nxt = ref.first;
	}
	BDFSNode* next() {
		BDFSNode* r = nxt;
		if (nxt) nxt = nxt->next;
		return r;
	}
	BDFSNode* operator++(POSTFIX_OP) { return next();}
	void reset() { nxt = ref.first;}
private:
	const BDFSList& ref;
	BDFSNode* nxt;
};


class BDFTestNode : public BDFSNode {
	PortHole& port;
public:
	BDFTestNode(PortHole& p) : port(p) {}
	BDFSList ifTrue;
	BDFSList ifFalse;
	StringList print(int);
	int canMoveOver();
	void run(BDFScheduler&);
	~BDFTestNode();
};

class BDFSchedule {
	BDFTreeNode treeTop;
	BDFSList main;
public:
	BDFSchedule () : treeTop(this->main) {}
	void add(Star& s,const BoolTerm& t,int sdf, int writesBools) {
		treeTop.add(s, t, sdf, writesBools);
	}
	void run(BDFScheduler& s) { main.run(s);}
	StringList print () { return main.print(0);}
};
#endif
