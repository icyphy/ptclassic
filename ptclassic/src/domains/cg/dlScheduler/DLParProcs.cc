static const char file_id[] = "DLParProcs.cc";

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

#include "DLParProcs.h"
#include "StringList.h"
#include "EGGate.h"
#include "Profile.h"
#include "DLNode.h"
#include "PriorityQueue.h"
#include "Error.h"

DLParProcs :: DLParProcs(int pNum, MultiTarget* t) :ParProcessors(pNum,t) {
	LOG_NEW; schedules = new UniProcessor[pNum];
	candidate = 0;
}

DLParProcs :: ~DLParProcs() {
	LOG_DEL; delete [] schedules;
}

UniProcessor* DLParProcs :: getProc(int num) { return &(schedules[num]); }

void DLParProcs :: initialize(DLGraph* g) 
{
	ParProcessors :: initialize();
	candidate = mtarget->candidateProcs(this, 0);

	// member initialization
	myGraph = g;
}

/*****************************************************************
		SCHEDULE AIDS
 ****************************************************************/

static PriorityQueue pQue;

// prepare the sortest list of the finish times of the ancestors.
void DLParProcs :: prepareComm(DLNode* node) {
	pQue.initialize();

	// scan the ancestors.
	EGGateLinkIter preciter(node->ancestors);
	EGGate* q;

	while((q = preciter++)!=0) {
		DLNode* p = (DLNode*) q->farEndNode();
		int i = p->getProcId();
		int fTime = p->getFinishTime();
		// sort finishTime...
		pQue.levelput(q,fTime,i);
	}
}

// Firing a node. Insert the descendents if they are runnable as a result.
void DLParProcs :: fireNode(DLNode* temp) {
	EGGateLinkIter nextKid(temp->descendants);
	EGGate* d;
	while ((d = nextKid++) != 0) {
		DLNode* pd = (DLNode*) d->farEndNode();
		if (pd->fireable()) 
			myGraph->sortedInsert(myGraph->runnableNodes,pd,1);
	}
}

static DLNode dummy(-2);

// Examine when the IPC completes, which is given in "pQue".
int DLParProcs :: executeIPC (int destP) {

	// restore the current resource reservation...
	mtarget->restoreCommPattern();

	int start = 0;

	// Check the IPC requirements.
	pQue.reset();
	for (int i = pQue.length(); i > 0; i--) {
		LevelLink* lk = pQue.next();
		int when = int(lk->level);
		int srcP = int(lk->fineLevel);
		EGGate* df = (EGGate*) lk->e;

		// schedule the communication node.
		int t = when;
		if (srcP != destP) {
			int c = mtarget->commTime(srcP,destP,df->samples(),2);
			dummy.setExTime(c);
			t = mtarget->scheduleComm(&dummy,when) + c;
		}
		if (start < t) start = t;
	}
	return start;
}

// Schedule IPC
void DLParProcs :: scheduleIPC (int destP) {

	// restore the current resource reservation...
	mtarget->restoreCommPattern();

	// schedule communication...
	pQue.reset();
	for (int i = pQue.length(); i > 0; i--) {
		LevelLink* lk = pQue.next();
		int when = int(lk->level);
		int srcP = int(lk->fineLevel);
		EGGate* df = (EGGate*) lk->e;

		// schedule the communication node.
		if (srcP != destP) {
			LOG_NEW; DLNode* cnode = new DLNode(-2);
			// comm. cost
			int c = mtarget->commTime(srcP,destP,df->samples(),2);
			cnode->setExTime(c);
			mtarget->scheduleComm(cnode,when);
			SCommNodes.append(cnode);
		}
	}

	// save the current resource reservation...
	mtarget->saveCommPattern();
}

/*****************************************************************
		For Scheduling Atomic Node
 ****************************************************************/

