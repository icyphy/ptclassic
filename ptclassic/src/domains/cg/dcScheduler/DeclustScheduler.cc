static const char *file_id = "DeclustScheduler.cc";
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

Programmer: Soonhoi Ha based on G.C. Sih's code
Date of last revision: 5/92
'
*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DeclustScheduler.h"
#include "miscFuncs.h"
#include "streamCompat.h"

// function to display the schedule
StringList DeclustScheduler::displaySchedule() {
	StringList out;
	out += bestSchedule->display(galaxy());
	return out;
}

// set up processors
void DeclustScheduler :: setUpProcs(int num) {
	ParScheduler :: setUpProcs(num);
	LOG_DEL; delete schedA;
	LOG_DEL; delete schedB;
	LOG_NEW; schedA = new DCParProcs(num, mtarget);
	LOG_NEW; schedB = new DCParProcs(num, mtarget);
	parProcs = schedA;	// temporal set for manual scheduling.
}

// destructor
DeclustScheduler::~DeclustScheduler() {
	LOG_DEL; delete myGraph;
	LOG_DEL; delete schedA;
	LOG_DEL; delete schedB;

	// remove clusters
	EClusts.removeDCClusters();
	combinations.removeDCClusters();
}

			/////////////////////
			///  preSchedule  ///
			/////////////////////

// At first, check whether the galaxy contains any wormhole. If yes,
// return FALSE since this scheduler does not work right for wormholes.
// prepare scheduline: Here, we do the first two steps of declustering:
// (1) find elementary clusters and (2) build a cluster hierarchy.

int DeclustScheduler::preSchedule() {

	// check if wormhole exists or not
	if (withParallelStar()) {
		Error::abortRun("Declustering algorithm can not handle ",
			"parallel stars. Please use another scheduler");	
		return FALSE;
	}

	if (logstrm)
		*logstrm << "Created DCGraph, moving to elementary clusters\n";

	EClusts.initialize();

	// Finds elementary clusters and stores pointers to them in EClusts
  	FindElemDCClusters();

	if (logstrm)
		*logstrm << "Finished elementary clusters\n";

	// A list of clusters which are created in DCClusterUpDCClusters
	// The last cluster created is first in the list
	combinations.initialize();

	// Hierarchical DCCluster Grouping
	if(!DCClusterUp(combinations)) {
		Error::abortRun("cluster grouping failed!");
		return FALSE;
	}

	if (logstrm) {
		*logstrm << "Finished hierarchical cluster grouping\n";
		*logstrm << "combinations are:\n";
		*logstrm << combinations.print();
	}
	return TRUE;
}

			////////////////////
			///  scheduleIt  ///
			////////////////////
// Computes the schedule. Do the other two steps of declustering:
// (3) cluster decomposition, (4) cluster breakdown.
// Returns TRUE if everything went ok, else returns FALSE

int DeclustScheduler::scheduleIt() {

	EClusts.resetList();
	combinations.resetList();
	
	if (logstrm) {
		*logstrm << "\nstarted pulldown: cluster decomposition.\n";
	}

	// Initialize remaining clusters to elementary clusters
	DCClusterList remDCClusters(EClusts);

	// DCCluster Hierarchy Decomposition
	pulldown(&combinations, &remDCClusters);
	if (haltRequested()) return FALSE;

	if (logstrm) {
		*logstrm << "\n***  Remaining clusters after pulldown  ***\n";
		*logstrm << remDCClusters.print();
	}

	// Examine the best schedule and use cluster shifting techniques
	//	to improve it while breaking down the cluster granularity 
	if (remDCClusters.listSize() > 0) scheduleAnalysis(remDCClusters);
	if (haltRequested()) return 0;

	if (logstrm) {
		*logstrm << "The best schedule after cluster shifting is \n";
		*logstrm << bestSchedule->display(galaxy());
	}

	// If necessary, break down granularity of elementary clusters 
	// Currently, cluster is not broken down if OSOPreq() is set.
	if(!OSOPreq()) {
		clusterBreakdown();
		if (haltRequested()) return 0;
	}

	if (logstrm) {
		*logstrm << "The best schedule after cluster breakdown is \n";
		*logstrm << bestSchedule->display(galaxy());
	}

	// Important: set parProcs to the bestSchedule
	parProcs = bestSchedule;
	bestSchedule->finalizeGalaxy(myGraph);

	return TRUE;
}

			///////////////////
			///  copyNodes  ///
			///////////////////
// Copies a DCNodeList from model to target
static void copyNodes(DCNodeList &model, DCNodeList &t) {
	t.initialize();
	DCNodeListIter iter(model);
	DCNode *node;
	while ((node = iter++) != 0) {
		t.append(node);
	}
}

			/////////////////////
			///  assignNodes  ///
			/////////////////////
// Assigns the DCNodes in the list to the given proc
static void assignNodes(DCNodeList &list, int proc) {
	DCNodeListIter iter(list);
	DCNode *n;
	while ((n = iter++) != 0) {
		n->assignProc(proc);
	}
}

			//////////////////////
			///  setupSamples  ///
			//////////////////////
