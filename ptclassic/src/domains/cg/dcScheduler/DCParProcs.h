#ifndef _DCParProcs_h
#define _DCParProcs_h 1
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha based on G.C. Sih's code.

These schedule classes are used by the parallel scheduler

*****************************************************************/

#include "DCUniProc.h"
#include "DCNode.h"
#include "ParProcessors.h"

class DCGraph;
class BaseMultiTarget;

			//////////////////////////
			///  class DCParProcs  ///
			//////////////////////////
// Schedule for multiple processors.
class DCParProcs : public ParProcessors {

private:
	// schedules
	DCUniProc* schedules;

protected:
	ParNode* createCommNode(int i);

public:
	// Constructor takes number of processors as an argument
	DCParProcs(int procs, BaseMultiTarget* mtarget);

	// Destructor
	~DCParProcs();

	// Returns a pointer to the proper UniProc
	DCUniProc *getSchedule(int num) { return &(schedules[pId[num]]); }
	UniProcessor* getProc(int num);

	// return the amount of the IPC.
	int commAmount() { return commCount; }

	// find a schedule limiting progression (SLP)
	// return TRUE, if succeeded.
	int findSLP(DCNodeList*);

	// save best schedule results for nodes
	void saveBestResult(DCGraph*);

	// After the best schedule is obtained, we make a final version
	// of expanded graph including comm. nodes
	void finalizeGalaxy(DCGraph*);

	// Categorizes each processor as being heavily or lightly loaded.
	// It sets an integer array: 1 for heavy and -1 for light processors.
	// Initial threshold is 50% of the maxload.
	// Considers all processors heavily loaded if all processors are loaded
	//      beyond 75% of the maximum load on any processor.
	// We regard at most one idle processor as lightly loaded. Leave other
	// idle processors untouched.
	void categorizeLoads(int* procs);
};

#endif

