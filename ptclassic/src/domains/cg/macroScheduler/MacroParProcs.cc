static const char file_id[] = "MacroParProcs.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MacroParProcs.h"
#include "StringList.h"
#include "EGGate.h"
#include "Profile.h"
#include "Error.h"
#include "CGMacroCluster.h"

/*****************************************************************
		For Scheduling Atomic Node
 ****************************************************************/

// schedule a normal atomic block.
void MacroParProcs :: scheduleSmall(MacroNode* pd)
{
	DLParProcs :: scheduleSmall(pd);
	((MacroGraph*)myGraph)->decreaseWork(pd->getExTime());
}

/*****************************************************************
		For Scheduling Large Node
 ****************************************************************/

// Schedule a big block (clusters, or data parallel stars).
// There will be idle time at the front of this block due to the
// mismatched pattern of processor availability.
void MacroParProcs :: scheduleBig(MacroNode* node, int when, IntArray& avail)
{
	Profile* pf = node->profile();

	int optNum = pf->getEffP();

	// schedule the idle node and the profile into the processors.
	int shift = pf->frontIdleLength(avail);

	int saveStart = 0, saveFinish = 0;
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
	((MacroGraph*) myGraph)->decreaseWork(node->getExTime());
}

// First, schedule the communication nodes associate with the macro node.
// Second, determine the pattern of processor availability and store it
// sorted.  
// Return the schedule time.
// Note that the avail[i] becomes negative when the i-th processor is
// available at the "earliest" time.
int MacroParProcs :: determinePPA(MacroNode* pd, IntArray& avail)
{
	// examine candidate processors
	if (!candidateProcsForMacro((CGMacroClusterBag*) pd->myMaster())) {
		Error :: abortRun("not able to schedule a macro node",
		" please check resource constraints");
		return -1;
	}

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
int MacroParProcs :: decideStartingProc(MacroNode* node, int* earliest) {

	// prepare the sortest list of the finish times of the ancestors.
	prepareComm(node);

	int ix = 0;	// candidate array index
	int temp;
	int optId = candidate.elem(0);
	int start = executeIPC(optId);
	temp = getProc(optId)->getAvailTime();
	if (start < temp) start = temp;
	int bound = candidate.size() - 1;

	while (ix < bound) {
		ix++;
		int pix = candidate.elem(ix);
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
void MacroParProcs :: copyBigSchedule(MacroNode* node, IntArray& avail) {
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

			////////////////////////
			///  scheduleParNode ///
			////////////////////////

int MacroParProcs :: scheduleParNode(ParNode* pnode) {
	MacroNode* node = (MacroNode*) pnode;
	Profile* pf = node->profile();
	int num =  pf->getEffP();
	int invocNum = node->invocationNumber();

	// Compare the processor availability and the startTime profile
	int tempIx;
	int tempMax, refMin = -1;
	for (int i = 0; i < num; i++) {
		if (pf->getStartTime(i) == 0) {
			tempIx = pf->assignedTo(invocNum,i);
			tempMax = getProc(tempIx)->getAvailTime();
			if ((refMin < 0) || (refMin < tempMax)) {
				refMin = tempMax;
			} 
		}
	}

	// compute the shift.
	int shift = 0;
	for (i = 0; i < num; i++) {
		tempIx = pf->assignedTo(invocNum,i);
		tempMax = getProc(tempIx)->getAvailTime() - refMin -
			  pf->getStartTime(i);
		if (tempMax > shift) shift = tempMax;
	}

	// schedule the node
	int saveFinish = 0;
	for (i = 0; i < num; i++) {
		tempIx = pf->assignedTo(invocNum,i);
		UniProcessor* proc = getProc(tempIx);
		int leng = pf->getFinishTime(i) - pf->getStartTime(i);
		int when = refMin + shift + pf->getStartTime(i);
		proc->schedAtEnd(node, when, leng);
		if (saveFinish < when + leng) saveFinish = when + leng;
	}
	node->setScheduledTime(refMin + shift);
	node->setFinishTime(saveFinish);
	return TRUE;
}

IntArray* MacroParProcs :: candidateProcsForMacro(CGMacroClusterBag* s) {
	// clear the array.
	int k = 0;

	// Scan the processors. 
	// unused processors and all used processors which satisfy
	// resource constraints.
	for (int i = 0; i < numProcs; i++) {
		UniProcessor* uni = &(schedules[i]);
		CGTarget* t = uni->target();
		if (s && t && !mtarget->childHasResources(*s, i))
			continue;
		candidate[k] = i;
		k++;
	}
	candidate.truncate(k);
	if (!k) return 0;
	else return &candidate;
}