// Updates array ins by adding the number of data samples passed into the
//	given node from ancestors on other processors, and updates the
//	array outs by adding the number of data units passed out of the
//	given node to descendants on other processors.

static void setupSamples(DCNode *node, int *ins, int *outs) {

	int pix = node->getProcId(); // The processor that node is on

	// Iter through the tempAncs to update incomms 
	DCAncestorIter aiter(node);
	DCNode *n;
	while ((n = aiter++) != 0) {
		// Ignore communication nodes, and make sure IPC occurs
		if ((n->getType() == 0) && (n->getProcId() != pix)) {
			// The number of samples passed from n to node
			int samps = n->getSamples(node);
			ins[n->getProcId()] += samps;
		}
	}

	// Iter through the tempDescs to update outcomms 
	DCDescendantIter diter(node);
	while ((n = diter++) != 0) {
		if ((n->getType() > 0) && (n->getProcId() != pix)) {
			// The number of samples passed from node to n
			int samps = node->getSamples(n);
			outs[n->getProcId()] += samps;
		}
	}
}

                        ////////////////////
                        ///  adjacentIn  ///
                        ////////////////////

// return an adjacent node in the given node list.
// If more than one or no adjacent node in the list, return 0;
// direction = 1 when the node is an ancestor, -1 if a descendant.

static DCNode* adjacentIn(DCNode* node, DCNodeList& nlist, int& direction) {

	int count = 0;
	DCNode* adj = 0;
	DCAncestorIter aiter(node);
	DCNode* n;
	while ((n = aiter++) != 0)
		if (nlist.member(n)) {
			if (count == 1) return 0;
			adj = n;
			count = 1;
			direction = 1;
		}

	DCDescendantIter diter(node);
	while ((n = diter++) != 0)
		if (nlist.member(n)) {
			if (count == 1) return 0;
			adj = n;
			count = 1;
			direction = -1;
		}
        return adj;
}

			//////////////////////////
			///  clusterBreakdown  ///
			//////////////////////////
// This function isolates the processor sections of the schedule
//	limiting progression for bestSchedule and tries to break
//	them off onto other processors.
// It has the capability to break elementary processors.

int DeclustScheduler::clusterBreakdown() {
	// Make sure the clusters are at the elementary level
	EClusts.setDCClusters();

	DCNodeList SLP;
        if (!bestSchedule->findSLP(&SLP)) {
		Error::abortRun("Problem in finding SLP");
		return FALSE;
	}
	if (logstrm) {
		*logstrm << "\nThe SLP in clusterBreakdown is \n";
		*logstrm << SLP.print();
	}

	int bestProc = 0;
	int bestMspan = bestSchedule->getMakespan();
	int changeFlag = 0;

	DCNodeList SLPsection, bestBreak; // nodelist to hold SLP on curProc

	while (SLP.size() > 0) {
		// Start finding the next SLPsection
		SLPsection.initialize();

		DCNode* node = (DCNode*)SLP.headNode();
		int curProc = node->getProcId(); 

		// Find the non-communication SLP nodes on this processor
		while (node && (node->getProcId() == curProc)) {
			if (node->getType() == 0) SLPsection.insert(node);
			node = (DCNode*) SLP.takeFromFront();
			if (SLP.size()) node = (DCNode*) SLP.headNode();
			else node = 0;
		}
			
		// Here, SLPsection contains the SLPnodes on curProc
		int mspan = findBreakpaths(SLPsection, curProc);

		if (mspan < bestMspan) { // Keep the best cluster break
			changeFlag = 1;
			bestMspan = mspan;
			copyNodes(SLPsection, bestBreak);
			bestProc = curProc;
		}
	}

	// Implement the best break.
	if (changeFlag) {
		if (logstrm) {
			*logstrm << "\nThe set of nodes to be broken down:";
			*logstrm << bestBreak.print() << "\n";
		}
		assignNodes(bestBreak, bestProc);
		workSchedule->initialize();
		mtarget->clearCommPattern();
		workSchedule->listSchedule(myGraph);
		// Save the best schedule
		workSchedule->saveBestResult(myGraph);
		mtarget->saveCommPattern();

		// interchange best and workSchedule.
		DCParProcs* temp = workSchedule;
		workSchedule = bestSchedule;
		bestSchedule = temp; 
	}
	return TRUE;
}

			////////////////////////
			///  findBreakpaths  ///
			////////////////////////
// Tries breaking sections of the SLP onto other processors. 
// slpsect is the section of the slp on Proc (not including comm nodes)
// Returns the best makespan from breaking clusters in this slpsect
//	and leaves with the best break nodes in slpsect, and the
//	processor the nodes were shifted to in Proc.

