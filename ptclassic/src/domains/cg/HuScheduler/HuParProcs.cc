static const char file_id[] = "HuParProcs.cc";

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

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "HuParProcs.h"
#include "EGGate.h"
#include "StringList.h"
#include "CGWormhole.h"
#include "Profile.h"
#include "HuNode.h"

// common reference of the idle node
static HuNode idleNode(1);

// constructor
HuParProcs :: HuParProcs(int pNum, MultiTarget* t) : ParProcessors(pNum,t)
{
	LOG_NEW; schedules = new HuUniProc[pNum];
}

HuParProcs :: ~HuParProcs() {
	LOG_DEL; delete [] schedules;
}

UniProcessor* HuParProcs :: getProc(int num) { return getSchedule(num); }

void HuParProcs :: initialize(HuGraph* g) 
{
	ParProcessors :: initialize();

	// member initialization
	myGraph = g;
	clock = 0;
}

/*****************************************************************
		SCHEDULE AIDS
 ****************************************************************/

void HuParProcs :: assignNode(HuNode* pd, int leng, int pNum)
{
	// assign the node to the processor
	HuUniProc* proc = getSchedule(pNum);

	// check whether to insert idle time or not.
	int idle = proc->getIdleTime();
	if (idle > 0)
		proc->appendNode(&idleNode, idle);
	proc->appendNode(pd, leng);
	pd->setScheduledTime(proc->getTimeFree());
	pd->setProcId(pNum);

	// update the clock of the processors.
	int ck = proc->getTimeFree() + leng;
        proc->setTimeFree(ck);
        proc->setAvailTime(ck);
	pd->setFinishTime(ck);
}

void HuParProcs :: setIndex(int v)
{
	HuUniProc* proc = getSchedule(pIndex[v]);
	proc->setIndex(v);
}

// determine the pattern of processor availability and store it
// sorted.  Processor index is also stored for random reference.
void HuParProcs :: determinePPA(IntArray& avail)
{
	// fill out the array initially
	int base = getSchedule(pIndex[0])->getTimeFree();
	for (int i = 0; i < numProcs; i++) {
		avail[i] = getSchedule(pIndex[i])->getTimeFree() - base;
	}
}

// When a processor is assigned a task, the pattern of processor
// availability is changed. Keep track of this change.
void HuParProcs :: renewPatternIndex(int spot)
{
	int temp = pIndex[spot];
	int v = getSchedule(temp)->getTimeFree();
	int j = spot+1;
	// Among those of same timeFree field, 
	// the recented, the smallest index.
	while (j < numProcs && getSchedule(pIndex[j])->getTimeFree() < v) {
		pIndex[j-1] = pIndex[j];
		j++;
	}
	pIndex[j-1] = temp;
}
		
// re-sort the sorted array.
void reSortArray(IntArray& avail, int ix)
{
	int temp = avail[ix];
	int j = ix+1;
	while (j < avail.size() && avail[j] < temp) {
		avail[j-1] = avail[j];
		j++;
	}
	avail[j-1] = temp;
}

// Advance the clock until the free time of processor of pIndex[ix].
// The nodes to be finished during the clock-advancement may make
// some descendents runnable; put them into the runnable list.
void HuParProcs :: advanceClock(int ix)
{
	// If clock is not advanced, just return.
	if (clock >= getSchedule(pIndex[ix])->getTimeFree()) return;

	// Advance the clock
	int k = ix;
	HuUniProc* proc = getSchedule(pIndex[k]);
	clock = proc->getTimeFree();

	// Update the runnable node list to include all descendents
	// of "temp" which have become fireable as a result of "temp"
	// being finished its execution.
	do {	
		int t = proc->getNextFiringTime();
		while ((clock >= t) && (t > 0)) {
			NodeSchedule* ns = proc->nextNodeToBeFired();
			HuNode* temp = (HuNode*) ns->getNode();
			if (temp) {
				if (ns->getDuration()) {
					temp->setAssignedFlag();
				}
				if (temp->alreadyAssigned()) {
					proc->setFiringInfo(ns,
						temp->getFinishTime());
					fireNode(temp,k);
				}
			}
			t = proc->getNextFiringTime();
		}
		k++;
		if (k < numProcs) proc = getSchedule(pIndex[k]);
	} while (k < numProcs);
}
			
// fire a node and add runnable descendants into the list.
void HuParProcs :: fireNode(HuNode* n, int preferredProc) {
	EGGateLinkIter nextKid(n->descendants);
	EGGate* d;
	while ((d = nextKid++) != 0) {
		HuNode* pd = (HuNode*) d->farEndNode();
		if (pd->fireable()) {
			pd->setAssignedFlag();
			myGraph->sortedInsert(myGraph->runnableNodes,pd,1);
			pd->setPreferredProc(pIndex[preferredProc]);
		}
	}
}

void HuParProcs :: checkPreferredProc(int pNum) 
{
	HuUniProc* proc = getSchedule(pNum);
	if (proc->getTimeFree() <= clock) {
		// yes, available
		int temp = 0;
		while (pIndex[temp] != pNum) temp++;
		pIndex[temp] = pIndex[0];
		pIndex[0] = pNum;
	} else {
		// no, check whether we may exchange the nodes or not.
		if (proc->getPrevTime() == clock) {
			HuNode* qn = (HuNode*) proc->getCurrentNode();
			HuNode* q = (HuNode*) getSchedule(pIndex[0])->getCurrentNode();
			if ((qn->getPreferredProc() != pNum) && (qn != q)) {
				// exchange the nodes.
				int length = proc->getAvailTime() - clock;
				proc->removeLastSchedule();
				proc->setAvailTime(clock);
				proc->setTimeFree(clock);
				assignNode(qn, length, pIndex[0]);
				setIndex(0);
				// adjust processor availability pattern
				int temp = 0;
				while (pIndex[temp] != pNum) temp++;
				pIndex[temp] = pIndex[0];
				pIndex[0] = pNum;
			}
		}
	}
}		
				
		