// schedule a normal atomic block.
void DLParProcs :: scheduleSmall(DLNode* pd)
{
	// examine candidate processors
	if (pd->atBoundary()) {
		candidate->truncate(1);
		candidate->elem(0) = 0;
	} else if (pd->sticky() && pd->invocationNumber() > 1) {
		candidate->truncate(1);
		ParNode* firstN = (ParNode*) pd->myMaster()->myMaster();
		candidate->elem(0) = firstN->getProcId();
	} else {
		candidate = mtarget->candidateProcs(this, pd->myMaster());
	}

	if (candidate->size() == 0) {
		Error::abortRun(*pd->myMaster(), " is not supported.",
			" please check resource constraints.");
		return;
	}

	// compare the earliest schedule time with candidate processors.
	// Choose the processor index giving earliest firing time.
	int earliest;
	int optId = compareCost(pd, &earliest);

	// schedule the node on the optimal processor.
	assignNode(pd, optId, earliest);
	fireNode(pd);

	// renew the states of the graph
	myGraph->decreaseNodes();
	myGraph->decreaseWork(pd->getExTime());
}

// Among candidate processors, choose a processor that can execute
// the node earliest.
int DLParProcs :: compareCost(DLNode* node, int* earliest) {

	// prepare the sortest list of the finish times of the ancestors.
	prepareComm(node);

	int leng; 
	if (!node->myMaster()) leng = node->getExTime();

	int ix = 0;	// candidate array index
	int optId = candidate->elem(0);
	*earliest = costAssignedTo(node, optId, executeIPC(optId));
	if (node->myMaster()) leng = mtarget->execTime(node->myMaster(), 
					getProc(optId)->target());
	int min = *earliest + leng;

	int bound = candidate->size() - 1;

	while (ix < bound) {
		ix++;
		int pix = candidate->elem(ix);
		int cost = costAssignedTo(node, pix, executeIPC(pix));
		if (cost >= 0) {
			if (node->myMaster()) leng = mtarget->execTime(
				node->myMaster(), getProc(pix)->target());
			
			if (cost+leng < min) {
				*earliest = cost;
				optId = pix;
				min = cost+leng;
			}
		}
	}
	return optId;
}

// Compute the earliest firing time of the node onto the destP processor.
// The resources are temporarily reserved for this assignment.
int DLParProcs :: costAssignedTo(DLNode* node, int destP, int start) {

	// assigning the node into the processor?
	int availT = getProc(destP)->getAvailTime();
	if (availT <= start)  {
		availT = start;
	} else {		
		// check if there is an idle slot to fit this node in.
		int temp = getProc(destP)->filledInIdleSlot(node, start);
		if (temp >= 0) availT = temp;
	}
	return availT;
}

// assign a atomic node into the "pix" processor at "tm" time slot.
void DLParProcs :: assignNode(DLNode* pd, int destP, int tm) {
	
	// schedule communication...
	scheduleIPC(destP);

	// assigning the node into the processor.
	UniProcessor* proc = getProc(destP);
	int leng;
	if (!pd->myMaster()) leng = pd->getExTime();
	leng = mtarget->execTime(pd->myMaster(), proc->target());

	pd->setProcId(destP);
	pd->setFinishTime(tm + leng);

	int availT = proc->getAvailTime();
	if (availT <= tm) {
		// schedule the node at the end.
		proc->schedAtEnd(pd,tm,leng);
	} else {
		// schedule in the middle (idle slot)
		int when = proc->filledInIdleSlot(pd,tm); 
		if (when < 0)
			Error::abortRun("no enough idle slot!");
		else proc->schedInMiddle(pd,when,leng);
	}
}
	
/*****************************************************************
		For Scheduling Large Node
 ****************************************************************/

