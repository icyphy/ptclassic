static const char file_id[] = "DCParProcs.cc";
/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha based on G.C. Sih's code
Date of last revision: 5/92 

*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DCParProcs.h"
#include "DCGraph.h"
#include "MultiTarget.h"

			/////////////////////
			///  Constructor  ///
			/////////////////////
DCParProcs::DCParProcs(int n, MultiTarget* t) : ParProcessors(n,t) {
	LOG_NEW; schedules = new DCUniProc[n];
}

UniProcessor* DCParProcs :: getProc(int num) { return getSchedule(num); }
ParNode* DCParProcs :: createCommNode(int i) {
	LOG_NEW; return new DCNode(i);
}

			////////////////////
			///  Destructor  ///
			////////////////////

DCParProcs::~DCParProcs() {
	LOG_DEL; delete [] schedules;
}

			//////////////////
			///  copyInfo  ///
			//////////////////
// save scheduling information of the nodes
void DCParProcs::saveBestResult(DCGraph* graph) {
	// copy info for all regular nodes
	graph->copyInfo();

	// for communication nodes
	EGNodeListIter Noditer(SCommNodes);
	DCNode *nodep;
	while ((nodep = (DCNode*) Noditer++) != 0) {
		nodep->saveInfo();
	}
}

// reset the visit flag of nodes in the list
static void resetVisitFlag(EGNodeList& nlist) {
	EGNodeListIter iter(nlist);
	EGNode *eg;
	while ((eg = iter++) != 0) {
		eg->resetVisit();
	}
}

			/////////////////
			///  findSLP  ///
			/////////////////
// This routine finds the progression of nodes (regular and communication)
//      in bestSchedule (not the graph) which prevents the makespan
//      from being any shorter.
// The limiting progression can span several processors and cannot contain
//       idle time.  If there are several schedule limiting progressions
//       (slp's), this routine returns one of them.

int DCParProcs::findSLP(DCNodeList *slp) {

	slp->initialize();

	// make each comm node as unvisited
	resetVisitFlag(SCommNodes);

	// the time we trace the path to, going backwards from the makespan
	//	down to zero.
	int time = getMakespan();

	// Find the lowest index proc which finishes exactly at the makespan
	for (int curProc = 0; curProc < numProcs; curProc++) {
		if ((getSchedule(curProc)->getAvailTime() == time))
			break;
	}

	// Keep tracing the path.
	NodeSchedule* ns = getSchedule(curProc)->getCurSchedule();
	while (time != 0) {
		DCNode* curNode = (DCNode*) ns->getNode();
		if (curNode->getType() <= 0) {
			slp->insert(curNode);
			time = curNode->getBestStart();
			ns = ns->previousLink();
			continue;
		}

		// < switch processors >
		// backtrack slp until to meet a comm node
		DCNode* node, *commNode;
		while ((node = slp->headNode()) != 0) {
			if (node->getType() >= 0) { // non-receive node
				slp->takeFromFront(); // cut off node
				continue;
			}

			// Ask topology dependent section if a communication
			// reservation is constraining the SLP.
			commNode = (DCNode*) mtarget->backComm(node);
	
			if ((commNode == 0) || commNode->alreadyVisited()) {
				// No comm node is constraining or already 
				// seen it, disregard this comm node.
				slp->takeFromFront();
			} else {
				// Switch slp processors
				curProc = commNode->getProcId();
				// Mark the commNode as being visited
				commNode->beingVisited();
				break;
			}
		}

		if (node == 0) {	// No comm node
			Error::abortRun("Backtracking error in SLP");
			return FALSE;
		} else {
			ns = getSchedule(curProc)->getNodeSchedule(commNode);
			if (!ns) {
				Error::abortRun("can't find node schedule");
				return FALSE;
			}
		}

	}       // End of while loop going until time reaches zero
	return TRUE;
}

			/////////////////////////
			///  categorizeLoads  ///
			/////////////////////////
// Categorizes each processor as being heavily or lightly loaded.
// It sets an integer array: 1 for heavy and -1 for light processors.
// Initial threshold is 50% of the maxload.
// Considers all processors heavily loaded if all processors are loaded
//      beyond 75% of the maximum load on any processor.
// We regard at most one idle processor as lightly loaded. Leave other
// idle processors untouched.

void DCParProcs::categorizeLoads(int* procs) {

        int maxload = 0;

        for (int i = 0; i < numProcs; i++) {
                DCUniProc* sch = getSchedule(i);
                int load = sch->computeLoad();
                if (load > maxload) maxload = load;
        }

        // Set an initial load threshold at half the maximum load
        int threshold = (int)(maxload / 2);
	int flag = 0;

	do {
		int seenIdle = 0;
		for (i = 0; i < numProcs; i++) {
			int temp = getSchedule(i)->getLoad();
			if (temp > threshold) {
				procs[i] = 1;
			} else {
				if (temp == 0) {
					if (!seenIdle) {
						seenIdle = 1;
						procs[i] = -1;
					} 
				} else		procs[i] = -1;
				flag = 1;
			}
		}
		if (!flag) {
			// raise the threshold
			threshold += (int)(threshold/2);
			flag = 2;
		} else flag = 0;
        } while (flag == 2);
}
			
			////////////////////////
			///  finalizeGalaxy  ///
			////////////////////////

// After the final schedule is made, we make a final version of the
// expanded graph including all comm. nodes.

void DCParProcs :: finalizeGalaxy(DCGraph* graph) {

	// reset the graph
	graph->replenish(0);

	// for communication nodes,
	EGNodeListIter niter(SCommNodes);
	ParNode* n;
	while ((n = (ParNode*) niter++) != 0) {
		if (n->getType() == -1) {	// send node
			ParAncestorIter ancs(n);
			ParNode* srcN = ancs++;
			n->removeAncs(srcN);
			srcN->connectedTo(n);
		} else {
			ParDescendantIter descs(n);
			ParNode* destN = descs++;
			if (destN) {
				n->removeDescs(destN);
				n->connectedTo(destN);
			}
		}
	}
}