int DeclustScheduler::findBreakpaths(DCNodeList &slpsect, int &Proc) {
	
	int bestM = bestSchedule->getMakespan();
	int bMspan = bestM;

	// amount of IPC.
	LOG_NEW; int *incomms = new int [numProcs];
	LOG_DEL; int *outcomms = new int [numProcs];

	DCNodeList bestList;		// temporary storage for the best 
	bestList.initialize();		// break path.

	DCNodeListIter iter(slpsect);
	DCNode* node;

	iter.reset();
	while ((node = (DCNode*) iter++) != 0) {

		// if the node is connected to only one one node in slpsect,
		// it is called an "edge" node. 
		// "direction" indicates whether the adjacent node is an
		// ancestor(1) or a descendant(-1).
		int pix = node->getProcId();
		int direction = 0;
		if(!adjacentIn(node, slpsect,direction)) continue;
		
		// path of nodes for potential shift
		DCNodeList shiftPath;
		shiftPath.initialize();
		int runShift = 0;
		
		// clear communication requirements.
		for (int i = 0; i < numProcs; i++) {
			incomms[i] = 0; outcomms[i] = 0;
		}

		// setUp the arrays with communication sample info
		setupSamples(node, incomms, outcomms);

		// extend the shiftPath
		int shiftFlag = TRUE;
		while (node) {
			shiftPath.append(node);
			runShift += node->getExTime(); //Update sum of runtimes

			DCNode* nextN = node->adjacentNode(slpsect,direction);
			if (!nextN) break;

			// scan through the processors
			for (int ix = 0; ix < numProcs; ix++) {
				if (ix == pix) continue;

				int z; // #samples
				int sCost, rCost; // IPC
				int lbound;	// lower bound: runtime shift
						// must exceed comm penalty
				// upper bounds, not to overload destination
				// processor
				int ubound = bestM - 
				   bestSchedule->getSchedule(ix)->getLoad();

				if (direction < 0) {
					z = node->getSamples(nextN);
					sCost = CommCost(ix, pix, z, 0);
					rCost = CommCost(ix, pix, z, 1);
					lbound = rCost - CommCost(ix, pix,
						 incomms[ix],1);
					ubound += CommCost(ix,pix,
						incomms[ix],0) - sCost;
				} else {
					z = nextN->getSamples(node);
					sCost = CommCost(pix, ix, z, 0);
					rCost = CommCost(pix, ix, z, 1);
					lbound = sCost - CommCost(pix,ix,
						outcomms[ix],0);
					ubound += CommCost(pix,ix,outcomms[ix],
						1) - rCost;
				}

				if ((runShift<ubound) && (runShift>lbound)) {
					workSchedule->initialize();
					mtarget->clearCommPattern();

					// shift back and forth
					assignNodes(shiftPath, ix); 
					int m = 
					   workSchedule->listSchedule(myGraph);
					assignNodes(shiftPath, pix); 

					if (m < bMspan) {
				   		bMspan = m;
				   		Proc = ix;
				   		copyNodes(shiftPath, bestList);
					}
				} else if (runShift >= ubound)
					shiftFlag = FALSE;
			}
			if (!shiftFlag) break;
			node = nextN;
		}
	}
	copyNodes(bestList, slpsect);
	LOG_DEL; delete [] incomms;
	LOG_DEL; delete [] outcomms;
	return bMspan;
}

			//////////////////////////
			///  scheduleAnalysis  ///
			//////////////////////////
// Analyzes BestSchedule and tries various techniques to improve it.
// remDCClusts holds the clusters remaining after pulldown. 
// When this function finishes, the cluster hierarchy has been broken
//	down to the elementary cluster level.

void DeclustScheduler::scheduleAnalysis(DCClusterList &remDCClusts) {

	// Set the cluster property for each node to show the cluster it is in
	remDCClusts.setDCClusters();

	// Now find the schedule-limiting-progression
	DCNodeList SLP;
	if (!bestSchedule->findSLP(&SLP)) return;
	if (logstrm) {
		*logstrm << "cluster shifting begins.\n";
		*logstrm << "\nThe SLP is \n";
		*logstrm << SLP.print();
	}

	// Tries commReduction and loadShift routines to get a better schedule.
	int notElem = 1;

	while (notElem == 1) { 
		if (haltRequested()) return;
		DCClusterList slpDCClusts;
		slpDCClusts.findDCClusts(SLP);	// Find the SLP DCClusters

		// Try communication reduction routines
		if (commReduction(remDCClusts, &slpDCClusts) != 0) {
			if (!bestSchedule->findSLP(&SLP)) return;
			slpDCClusts.findDCClusts(SLP);
			if (logstrm) {
				*logstrm << "\nNew SLP after commReduction:\n";
				*logstrm << SLP.print();
			}
		}

		// Try load shifting
		if (loadShift(remDCClusts, &slpDCClusts) != 0) { 
			if (!bestSchedule->findSLP(&SLP)) return;
			if (logstrm) {
				*logstrm << "\nNew SLP after loadShift:\n";
				*logstrm << SLP.print();
			}
		}

		// Break down the cluster granularity 
		DCClusterList cDCClusts(remDCClusts);
		DCClusterListIter iter(cDCClusts);
		notElem = 0;
		DCCluster* clus;
		while ((clus = iter++) != 0) {
			DCCluster* comp1 = clus->getComp1();
			DCCluster* comp2 = clus->getComp2();

			if (comp1 != 0) { // clus is not elementary
				clus->broken();	// Indicate broken cluster
                        	remDCClusts.remove(clus);
                        	remDCClusts.insert(comp1);
                        	remDCClusts.insert(comp2);
				notElem = 1;
			}
		}
		remDCClusts.setDCClusters();
	}
}

			///////////////////
			///  loadShift  ///
			///////////////////
