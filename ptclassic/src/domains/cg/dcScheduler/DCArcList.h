#ifndef _DCArcList_h
#define _DCArcList_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: G.C. Sih
Modifier: Soonhoi Ha
Date of last revision: 5/92

The parallel scheduler uses class DCArc to represent internode arcs

*****************************************************************/

#include "DataStruct.h"
#include "DCNode.h"
#include "StringList.h"

			/////////////////////
			///  class DCArc  ///
			/////////////////////
// This class is used to represent CutArcs in the parallel scheduler

class DCArc
{
friend class DCArcList;
public:
	DCArc(DCNode *src, DCNode *sink, int fst, int scd, int thd);

	DCNode* getSrc() {return srcnode;}
	DCNode* getSink() {return sinknode;}
	int getF() { return first; }
	int getS() { return second; }
	int getT() { return third; }
	DCArcList* parentList() { return parent; }

	int operator==(DCArc &a) 
	 	{return ((srcnode == a.srcnode) && (sinknode == a.sinknode));}

	// check whether this arc is between invocs of the same star.
	int betweenSameStarInvocs() {
		return (srcnode->myMaster() == sinknode->myMaster()); }

	void reverse();
	StringList print();

private:
	DCNode *srcnode;
	DCNode *sinknode;
	DCArcList* parent;

	// The following three entries form a triplet of information used
	//	to help find cutArcs.
	int first;	// Sum of node ExecTimes preceding this arc. 
	int second;	// Communication time for this arc.
	int third;	// Sum of node ExecTimes following this arc.
};

			/////////////////////////
			///  class DCArcList  ///
			/////////////////////////
// Implements list of pointers to DCArcs.
class DCArcList : public SequentialList 
{

public:

	DCArcList() {initialize();}

	DCArcList(DCArcList &arclist);

	// Return the DCArc at the front of the list
	DCArc *head() {return (DCArc*)SequentialList::head();}

	// Insert a DCArc into the front of the list
	void insert(DCArc *arc) {
		arc->parent = this;
		SequentialList::prepend(arc);
	}

	// Append a DCArc into the back of the list
	void append(DCArc *arc) {
		arc->parent = this;
		SequentialList::put(arc);
	}

	// Remove a DCArc from the list
	int remove(DCArc *arc) {
		arc->parent = 0;
		return SequentialList::remove(arc);
	} 

	// return TRUE if the given arc is a member of the list.
	int member(DCArc* arc);

	// Print a list of the arcs in the list
	StringList print();

	// The number of DCArcs in the list
	int mySize() {return size(); } 

	// Destructor
	~DCArcList();
};

class DCArcIter : private ListIter
{
public:
	// Constructor
	DCArcIter(const DCArcList& list) : ListIter(list) {}

	// Get the next DCArc in the list
	DCArc *next() {return (DCArc*)ListIter::next();}
	DCArc *operator++ () {return next();}

	// reset to the beginning of the list
	void reset() {ListIter::reset();}

	// connect this iterator to another nodelist
	void reconnect(DCArcList& list) {ListIter::reconnect(list);}

};

#endif