/*****************************************************************
		MAIN SCHEDULE ROUTINES
 ****************************************************************/

// schedule a normal atomic block.
void HuParProcs :: scheduleSmall(HuNode* pd)
{
	// advance the global clock if necessary
	advanceClock(0);

	// check OSOP option
	if (pd->sticky() && pd->invocationNumber() > 1) {
		ParNode* firstN = (ParNode*) pd->myMaster()->myMaster();
		int whichP =  firstN->getProcId();
		assignNode(pd, pd->myExecTime(), whichP);
	} else {
		// check whether preferredProc is available or not.
		checkPreferredProc(pd->getPreferredProc());

		// schedule the node on the first available processor
		assignNode(pd, pd->myExecTime(), pIndex[0]);

		if (pd->myExecTime() == 0) fireNode(pd,0);
	}

	// renew the states of the graph
	myGraph->decreaseNodes();
	myGraph->decreaseWork(pd->myExecTime());

	// renew the pattern of processor availability.
	renewPatternIndex(0);
}

// Schedule a big block (CGWormhole, or dynamic construct).
// There will be idle time at the front of this block due to the
// mismatched pattern of processor availability.
// We try to minimise this slot by scheduling other runnable nodes
// into this idle time slot if possible.
void HuParProcs :: scheduleBig(HuNode* node, int opt, IntArray& avail)
{
	CGStar* wormStar = (CGStar*) node->myStar();
	CGWormhole* worm = wormStar->myWormhole();
	Profile& pf = worm->getProfile(opt);
	// set-up the correct "minWork" member of the graph.
	int small = myGraph->smallestExTime();
	int optNum = pf.getEffP();

	// check whether preferredProc is available or not.
	advanceClock(0);
	checkPreferredProc(node->getPreferredProc());
	int shift = pf.frontIdleLength(avail);

	// schedule the runnable nodes into the idle slot
	// at the front on all assigned processors.
	for (int i = 0; i < optNum; i++) {

		// The amount of the idle time
		int idle = shift - avail[i] + pf.getStartTime(i);

		// advance the global clock
		advanceClock(i);	

		// scan the runnable nodes for best-fit.
		HuNode* tiny;
		while ((tiny = myGraph->findTinyBlock(idle)) != 0) {
			// schedule it.
			assignNode(tiny, tiny->myExecTime(), pIndex[i]);
			// renew the states of the graph
			myGraph->decreaseNodes();
			myGraph->decreaseWork(tiny->myExecTime());
			// renew the "avail" array and patternAvail array.
			renewPatternIndex(i);
			avail[i] += tiny->myExecTime();
			reSortArray(avail,i);
			// get the idle time again.
			idle = shift - avail[i] + pf.getStartTime(i);
			advanceClock(i);
			if (tiny->myExecTime() <= myGraph->getMinWork())
				small = myGraph->smallestExTime();
		}
	}

	// schedule the idle node and the profile into the processors.
	int nonzero = 1;
	for (i = optNum - 1; i >= 0; i--) {
		// amount of idle time
		int idle = shift - avail[i] + pf.getStartTime(i);
		// schedule idle time
		getSchedule(pIndex[i])->incTimeFree(idle);
		// schedule the profile
		int leng = pf.getFinishTime(i) - pf.getStartTime(i);
		if (leng > 0) nonzero--;
		assignNode(node, leng, pIndex[i]);
		setIndex(i);
		// renew the pattern of processor availability.
		renewPatternIndex(i);
	}
	
	// renew the states of the graph
	node->resetAssignedFlag(nonzero);
	myGraph->decreaseNodes();
	myGraph->decreaseWork(node->myExecTime());
}

// schedule idle time.
int HuParProcs :: scheduleIdle() {
	
	// Determine the amount of idle time
	int i = 0;
	while (i < numProcs && getSchedule(pIndex[i])->getTimeFree() <= clock) 
		i++;
	if (i >= numProcs) {
		return FALSE;
	} else {
		advanceClock(i);

		// insert idle time to the processors.
		// After we insert idle time to some processors, 
		// we will have more than one processor with 
		// same "timeFree".  Change the order of the
		// processor availability so that the processor 
		// with least amount of the idle time is the 
		// first available processor among those processors.
		// This scheme will reduce the communication cost.

		int t = getSchedule(pIndex[i])->getTimeFree();
		for (int k = i - 1; k >= 0; k--) {
			getSchedule(pIndex[k])->setTimeFree(t);
		}

		int pN = i + 1;
		while (pN < numProcs && 
			getSchedule(pIndex[pN])->getTimeFree() <= t) pN++;

		// Sort the processor index with idle time.
		// The smallest idle time, the smallest index.
        	for (i = 1; i < pN; i++) {
                	int j = i - 1;
			HuUniProc* up = getSchedule(pIndex[i]);
                	int x = up->getIdleTime();
                	int temp = pIndex[i];
                	while (j >= 0 && 
				x < getSchedule(pIndex[j])->getIdleTime()) {
                        	pIndex[j+1] = pIndex[j];
                        	j--;
                	}
                	j++;
			pIndex[j] = temp;
        	}
		return TRUE;
	}
}