// Tries shifting slp clusters on heavily loaded to lightly loaded processors.

int DeclustScheduler::loadShift(DCClusterList &, DCClusterList *slpC) {

	if (logstrm) {
		*logstrm << "\nStarting loadShift\n";
	}

	int bestM = bestSchedule->getMakespan();
	int changeFlag = 0;

	// Categorize each processor as being heavily(1) or lightly(-1) loaded
	// We include at most one idle processor in lightly loaded procs.
	LOG_NEW; int *procs = new int [numProcs];
	for (int i = 0; i < numProcs; i++) procs[i] = 0;
	bestSchedule->categorizeLoads(procs);

	// Find the SLP clusters which are on the heavy procs
	DCClusterList slpHeavy;
	DCClusterListIter iter(*slpC);
	DCCluster *clust;
	while ((clust = iter++) != 0) {
		int pix = clust->getProc();
		DCUniProc* myProc = bestSchedule->getSchedule(pix);

		// Pick out the slp clusters which are on heavily loaded procs 
		// 	and take less than 80% load of the processor
		if ((procs[pix] > 0) && 
		    (clust->getExecTime() < int(myProc->getLoad() * 0.8)))
			slpHeavy.insert(clust);
	}

	if (logstrm) {
		*logstrm << "slpHeavy is\n" << slpHeavy.print();
	}

	// Try shifting these clusters onto lightly loaded processors
	iter.reconnect(slpHeavy);
	while ((clust = iter++) != 0) {
		int pix = clust->getProc();	// The current proc location

		// Go through the lightly loaded processors
		for (i = 0; i < numProcs; i++) {
			if (procs[i] >= 0) continue;
			int pload = bestSchedule->getSchedule(i)->getLoad();

			// Make sure there is the possibility of a faster sched
			if (clust->getExecTime() < (bestM - pload)) {

				// Try shifting the cluster onto this proc
				clust->assignP(i);

				workSchedule->initialize();
				mtarget->clearCommPattern();
				int mspan =workSchedule->listSchedule(myGraph);

                       		if (mspan < bestM) {
                                	bestM = mspan;
					if (logstrm) {
						*logstrm << "Shifting ";
						*logstrm << clust->print();
						*logstrm << " gives makespan ";
						*logstrm << bestM << ".\n";
					}
					changeFlag = 1;

					// Save the best schedule
					workSchedule->saveBestResult(myGraph);
					mtarget->saveCommPattern();

					// interchange best and workSchedule.
					DCParProcs* temp = workSchedule;
					workSchedule = bestSchedule;
					bestSchedule = temp; 
                        	}
                        	else {
                                	// Switch cluster locations back
					clust->assignP(pix);
                        	}
			}
		}
	}
	LOG_DEL; delete [] procs;
	return changeFlag;
}

			///////////////////////
			///  highestScores  ///
			///////////////////////
// Given a list of clusters, this routine returns the three with
//	highest scores (#samples offproc - #samples onproc) > 0.

static void highestScores(DCClusterList *clusts, DCClusterList &highest) {

	highest.initialize();

	// find out the three clusters with highest scores.
	DCClusterListIter citer(*clusts);
	DCCluster *clus, *clus1 = 0, *clus2 = 0, *clus3 = 0;
	int score, first = 0, second = 0, third = 0;
	while ((clus = citer++) != 0) {
		score = clus->getScore();
		if (score > first) {	// New highest score
			third = second; clus3 = clus2;
			second = first; clus2 = clus1;
			first = score;	clus1 = clus;
		}
		else if (score > second) {
			third = second; clus3 = clus2;
			second = score; clus2 = clus;
		}
		else if (score > third) {
			third = score;  clus3 = clus;
		}
	}
	if (first > 0) highest.append(clus1);
	if (second > 0) highest.append(clus2);
	if (third > 0) highest.append(clus3);
}

			///////////////////
			///  prcDCClusts  ///
			///////////////////
// Fills clist with the clusters from clustlist which are on proc
static void prcDCClusts(DCClusterList &clist, DCClusterList &clustlist, int proc) {

	clist.initialize();

	DCClusterListIter iter(clustlist);
	DCCluster *cl;
	while ((cl = iter++) != 0) {
		if (cl->getProc() == proc) clist.append(cl);
	}
}

			///////////////////////
			///  commReduction  ///
			///////////////////////
