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
#include "BaseMultiTarget.h"

			/////////////////////
			///  Constructor  ///
			/////////////////////
DCParProcs::DCParProcs(int n, BaseMultiTarget* t) : ParProcessors(n,t) {
	LOG_NEW; schedules = new DCUniProc[n];
	SCommNodes.initialize();
	commCount = 0;
}

UniProcessor* DCParProcs :: getProc(int num) { return getSchedule(num); }

			////////////////////
			///  Destructor  ///
			////////////////////

DCParProcs::~DCParProcs() {
	LOG_DEL; delete [] schedules;
	SCommNodes.removeNodes();
}

			////////////////////
			///  initialize  ///
			////////////////////
void DCParProcs::initialize() {
	commCount = 0;
	ParProcessors :: initialize();
}

// copy node lists
static void copyNodes(DCNodeList& from, DCNodeList& to) {
	to.initialize();
	DCNodeListIter niter(from);
	DCNode* node;
	while ((node = niter++) != 0)
		to.append(node);
}

// find the earliest time when all ancestors are scheduled.
static int ancestorsFinish(DCNode* node) {
	int finish = 0;
	DCAncestorIter iter(node);
	DCNode *pg;
	while ((pg = iter++) != 0) {
		if (pg->getFinishTime() > finish)
			finish = pg->getFinishTime();
	}
	return finish;
}

			//////////////////
			///  copyInfo  ///
			//////////////////
// save scheduling information of the nodes
void DCParProcs::saveBestResult(DCGraph* graph) {
	// copy info for all regular nodes
	graph->copyInfo();

	// for communication nodes
	DCNodeListIter Noditer(SCommNodes);
	DCNode *nodep;
	while ((nodep = Noditer++) != 0) {
		nodep->saveInfo();
	}
}

			/////////////////////
			///  listSchedule ///
			/////////////////////
// The list scheduler which obtains the makespan.  It finds all the
//      instances of interprocessor communication, constructs
//      new DCNodes to represent them, and schedules them along
//      with the other DCNodes.
int DCParProcs :: listSchedule(DCGraph* graph) {

	// runnable nodes
	DCNodeList readyNodes;
	copyNodes(graph->InitNodes, readyNodes);

	// reset the graph
	graph->replenish(0);

	// find communication nodes and insert them into the graph.
	findCommNodes(graph);

	while (readyNodes.mySize() > 0) {
		DCNode* node = readyNodes.takeFromFront();
		DCUniProc* p = getSchedule(node->whichProc());

		// compute the earliest schedule time
		int start = ancestorsFinish(node);
		
		// If node is a comm node, call topology dependent section
		// to see if there are any constraints
		if (node->getType() < 0) {
			int t = mtarget->scheduleComm(node, start);
			if (start < t) start = t;
		}
		
		// schedule node
		p->addNode(node, start);

		// check whether any descendant is runnable after this node.
		DCDescendantIter iter(node);
		DCNode* n;
		while ((n = iter++) != 0) {
			if (n->fireable())
				graph->sortedInsert(readyNodes, n, 1);
		}
	}
	return getMakespan();
}

			///////////////////////
			///  findCommNodes  ///
			///////////////////////
// Makes send and receive DCNodes for each interprocessor communication.
// Want to splice these into tempAncs and tempDescs for list scheduling.

void DCParProcs::findCommNodes(DCGraph* graph) {

	// Make sure the list of communication nodes is clear
	SCommNodes.removeNodes();

	DCIter iter(*graph);
	DCNode *pg, *desc;
	while ((pg = iter++) != 0) {
		EGGateLinkIter giter(pg->descendants);
		EGGate* g;
		while ((g = giter++) != 0) {
			DCNode* desc = (DCNode*)(g->farEndNode());
			int from = pg->whichProc();
			int to = desc->whichProc();
			if (from != to) {
				int num = g->samples();

				// make comm nodes.
				// 1. compute communication times.
				int sTime = mtarget->commTime(from,to,num,0);
				int rTime = mtarget->commTime(from,to,num,1);

				// 2. make send and receive nodes
				LOG_NEW; DCNode* snode = new DCNode(-1);
				snode->assignProc(from);
				snode->assignSL(pg->getSL() + 1);
				snode->setExTime(sTime);
				
				LOG_NEW; DCNode* rnode = new DCNode(-2);
				rnode->assignProc(to);
				rnode->assignSL(desc->getSL());
				rnode->setExTime(rTime);

				// 3. insert them into the graph.
				pg->connectedTo(snode);
				snode->connectedTo(rnode);
				rnode->connectedTo(desc);

				SCommNodes.insert(snode);
				SCommNodes.insert(rnode);
			}
		}
	}
	// Set the communcation node count for the schedule
	commCount = (int)(SCommNodes.mySize() / 2);
}

// reset the visit flag of nodes in the list
static void resetVisitFlag(DCNodeList& nlist) {
	DCNodeListIter iter(nlist);
	DCNode *eg;
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
			if (node->getType() >= -1) { // non-receive node
				slp->takeFromFront(); // cut off node
				continue;
			}

			// Ask topology dependent section if a communication
			// reservation is constraining the SLP.
			commNode = backComm(node);
	
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

			//////////////////
			///  backComm  ///
			//////////////////
// In case of receive node, take out the partner send node
DCNode* DCParProcs :: backComm(DCNode* rNode) {
	DCAncestorIter iter(rNode);
	DCNode* n = iter++;
	return n;
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
			
		
