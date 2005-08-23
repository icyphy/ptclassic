#ifndef _DCArcList_h
#define _DCArcList_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
@(#)DCArcList.h	1.8	3/2/95

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
	~DCArcList();

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
};

class DCArcIter : private ListIter
{
public:
	DCArcIter(const DCArcList& list) : ListIter(list) {}

	// Get the next DCArc in the list
	DCArc *next() {return (DCArc*)ListIter::next();}
	DCArc *operator++ (POSTFIX_OP) {return next();}

	// reset to the beginning of the list
	void reset() {ListIter::reset();}

	// connect this iterator to another nodelist
	void reconnect(DCArcList& list) {ListIter::reconnect(list);}
};

#endif