// Tries to reduce IPC's by switching clusters that have a high value of
//	samples passed off processor - samples passed on processor
// remC is the list of clusters that are currently intact
// slpC is the list of clusters in the schedule limiting progression
// Returns 1 if a faster schedule was found

int DeclustScheduler::commReduction(DCClusterList &remC, DCClusterList *slpC) {

	DCClusterList highestSLP;

	// Fill highestSLP with the 3 slp clusters with highest scores > 0 
	remC.resetScore();
	myGraph->computeScore();
	highestScores(slpC, highestSLP);

	if (logstrm) {
		*logstrm << "commReduction : 3 chosen slp clusters are\n";
		*logstrm << highestSLP.print();
	}

	if (highestSLP.listSize() == 0) return FALSE;

	int bestMakespan = bestSchedule->getMakespan();
	int changeflag = 0;

	DCClusterListIter iter(highestSLP);
	DCCluster* cls;
	LOG_NEW; int *canProcs = new int [numProcs];

	// Iterate through the 3 slp clusters with highest scores
	while ((cls = iter++) != 0) {
		int myP = cls->getProc();
		if (haltRequested()) return FALSE;

		// Find other valid processors that cls communications with
		for (int i = 0; i < numProcs; i++) canProcs[i] = 0;
		myGraph->commProcs(cls, canProcs);

		// find out the cluster switching candidates
		DCClusterList candDCClusts;

		// Find cluster candidates for switching proc with cls
		// Go through the list of processor candidates for cls
		for (i = 0; i < numProcs; i++) {
			if (canProcs[i] == 0 || i == myP) continue;

			// Fill Pclusts with clusters on processor i
			DCClusterList Pclusts;
			prcDCClusts(Pclusts, remC, i);
			highestScores(&Pclusts, candDCClusts);
		}
	
		// scan through the candDCClusts list for switching
		DCClusterListIter canIter(candDCClusts);	
		DCCluster* cand;
		while ((cand = canIter++) != 0) {
			// Try switching cluster locations
			cls->switchWith(cand);

			// list schedule it
			workSchedule->initialize();
			mtarget->clearCommPattern();
			int finish = workSchedule->listSchedule(myGraph);
                       	if (finish < bestMakespan) {
				bestMakespan = finish;
				if (logstrm) {
					*logstrm << "Switching ";
					*logstrm << cls->print() << " and ";
					*logstrm << cand->print() << " gives ";
					*logstrm << finish << " makespan.\n";
				}
				changeflag = 1;

				// Save the best schedule
				workSchedule->saveBestResult(myGraph);
				mtarget->saveCommPattern();

				// interchange bestSchedule and workSchedule.
				DCParProcs* temp = workSchedule;
				workSchedule = bestSchedule;
				bestSchedule = temp; 
                        } else {
				// Switch cluster locations back
				cls->switchWith(cand);
			}
		}
	}
	LOG_DEL; delete [] canProcs;
	return changeflag;	// Return 1 if accepted a better schedule
}

			//////////////////
			///  pulldown  ///
			//////////////////
// combDCClusts is a list of higher level clusters (not elementary) which
//	were created in DCClusterUp.  This function decomposes these clusters
//	into their cluster components if doing so results in a faster schedule.
// Changes the list *Cleft to contain the set of unbroken clusters, where
//	Cleft initially contains the elementary clusters.

