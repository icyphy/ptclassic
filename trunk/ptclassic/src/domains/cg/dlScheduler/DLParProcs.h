#ifndef _DLParProcs_h
#define _DLParProcs_h

#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
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
	DLParProcs(int pNum, MultiTarget* t): ParProcessors(pNum, t) {}
	~DLParProcs() {}

	// initialize
	void initialize(DLGraph*);

//////////  main scheduling routine.

	virtual void scheduleSmall(DLNode*);		// atomic node

protected:
	// The program graph to be scheduled
	DLGraph* myGraph;

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

//////////////////  schedule aids ///////////////////////

	// sortest list of finish times of the ancestors
	virtual void prepareComm(DLNode*);

	// schedule IPC
	virtual void scheduleIPC(int);

	// Emulate IPC for a given destination processor.
	// Return the completion time.
	int executeIPC(int);

//////////////////  schedule ATOMIC node  ///////////////////////
private:

	// compare the dynamic cost of a node for each candidate processor
	// assinged. Return the index of the optimal processor, and the
	// proposed schedule time as a int pointer argument.
	int compareCost(DLNode*, int*);

};

#endif
