static const char file_id[] = "BaseMultiTarget.cc";

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

#include "BaseMultiTarget.h"
#include "SDFScheduler.h"
#include "Error.h"
#include "Profile.h"
#include <std.h>

// constructor
BaseMultiTarget::BaseMultiTarget(const char* name,const char* starclass,
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
void BaseMultiTarget :: initState() {
	Block :: initState();
	if (int(adjustSchedule)) manualAssignment = TRUE;
	if (int(manualAssignment)) oneStarOneProc = TRUE;
}
	
int BaseMultiTarget :: run() {
	iters = (int)mySched()->getStopTime();
	mySched()->setStopTime(1);
	int i = Target::run();
	return i;
}

SDFStar* BaseMultiTarget :: createSend(int from, int to, int num) {
	LOG_NEW; return new SDFStar;
}

SDFStar* BaseMultiTarget :: createReceive(int from, int to, int num) {
	LOG_NEW; return new SDFStar;
}

void BaseMultiTarget :: setProfile(Profile*) {}

int BaseMultiTarget :: totalWorkLoad() { return -1; }		// undefined...

int BaseMultiTarget :: computeProfile(int pNum, int, IntArray*) {
	// do nothing in this base class.
	return pNum;
}

void BaseMultiTarget :: insideSchedule() {}

void BaseMultiTarget :: downLoadCode(int, Profile*) {}

void BaseMultiTarget :: saveCommPattern() {}
void BaseMultiTarget :: restoreCommPattern() {}
void BaseMultiTarget :: clearCommPattern() {}

// By default, assume zero communication time.
int BaseMultiTarget :: scheduleComm(ParNode*,int when, int) 
	{ return when; }

ParNode* BaseMultiTarget :: backComm(ParNode*) { return 0; }

IntArray* BaseMultiTarget :: candidateProcs(ParProcessors*) 
	{ return NULL; }

void BaseMultiTarget :: setTargets(int n) { 
	nprocs = n; 
	char temp[6];
	sprintf(temp, "%d", n);
	const char* t = hashstring(temp);
	nprocs.setValue(t); }

int BaseMultiTarget :: inheritChildTargets(Target* father) {

        if (int(nprocs) > father->nProcs()) {
                Error::abortRun(readName(), " target has more children than \
its parent target: ", father->readName());
                return FALSE;
        }

        nChildrenAlloc = int(nprocs);
	inheritChildren(father,0,nChildrenAlloc-1);
	return TRUE;
}