int DeclustScheduler::pulldown(DCClusterList *combDCClusts, DCClusterList *Cleft) {

	DCClusterList reverse;
		
	// initially best schedule is set to schedA
	bestSchedule = schedA;
	workSchedule = schedB;

	// Schedule using the initial placement assigned in DCClusterUp
	schedA->initialize();
	mtarget->clearCommPattern();

	int bestPeriod = schedA->listSchedule(myGraph);
	int bestComm = schedA->commAmount();

	if (logstrm) {
		*logstrm << "\n-- schedule traces --------------------\n";
		*logstrm << "Initial schedule: \n";
		*logstrm << bestSchedule->display(galaxy());
	}

	bestSchedule->saveBestResult(myGraph);
	mtarget->saveCommPattern();

	if (Cleft->listSize() < 2) // If only one elementary cluster
		return TRUE;

	DCClusterListIter iter(*combDCClusts);
	DCCluster *cl;
	while ((cl = iter++) != 0) { // Go through each hierarchical cluster
		reverse.insert(cl);

		// Find which cluster component should be shifted off processor
		DCCluster* pullDCCluster = cl->pullWhich();

		// candidate processors.
		IntArray* canProcs = mtarget->candidateProcs(bestSchedule);

		// Shift onto each of these processors and listSchedule
		int myProc = pullDCCluster->getProc();
		int bestProc = myProc;

		for (int ix = canProcs->size() - 1; ix >= 0; ix--) {
			// processor index
			int pix = canProcs->elem(ix);
			if (pix == myProc) continue;

			// Assign the cluster to a processor 
			pullDCCluster->assignP(pix);

			// schedule the graph.
			workSchedule->initialize();
			mtarget->clearCommPattern();
			int makespan = workSchedule->listSchedule(myGraph);
			int workComm = workSchedule->commAmount();

			if ((makespan < bestPeriod) || 
			 ((makespan == bestPeriod) && (workComm < bestComm))) {
				bestPeriod = makespan;
				bestComm = workSchedule->commAmount();
				bestProc = pix;

				// Save the best schedule
				workSchedule->saveBestResult(myGraph);
				mtarget->saveCommPattern();

				// interchange bestSchedule and workSchedule.
				DCParProcs* temp = workSchedule;
				workSchedule = bestSchedule;
				bestSchedule = temp; 
			} 
		}

		if (bestProc != myProc) {
			// Leave assignment on best processor
			pullDCCluster->assignP(bestProc);

			// Indicate that the cluster was broken
			cl->broken();

			if (logstrm) {
				*logstrm << "New best schedule is: \n";
				*logstrm << bestSchedule->display(galaxy());
			}
		}
		else {	// Unbroken, assign to former processor
			pullDCCluster->assignP(myProc);
		}
	}

	// Finding the unbroken clusters has to be done this way because
	// 	a cluster may be broken further down in the hierarchy.
	iter.reconnect(reverse);
	while ((cl = iter++) != 0) {
		if ((cl->getIntact() == 1) && 
			(Cleft->member(cl->getComp1()) == 1) &&
			(Cleft->member(cl->getComp2()) == 1)) {

			Cleft->remove(cl->getComp1());
			Cleft->remove(cl->getComp2());
			Cleft->insert(cl);
		}
	}
	return TRUE;
}

			//////////////////
			///  CommCost  ///
			//////////////////
// Calls topology dependent section to get the comm cost
// ty = 0 means send time, ty = 1 means recv time, ty = 2 means sum of both
int DeclustScheduler::CommCost(int sP, int dP, int smps, int ty) {
	return mtarget->commTime(sP, dP, smps, ty);
}

			///////////////////
			///  DCClusterUp  ///
			///////////////////
// Does hierarchical clustering of clusters to establish a parallelism
//	hierarchy for the graph
int DeclustScheduler::DCClusterUp(DCClusterList& Nclusts) {

	DCClusterList DCClusts(EClusts);

	while (DCClusts.listSize() >= 2) {

		// Grab the smallest cluster in the list.
		DCCluster* smallest = DCClusts.popHead(); 

		// find the partner.
		DCCluster* other = smallest->findCombiner(); 
		if (!other) other = DCClusts.popHead();
		else	    DCClusts.remove(other);

		// make a macro cluster
		LOG_NEW; DCCluster *combined = new DCCluster(smallest, other);
		combined->setName(myGraph->genDCClustName(1));
		DCClusts.insertSorted(combined);
		Nclusts.insert(combined);
	}

	// initially, all nodes are assigned to a single processor
	if (EClusts.listSize() < 2) {
		EClusts.firstDCClust()->assignP(0);
	} else {
		Nclusts.firstDCClust()->assignP(0);
	}

	return TRUE;
}

			//////////////////////////
			///  FindElemDCClusters  ///
			//////////////////////////
// Finds a set of elementary clusters for the graph.

void DeclustScheduler::FindElemDCClusters()
{
	DCNodeListIter nodeiter(myGraph->BranchNodes);
	DCNode *node;

	// Go through the branch nodes, lowest SL first to find cutArcs
	while ((node = nodeiter++) != 0) {
		ParallelismDetection(node,0);
	}

	// reverse connection
	myGraph->replenish(1);

	// Go through the merge nodes, highest SL first to find cutArcs
	// Remember that tempAncs and tempDescs have already been switched
	nodeiter.reconnect(myGraph->MergeNodes);
	while ((node = nodeiter++) != 0) {
		ParallelismDetection(node,1);
	}

	// Restore tempAncs and tempDescs
	myGraph->replenish(0);

	// create elementary clusters.
	myGraph->formElemDCClusters(EClusts);
}

			//////////////////////////////
			///  ParallelismDetection  ///
			//////////////////////////////
// Does branch analysis for the given branch node, picking a cut-arc
//	for pairs of descendants.
			
int DeclustScheduler::ParallelismDetection(DCNode *bnode, int direction) {

	// Iterator to go through the descendants of bnode.
	DCDescendantIter bdescs(bnode);
	DCNode *D1, *D2, *Mnode;

	if (logstrm) {
		*logstrm << "\nIn Parallelism Detection with node ";
		*logstrm << bnode->print(); 
	}

	// Analyze pairs of descendants
	// Note: tempDescs will be removed in NBranch or IBranch
	while (bnode->amIBranch() >= 1) {

		bdescs.reset();
		D1 = bdescs++;	
		D2 = bdescs++;

 		// Find the first merge node in intersection of TClosures
		Mnode = myGraph->intersectNode(D1,D2,direction);

		if (Mnode == 0) { // Nonintersecting Branch case
			if (logstrm) *logstrm << " (NBranch)\n";
			NBranch(bnode, D1, D2, direction);
		}
		else { // Intersecting Branch case
			if (logstrm) *logstrm << " (IBranch)\n";
			IBranch(bnode, D1, D2, Mnode, direction);
		}
	}
	// Restore the tempDescs of the branchnode
	bnode->copyAncDesc(myGraph,direction);

	return TRUE;
}

			/////////////////
			///  NBranch  ///
			/////////////////
