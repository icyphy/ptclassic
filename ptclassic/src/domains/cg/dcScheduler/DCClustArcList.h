#ifndef _DCClustArcList_h
#define _DCClustArcList_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: G.C. Sih

DCClustArc is used for intercluster arcs by the parallel scheduler

*****************************************************************/

#include "DataStruct.h"
#include "StringList.h"
class DCCluster;

			//////////////////////
			//  class DCClustArc  //
			//////////////////////
// For representing arcs between clusters.
class DCClustArc {
public:
	DCClustArc(DCCluster *n, int samps): neighbor(n), samples(samps) {}
	DCCluster *getNeighbor() { return neighbor; }
	void changeNeighbor(DCCluster *name) { neighbor = name; }
	void changeSamples(int newsamps) { samples = newsamps; }
	void addSamples(int delta) { samples += delta; }
	int getSamples() { return samples; }
	StringList print();
private:
	DCCluster *neighbor;
	int samples;
};

			//////////////////////////
			//  class DCClustArcList  //
			//////////////////////////
// For handling lists of intercluster arcs.
class DCClustArcList : public SequentialList
{
public:
	// void initialize(); (inherited from SequentialList)
	StringList print();
	void changeArc(DCCluster *oldname, DCCluster *newname);
	
	// check whether the given DCCluster is already registered as a
	// neighbor.
	DCClustArc* contain(DCCluster*);

	// remove arcs
	void removeArcs();
};

			//////////////////////////////
			//  class DCClustArcListIter  //
			//////////////////////////////
// For iterating through DCClustArcLists

class DCClustArcListIter : private ListIter {
public:
	DCClustArcListIter(const DCClustArcList& l) : ListIter(l) {}

	DCClustArc *next() {return (DCClustArc*)ListIter::next();}
	DCClustArc *operator++() {return next();}

	ListIter::reset;

	void reconnect(DCClustArcList& l) {ListIter::reconnect(l);}
};
#endif
