static const char file_id[] = "DecomScheduler.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha 
 Date of creation: 5/92

Loop scheduler

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DecomScheduler.h"
#include "DecomGal.h"
#include "LSNode.h"
#include "LSCluster.h"
#include "Target.h"
#include "MergeList.h"
#include "Error.h"
#include "streamCompat.h"


////////////////////////////
// Loop Scheduler methods //
////////////////////////////

void DecomScheduler::attemptMerge(LSNode &p, LSGraph &g)
{
	EGGateLinkIter nextGate(p.descendants);
	EGGate *d;
	SDFStar *currentmaster = p.myMaster();
	MergeList ml; 
	while ((d=nextGate.nextMaster(currentmaster)) != 0) {
		ml.insertMerge(&p,(LSNode*) d->farEndNode(), 1); 
	}

	nextGate.reconnect(p.ancestors);
	nextGate.reset();
	while ((d=nextGate.nextMaster(currentmaster))!=0) {
		ml.insertMerge(&p, (LSNode*) d->farEndNode(), 0); 
	}

	// If p is an isolated nodes, we detect the later invocation
	// which is not isolated. Then, attempt merge from that node.
	// If there is none, we do form a isolated cluster.

	if (!ml.size()) {
		LSNode* tmp = (LSNode*) p.getNextInvoc();
		while (tmp && (tmp->connected() == 0))
			tmp = (LSNode*) tmp->getNextInvoc();
		if (tmp) attemptMerge(*tmp,g);
		else g.formIsolatedCluster(p);
		return;
	}

	// dead-lock free cluster forming.
	MergeListIter nextMerge(ml);
	MergeLink *m;
	while ((m = nextMerge++) != 0)
		if(m->formRepeatedCluster(g)) return; 
}

// cluster building

void DecomScheduler::buildClusters(LSGraph &g)
{
	LSNode *p;
	g.initializeCandidates();
	while ((p=g.candidateFromFront())!=0) {
		// when all masters have only one instances, stop clustering.
		if (p->myMaster()->repetitions == Fraction(1)) {
			// push back the current node
			g.candidatePushBack(p);
			return;
		}
		attemptMerge(*p,g);
	}
}

//
// Main routine for DecomScheduler
//
int DecomScheduler::genSched(DecomGal* cgal)
{
	// Step 1. Shuvra's decomposition idea.
	//         After removing the arcs with enough delays.
	//	   If no arc is removed, skip decomposition step.

        if (cgal->simplify()) {      // remove arcs with enough delays.
        	cgal->decompose();      // decomposition step.
	}

	// Step 2. Joe's clustering algorithm.
	//         We no longer need to call this twice, or call genSubScheds.

	cgal->cluster();

	// Now, cgal is a compact form to be expanded for more looping.
	//
	// Step3. make an APEG graph.
	//

	myGraph.initialize();

	// Create the ExpandedGraph removing delay arcs.
	if (!myGraph.createMe(*cgal)) {
		Error::abortRun("Could not create expanded graph");
		invalid = TRUE;
		return FALSE;
	}
	if (logstrm) {
		*logstrm << myGraph.display();
	}

	// Step3. Build the PGAN cluster hierarchy.
	//
	buildClusters(myGraph);

	// Step 4. Schedule the cluster hierarchy.
	//
	if(!topLevelSchedule(myGraph)) {
		Error::abortRun("dead-locked... internal error");
		invalid = TRUE;
		return FALSE;
	}

	return TRUE;
}

// Schedule the cluster hierachy.

int DecomScheduler::topLevelSchedule(LSGraph &g)
{
	LSNodeListIter nextNode(g.candidates);
	LSNode* n;

	mySchedule.initialize();
	int flag;
	do {
		flag = FALSE;
		nextNode.reset();
		while ((n = nextNode++ ) != 0) {
			if (n->fireable()) {
				n->fireMe();
				mySchedule.append(*n->myMaster());
				g.removeCandidate(n);
				flag = TRUE;
			}
		}
	} while (flag);

	if (g.candidates.size() > 0) return FALSE;
	return TRUE;
}

StringList DecomScheduler::displaySchedule(int depth) {
	StringList sch;
	SDFSchedIter next(mySchedule);
	SDFBaseCluster* c;
	while ((c = (SDFBaseCluster*)next++) != 0) {
		sch += c->displaySchedule(depth);
	}
	return sch;
}

void DecomScheduler::genCode(Target& t, int depth) {
	SDFSchedIter next(mySchedule);
	SDFBaseCluster* c;
	while ((c = (SDFBaseCluster*)next++) != 0) {
		c->genCode(t,depth);
	}
}
