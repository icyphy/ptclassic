#ifndef _DCClustArcList_h
#define _DCClustArcList_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
