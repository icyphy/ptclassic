#ifndef _DLParProcs_h
#define _DLParProcs_h

#ifdef __GNUG__
#pragma interface
#endif

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

#include "ParProcessors.h"
#include "DLGraph.h"

/////////////////////////
// class DLParProcs //
/////////////////////////

// A class for managing the parallel processor schedules.

class DLParProcs : public ParProcessors {

public:
	DLParProcs(int, MultiTarget*);
	~DLParProcs();

	// initialize
	void initialize(DLGraph*);

	// get the i-th processor. Processors are indexed from 0 to
	// numProcs-1.
	UniProcessor* getProc(int num);

//////////  main scheduling routine.

	virtual void scheduleSmall(DLNode*);		// atomic node
	void scheduleBig(DLNode*, int, IntArray&); // large node

	// in case of a wormhole node, we schedule the same profile to
	// the same processor set for all invocations.
	// This routine is called for invocationNumber of node is 
	// greater than 1.
	void copyBigSchedule(DLNode*, IntArray&); 

  	// observe the pattern of processor availability before scheduling
  	// a dynamic construct (or non-atomic node).
	// First, schedule the communication nodes.
	// Second, compute the pattern. Return the proposed schedule time.
  	virtual int determinePPA(DLNode*, IntArray&);

protected:
	// The program graph to be scheduled
	DLGraph* myGraph;

	// processors
	UniProcessor* schedules;

	// candidate processors
	IntArray* candidate;

//////////////////  schedule aids ///////////////////////

	// Compute the earliest firing time of a node if a dest. proc. is
	// assumed. The resources are temporarily reserved for this
	// assignment. 
	// second argument is the processor id. 
	// third argument is the time the node can be scheduled.
	int costAssignedTo(DLNode*, int, int);

	// Fire a node. Check the runnability of descendents.
	virtual void fireNode(DLNode*);

	// assign a atomic node.
	// second argument is the processor id. Third, schedule time.
	void assignNode(DLNode*, int, int);

private:
//////////////////  schedule aids ///////////////////////

	// sortest list of finish times of the ancestors
	virtual void prepareComm(DLNode*);

	// schedule IPC
	virtual void scheduleIPC(int);

	// Emulate IPC for a given destination processor.
	// Return the completion time.
	int executeIPC(int);

//////////////////  schedule ATOMIC node  ///////////////////////

	// compare the dynamic cost of a node for each candidate processor
	// assinged. Return the index of the optimal processor, and the
	// proposed schedule time as a int pointer argument.
	int compareCost(DLNode*, int*);

//////////////////  schedule LARGE node  ///////////////////////

	// Among candidate processors, choose a processor that can execute
	// the node earliest.
	virtual int decideStartingProc(DLNode*, int*);
};

#endif