// Finds a CutArc for the NonIntersecting Branch case.
// N1 and N2 are the descendants of a branch node.
// dir = 0 means doing BranchAnalysis, dir = 1 means doing MergeAnalysis

void DeclustScheduler::NBranch(DCNode *BNode, DCNode *N1, DCNode *N2, int dir) 
{

	DCArcList *path1, *path2;

	// List of arcs from N1 to the graph end
	path1 = myGraph->traceArcPath(BNode, N1, 0, dir);

	// List of arcs from N2 to the graph end
	path2 = myGraph->traceArcPath(BNode, N2, 0, dir);

	// bestFinish is initialized to the sum of execution times
	//	of all the nodes in path1 and path2 (no cut case)
	int bestFinish = path1->head()->getT() + path2->head()->getT();
	DCArcIter arciter(*path1);
	DCArc *bestArc = 0;

	// Look at cutting an arc in path 1
	DCArc *cut, *other;
	other = path2->head(); 	// The first arc in path2
	while ((cut = arciter++) != 0) {
		if (cut->betweenSameStarInvocs() && cut->getSrc()->sticky()) 
			continue;
		int finish = NoMerge(cut, other);
		if (finish < bestFinish) {
			bestFinish = finish;
			bestArc = cut;
		}
	}

	// Look at cutting an arc in path 2
	arciter.reconnect(*path2);
	other = path1->head(); 	// The first arc in path1
	while ((cut = arciter++) != 0) {
		if (cut->betweenSameStarInvocs() && cut->getSrc()->sticky()) 
			continue;
		int finish = NoMerge(cut, other);
		if (finish < bestFinish) {
			bestFinish = finish;
			bestArc = cut;
		}
	}

	if (bestArc == 0) {
		if (logstrm) 
			*logstrm << "no cutArc found\n"; 

		// No cutArc selected, delete N2 from the tempDescs of BNode
		BNode->removeDescs(N2);
	} else {
		if (logstrm) 
			*logstrm << "find a cutArc: " << bestArc->print();
		
		// Delete the desc in the path that the bestArc is in
		if (bestArc->parentList() == path1) {
			// Delete N1 from tempDescs of BNode
			BNode->removeDescs(N1);
			path1->remove(bestArc);
		} else {
			// Delete N2 from tempDescs of BNode
			BNode->removeDescs(N2);
			path2->remove(bestArc);
		}

		// If doing MergeAnalysis, reverse the direction of the arc
		if (dir == 1) bestArc->reverse();

		// Put bestArc in cutArcs list
		myGraph->addCutArc(bestArc);
	}
	LOG_DEL; delete path1;
	LOG_DEL; delete path2;
}

			/////////////////
			///  NoMerge  ///
			/////////////////
// Returns the finish time for this NoMerge case.

int DeclustScheduler::NoMerge(DCArc *Cut, DCArc *Oth)
{
	int sendProcFin, recvProcFin, sendtime, recvtime;

	sendtime = CommCost(0, 1, Cut->getS(), 0); // Send comm time
	recvtime = CommCost(0, 1, Cut->getS(), 1); // Recv comm time

	sendProcFin = Cut->getF() + sendtime + Oth->getT();
	recvProcFin = Cut->getF() + sendtime + recvtime + Cut->getT();

	if (sendProcFin > recvProcFin)
		return sendProcFin;
	else
		return recvProcFin;
}


				/////////////////
				///  IBranch  ///
				/////////////////
// Finds two CutArcs for the Intersecting Branch case.
// dir = 0 means doing BranchAnalysis
// dir = 1 means doing MergeAnalysis

