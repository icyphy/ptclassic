#ifndef _ParProcessors_h
#define _ParProcessors_h

#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#include "NamedObj.h"
#include "UniProcessor.h"
#include "IntArray.h"
#include "BaseMultiTarget.h"

class ParGraph;

/////////////////////////
// class ParProcessors //
/////////////////////////

// A class for managing the parallel processor schedules.

class ParProcessors {

public:
	// constructor
	ParProcessors(int, BaseMultiTarget*);

	virtual ~ParProcessors();

	// return size
	int size()	{ return numProcs; }

	// sort the processor indices with finish time.
	void sortWithFinishTime();

	// get the i-th processor. Processors are indexed from 0 to
	// numProcs-1. Should be redefined.
	virtual UniProcessor* getProc(int num);

	// map targets to each processor
	void mapTargets();

	// get makespan
	int getMakespan(); 

	// initialize arrays
	void initialize();

	// list scheduling after processor assignment is done.
	// return the makespan.
	int listSchedule(ParGraph* graph);

	// display
	StringList display(NamedObj* gal);

	// create sub galaxies for each processor: code generation.
	void createSubGals();
	StringList displaySubUnivs();

	// code generation
	void generateCode();

	// Match comm. nodes with the comm. stars created during subGalaxy
	// generation.
	ParNode* matchCommNodes(SDFStar*, EGGate*, PortHole*);

protected:
	// number of processors.
	int numProcs;

	// target pointer
	BaseMultiTarget* mtarget;

  	// pattern of processor availability (PPA) is stored in indices.
  	IntArray pIndex;

	// processor indices
	IntArray pId;

	// A list of communication nodes
	EGNodeList SCommNodes;

	// The number of interprocessor communications in the schedule.
	int commCount;

	// makes send and receive nodes for IPC.
	void findCommNodes(ParGraph*, EGNodeList& );

	// create comm nodes
	virtual ParNode* createCommNode(int i);

	// remove comm nodes
	void removeCommNodes();

	// sort the processor ids with available times if they are not unused
	// processors. All unused processors are appended after the processors
	// that are available at the specified limit and before the busy
	// processors at that time.
	void sortWithAvailTime(int);

	// Check whether the user want to assign all invocations of a star
	// into the same processor or not.
	int OSOPreq() { return mtarget->getOSOPreq(); }
};

#endif
