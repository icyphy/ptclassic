#ifndef _ClustArcList_h
#define _ClustArcList_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: G.C. Sih

ClustArc is used for intercluster arcs by the parallel scheduler

*****************************************************************/

#include "DataStruct.h"
#include "StringList.h"
class Cluster;

			//////////////////////
			//  class ClustArc  //
			//////////////////////
// For representing arcs between clusters.
class ClustArc {
public:
	ClustArc(Cluster *n, int samps): neighbor(n), samples(samps) {}
	Cluster *getNeighbor() { return neighbor; }
	void changeNeighbor(Cluster *name) { neighbor = name; }
	void changeSamples(int newsamps) { samples = newsamps; }
	void addSamples(int delta) { samples += delta; }
	int getSamples() { return samples; }
	StringList print();
private:
	Cluster *neighbor;
	int samples;
};

			//////////////////////////
			//  class ClustArcList  //
			//////////////////////////
// For handling lists of intercluster arcs.
class ClustArcList : public SequentialList
{
public:
	// void initialize(); (inherited from SequentialList)
	StringList print();
	void changeArc(Cluster *oldname, Cluster *newname);
	
	// check whether the given Cluster is already registered as a
	// neighbor.
	ClustArc* contain(Cluster*);

	// remove arcs
	void removeArcs();
};

			//////////////////////////////
			//  class ClustArcListIter  //
			//////////////////////////////
// For iterating through ClustArcLists

class ClustArcListIter : private ListIter {
public:
	ClustArcListIter(const ClustArcList& l) : ListIter(l) {}

	ClustArc *next() {return (ClustArc*)ListIter::next();}
	ClustArc *operator++() {return next();}

	ListIter::reset;

	void reconnect(ClustArcList& l) {ListIter::reconnect(l);}
};
#endif