void DeclustScheduler::IBranch(DCNode *Bn, DCNode *N1, DCNode *N2, 
				DCNode *Mn, int dir) {

	DCArcList *path1, *path2;

	// List of DCArcs from N1 to the merge node
	path1 = myGraph->traceArcPath(Bn, N1, Mn, dir);

	// List of DCArcs from N2 to the merge node
	path2 = myGraph->traceArcPath(Bn, N2, Mn, dir);

	// Initialize bestFinish to the sum of execution times in both paths
	int bestFinish = path1->head()->getT() + path2->head()->getT();

	DCArc *bestArc1 = 0, *bestArc2 = 0;
	DCArc *other;

	// Look at putting both cuts in path 1
	other = path2->head(); 	// The first arc in path2
	FindBestCuts(path1, path1, other, bestArc1, bestArc2, &bestFinish); 

	// Try both cuts in path2
	other = path1->head(); 	// The first arc in path1
  	FindBestCuts(path2, path2, other, bestArc1, bestArc2, &bestFinish); 

	// Try cuts in different paths
	FindBestCuts(path1, path2, 0, bestArc1, bestArc2, &bestFinish);

	if (bestArc1) {		// Yes, we found a curArc

		if (logstrm) {
			*logstrm << "find a pair of curArcs: (";
			*logstrm << bestArc1->print() << ", ";
			*logstrm << bestArc2->print() << ")\n";
		}

		DCArc *closer;
		// Find out which is closer to the branch node
		if (bestArc1->getF() < bestArc2->getF())
			closer = bestArc1; // bestArc1 is closer
		else
			closer = bestArc2; // bestArc2 is closer

		if (closer->parentList() == path1) {
			Bn->removeDescs(N1); // It is in path 1
		} else {
			Bn->removeDescs(N2); // It is in path 2
		}

		if (dir == 1) {		// Merge Analysis so reverse arcs
			bestArc1->reverse();
			bestArc2->reverse();
		}

		// Add cutArcs 
		bestArc1->parentList()->remove(bestArc1);
		bestArc2->parentList()->remove(bestArc2);
		myGraph->addCutArc(bestArc1);
		myGraph->addCutArc(bestArc2);

	} else {	// No cutArc is found,
		if (logstrm) {
		   *logstrm << "No cutArcs were selected\n";
		}
		// Delete N2 from the tempDescs list for BNode
		Bn->removeDescs(N2);
	}
	LOG_DEL; delete path1;
	LOG_DEL; delete path2;
}

			//////////////////////
			///  FindBestCuts  ///
			//////////////////////
// Finds the best cuts for the Intersecting Branch case
// Returns 1 if better cuts were found, else returns 0
// If other = 0, the cuts are on different paths

void DeclustScheduler::FindBestCuts(DCArcList *list1, DCArcList *list2, 
	DCArc *other, DCArc *bestCut1, DCArc *bestCut2, int *bestFin) {

	DCArcIter iter1(*list1);
	DCArcIter iter2(*list2);
	DCArc *cut1, *cut2;

	while ((cut1 = iter1++) != 0) {

		if (cut1->betweenSameStarInvocs() && cut1->getSrc()->sticky()) 
			continue;
		iter2.reset();
		// If both cuts are on the same arclist, the second arc
		// should be behind the first.
		if (other)  {
			while ((cut2 = iter2++) != cut1) 
				;	// do nothing
		}

		while ((cut2 = iter2++) != 0) {
			if (cut2->betweenSameStarInvocs() && cut1->getSrc()->sticky()) 
				continue;
			int finish = Merge(cut1, cut2, other);

			if (finish < *bestFin) {
				*bestFin = finish;
				bestCut1 = cut1; 
				bestCut2 = cut2; 
			}
		}
	}
}

				///////////////
				///  Merge  ///
				///////////////
// Returns the estimated time that the two paths will finish execution
//	in this intersecting branch case if the two given arcs are cut.
// If the arcs are on the same path, Oth is the first arc in the other path 
// Oth = 0 means the cuts are on different paths.

int DeclustScheduler::Merge(DCArc *C1, DCArc *C2, DCArc *Oth)
{
	int localProcFin, othProcFin;
	int sendtimeC1, recvtimeC1, sendtimeC2, recvtimeC2;
	DCArc *Cut1, *Cut2;

	// Let Cut1 be the arc closer to the branch node 
	if (C1->getF() <= C2->getF()) {
		Cut1 = C1; Cut2 = C2;
	} else {
		Cut1 = C2; Cut2 = C1;
	} 

	// The send and recv communication times for the cuts
	sendtimeC1 = CommCost(0, 1, Cut1->getS(), 0);
	recvtimeC1 = CommCost(0, 1, Cut1->getS(), 1);
	sendtimeC2 = CommCost(0, 1, Cut2->getS(), 0);
	recvtimeC2 = CommCost(0, 1, Cut2->getS(), 1);

	if (Oth == 0) {		// Cuts are on different paths
	    localProcFin = Cut1->getF() + sendtimeC1 + Cut2->getF() +
				sendtimeC2 + recvtimeC2 + Cut2->getT();
	    othProcFin = Cut1->getF() + sendtimeC1 + recvtimeC1 + Cut1->getT()
			+ recvtimeC2 + Cut2->getT();
	} else { 			// Cuts are on the same path
	    localProcFin = Cut1->getF() + sendtimeC1 + recvtimeC2 +
				Cut2->getT() + Oth->getT();
	    othProcFin = Cut1->getF() + sendtimeC1 + recvtimeC1 + 
				Cut1->getT() + sendtimeC2 + recvtimeC2;
	}

	if (localProcFin > othProcFin)
		return localProcFin;
	else
		return othProcFin;
}

