#ifndef _BDFSchList_h
#define _BDFSchList_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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
#include "Connect.h"

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
		reset();
	}
	BDFSNode* next() {
		BDFSNode* r = nxt;
		if (nxt) nxt = nxt->next;
		return r;
	}
	BDFSNode* operator++() { return next();}
	void reset(int back = 0) { nxt = ref.first;}
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