// Schedule a big block (CGWormhole, or data parallel stars).
// There will be idle time at the front of this block due to the
// mismatched pattern of processor availability.
void DLParProcs :: scheduleBig(DLNode* node, int when, IntArray& avail)
{
	Profile* pf = node->profile();

	int optNum = pf->getEffP();

	// schedule the idle node and the profile into the processors.
	int shift = pf->frontIdleLength(avail);

	int saveStart, saveFinish = 0;
	for (int i = optNum - 1; i >= 0; i--) {
		// calculate time durations.
		int t = shift + when + pf->getStartTime(i);
		int leng = pf->getFinishTime(i) - pf->getStartTime(i);
		int pix = pIndex[i];
		UniProcessor* proc = getProc(pix);
		proc->schedAtEnd(node, t, leng);
		pf->assign(node->invocationNumber(),pf->getProcId(i),pix);
		// set processor id for IPC.
		if (pf->getProcId(i) == 0) {
			node->setProcId(pix);
			saveStart = t; 
		}
		if (saveFinish < t + leng) saveFinish = t + leng;
	}
	node->setScheduledTime(saveStart);
	node->setFinishTime(saveFinish);
	
	// renew the states of the graph
	fireNode(node);
	myGraph->decreaseNodes();
	myGraph->decreaseWork(node->getExTime());
}

// First, schedule the communication nodes associate with the wormhole node.
// Second, determine the pattern of processor availability and store it
// sorted.  
// Return the schedule time.
// Note that the avail[i] becomes negative when the i-th processor is
// available at the "earliest" time.
int DLParProcs :: determinePPA(DLNode* pd, IntArray& avail)
{
	// examine candidate processors
	candidate = mtarget->candidateProcs(this, pd->myMaster());

	// decide the starting processor assigned to the construct.
	int earliest;
	int optId = decideStartingProc(pd, &earliest);

	// schedule communication.
	scheduleIPC(optId);

	// sort the processor indices with available time.
	sortWithAvailTime(earliest);

	// fill out the array.
	for (int i = numProcs-1; i >= 0; i--) {
		int temp = getProc(pIndex[i])->getAvailTime();
		if (temp > earliest) {
			avail[i] = temp - earliest;	// positive.
		} else {
			if (pIndex[i] == optId) {
				int t = pIndex[0];
				pIndex[0] = optId;
				pIndex[i] = t;
				temp = getProc(t)->getAvailTime();
			}
			avail[i] = temp - earliest;	// non-positive.
		}
	}
	return earliest;
}

// Among candidate processors, choose a processor that can execute
// the node earliest.
int DLParProcs :: decideStartingProc(DLNode* node, int* earliest) {

	// prepare the sortest list of the finish times of the ancestors.
	prepareComm(node);

	int ix = 0;	// candidate array index
	int temp;
	int optId = candidate->elem(0);
	int start = executeIPC(optId);
	temp = getProc(optId)->getAvailTime();
	if (start < temp) start = temp;
	int bound = candidate->size() - 1;

	while (ix < bound) {
		ix++;
		int pix = candidate->elem(ix);
		int cost = -1;

		// If the processor is not available before the earliest
		// time so far, ignore that processor.
		temp = getProc(pix)->getAvailTime();
		if (temp < start) {
			cost = executeIPC(pix);
			if (cost < temp) start = temp;
			else if (cost < start) {
				start = cost;
				optId = pix;
			}
		}
	}
	*earliest = start;
	return optId;
}

// Schedule a big block for invocations > 1. Use the same profile and
// the same set of processors.
void DLParProcs :: copyBigSchedule(DLNode* node, IntArray& avail) {
	Profile* pf = node->profile();
	int optNum = pf->getEffP();

	// processor index
	int ix = pf->assignedTo(1,0);

	// schedule comm.
	prepareComm(node);
	scheduleIPC(ix);

	// fill out the array pIndex[], and avail[].
	for (int i = 0; i < optNum; i++)
		pIndex[i] = pf->assignedTo(1,i);

	int ref = getProc(ix)->getAvailTime();
	avail[0] = 0;
	for (i = 1; i < optNum; i++) {
		avail[i] = getProc(pIndex[i])->getAvailTime() - ref;
	}

	// main scheduling routine.
	scheduleBig(node, ref, avail);
}
