static const char file_id[] = "MultiTarget.cc";

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha

 Scheduler-independent Baseclass for all multi-processor code generation 
 targets. Virtual methods are provided for uniform wormhole interface.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MultiTarget.h"
#include "SDFScheduler.h"
#include "Error.h"
#include "Profile.h"
#include <std.h>

// constructor
MultiTarget::MultiTarget(const char* name,const char* starclass,
		   const char* desc) : CGTarget(name,starclass,desc),
	nChildrenAlloc(0), curChild(0), iters(0)
{
	// loop scheduler makes no sense for a multitarget
	loopScheduler.setAttributes(A_NONSETTABLE);
        addState(nprocs.setState("nprocs",this,"2","number of processors"));
        addState(inheritProcessors.setState(
	     "inheritProcessors",this,"NO","If yes, inherit child targets"));
        addState(sendTime.setState("sendTime",this,"1",
                                   "time to send one datum"));
        addState(oneStarOneProc.setState("oneStarOneProc",this,"NO",
	   "If yes, place all invocations of a star into the same processor"));
        addState(manualAssignment.setState("manualAssignment",this,"NO",
	   "If yes, specify processor assignment of stars manually"));
        addState(adjustSchedule.setState("adjustSchedule",this,"NO",
	   "If yes, overide the previously obtained schedule by manual assignment"));
}

// Based on priorities of the parameters, we may need to adjust the
// value of the parameters of low priority.
void MultiTarget :: initState() {
	Block :: initState();
	if (int(adjustSchedule)) manualAssignment = TRUE;
	if (int(manualAssignment)) oneStarOneProc = TRUE;
}
	
// associate send and receive stars.  Do nothing by default.
void MultiTarget :: pairSendReceive(DataFlowStar*, DataFlowStar*) {}

void MultiTarget :: setProfile(Profile*) {}
void MultiTarget :: prepareCodeGen() {}

int MultiTarget :: totalWorkLoad() { return -1; }		// undefined...

int MultiTarget :: computeProfile(int pNum, int, IntArray*) {
	// do nothing in this base class.
	return pNum;
}

void MultiTarget :: insideSchedule() {}

void MultiTarget :: downLoadCode(int, Profile*) {}

void MultiTarget :: addProcessorCode(int, const char* s) { myCode += s; }

void MultiTarget :: saveCommPattern() {}
void MultiTarget :: restoreCommPattern() {}
void MultiTarget :: clearCommPattern() {}

// By default, assume zero communication time.
int MultiTarget :: scheduleComm(ParNode*,int when, int) 
	{ return when; }

ParNode* MultiTarget :: backComm(ParNode*) { return 0; }

IntArray* MultiTarget :: candidateProcs(ParProcessors*) 
	{ return NULL; }

void MultiTarget :: setTargets(int n) { 
	nprocs = n; 
	char temp[6];
	sprintf(temp, "%d", n);
	const char* t = hashstring(temp);
	nprocs.setValue(t); }

int MultiTarget :: inheritChildTargets(Target* father) {

        if (int(nprocs) > father->nProcs()) {
                Error::abortRun(name(), " target has more children than \
its parent target: ", father->name());
                return FALSE;
        }

        nChildrenAlloc = int(nprocs);
	inheritChildren(father,0,nChildrenAlloc-1);
	return TRUE;
}

