static const char file_id[] = "BaseMultiTarget.cc";

/******************************************************************
Version identification:
@(#)BaseMultiTarget.cc	1.1	2/5/92

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
#include "Error.h"
#include "Profile.h"
#include <std.h>

// constructor
BaseMultiTarget::BaseMultiTarget(const char* name,const char* starclass,
		   const char* desc) : CGTarget(name,starclass,desc),
	nChildrenAlloc(0), curChild(0) {
        addState(nprocs.setState("nprocs",this,"2","number of processors"));
        addState(inheritProcessors.setState(
	     "inheritProcessors",this,"NO","If yes, inherit child targets"));
        addState(sendTime.setState("sendTime",this,"1",
                                   "time to send one datum"));
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
ParNode* BaseMultiTarget :: backComm(ParNode*) { return 0;}

// By default, assume zero communication time.
int BaseMultiTarget :: scheduleComm(ParNode*,int when) { return when; }

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

