#ifndef _AcyCluster_h
#define _AcyCluster_h 1
#ifdef __GNUG__
#pragma implementation
#endif

#include "SynDFCluster.h"


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

 Programmer:  Praveen K. Murthy
 Date of creation: Feb/9/96

Acyclic cluster to implement graph cutting routines needed by the
RPMC algorithm in AcyLoopScheduler.  For details of this graph cutting
heuristic, see chapter 6 of

	"Software synthesis from dataflow graphs", by

Shuvra S. Bhattacharyya, Praveen K. Murthy, and Edward A. Lee,
Kluwer Academic Publishers, Norwood, MA, 1996


*******************************************************************/

// AcyCluster is a class derived from SynDFCluster that implements all
// of the graph cutting routines required by the RPMC algorithm in
// AcyLoopScheduler.

class SequentialList;

class AcyCluster: public SynDFCluster {
public:
	AcyCluster() {};

	/* virtual */ Block* makeNew() const { return new AcyCluster; }
	int legalCutIntoBddSets(int K);
	int checkLegalCut(int cutValue, int bdd);
protected:
	void weightArcs();
	void tagDelayArcs();
	int markSuccessors(AcyCluster* c);
	int markPredecessors(AcyCluster* c);
	int computeCutCost(int flag_loc, int leftFlagValue);
	void findIndepBndryNodes(int type, Cluster* c, SequentialList& indepBndryNodes);
	int costOfMovingAcross(Cluster* bndryNode, int direction);
	void updateBestCut(int numOnLeftSide);

};

class AcyClusterIter : private SynDFClusterIter {
public:
	inline AcyClusterIter(Galaxy& g):SynDFClusterIter(g) {};
	inline AcyCluster* next() { return (AcyCluster*)(SynDFClusterIter::next());}
	inline AcyCluster* operator++(POSTFIX_OP) { return next(); }
	AcyCluster* next(int flagValue, int index) {
	    return (AcyCluster*)(SynDFClusterIter::next(flagValue, index));
	}
	ClusterIter::reset;
	ClusterIter::remove;
};


#endif
