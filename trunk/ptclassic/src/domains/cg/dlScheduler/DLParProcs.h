#ifndef _DLParProcs_h
#define _DLParProcs_h

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

#include "ParProcessors.h"
#include "DLGraph.h"

/////////////////////////
// class DLParProcs //
/////////////////////////

// A class for managing the parallel processor schedules.

class DLParProcs : public ParProcessors {

public:
	// constructor
	DLParProcs(int, BaseMultiTarget*);

	~DLParProcs();

	// initialize
	void initialize(DLGraph*);

	// get the i-th processor. Processors are indexed from 0 to
	// numProcs-1.
	UniProcessor* getProc(int num);

//////////  main scheduling routine.

	void scheduleSmall(DLNode*);		// atomic node
	void scheduleBig(DLNode*, int, int, IntArray&); // large node

  	// observe the pattern of processor availability before scheduling
  	// a dynamic construct (or non-atomic node).
	// First, schedule the communication nodes.
	// Second, compute the pattern. Return the proposed schedule time.
  	int determinePPA(DLNode*, IntArray&);

protected:

	// The program graph to be scheduled
	DLGraph* myGraph;

	// processors
	UniProcessor* schedules;

private:

//////////////////  schedule aids ///////////////////////

	// sortest list of finish times of the ancestors
	void prepareComm(DLNode*);

	// Fire a node. Check the runnability of descendents.
	void fireNode(DLNode*);

	// Emulate IPC for a given destination processor.
	// Return the completion time.
	int executeIPC(int);

	// schedule IPC
	void scheduleIPC(DLNode*, int);

//////////////////  schedule ATOMIC node  ///////////////////////

	// compare the dynamic cost of a node for each candidate processor
	// assinged. Return the index of the optimal processor, and the
	// proposed schedule time as a int pointer argument.
	int compareCost(DLNode*, int*);

	// Compute the earliest firing time of a node if a dest. proc. is
	// assumed. The resources are temporarily reserved for this
	// assignment. 
	// second argument is the processor id. 
	int costAssignedTo(DLNode*, int);

	// assign a atomic node.
	// second argument is the processor id. Third, schedule time.
	void assignNode(DLNode*, int, int);

//////////////////  schedule LARGE node  ///////////////////////

	// Among candidate processors, choose a processor that can execute
	// the node earliest.
	int decideStartingProc(DLNode*, int*);
};

#endif
