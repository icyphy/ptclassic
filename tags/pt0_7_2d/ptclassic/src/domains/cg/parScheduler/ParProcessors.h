#ifndef _ParProcessors_h
#define _ParProcessors_h

#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
@(#)ParProcessors.h	1.15	12/9/95

Copyright (c) 1990-1996 The Regents of the University of California.
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

#include "NamedObj.h"
#include "UniProcessor.h"
#include "IntArray.h"
#include "MultiTarget.h"

class ParGraph;

/////////////////////////
// class ParProcessors //
/////////////////////////

// A class for managing the parallel processor schedules.

class ParProcessors {
public:
	// constructor
	ParProcessors(int, MultiTarget*);

	virtual ~ParProcessors();

	// return size
	int size()	{ return numProcs; }

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

	// prepare code generation
	void prepareCodeGen();

	// code generation
	void generateCode();

	// set the indices of candidate processors to schedule the
	// argument star. If none exists, return FALSE.
	IntArray* candidateProcs(DataFlowStar* s);
	
	// Match comm. nodes with the comm. stars created during subGalaxy
	// generation.
	ParNode* matchCommNodes(DataFlowStar*, EGGate*, PortHole*);

        // Move vs. copy stars when making subgalaxies.  The newer
        // schedulers such as the HierScheduler use the StateScope
        // class - so the stars can be moved.  We still support schedulers
        // that require copying of stars for backwards compatibility.  The
        // moveStars member should be set to TRUE in the
        // ParScheduler::setUpProcs virtual method to get the new behavior.
        int moveStars;

	int replaceCommStar(DataFlowStar& /*newStar*/,
				     DataFlowStar& /*oldStar*/);
protected:
	// processors
	UniProcessor* schedules;

	// number of processors.
	int numProcs;

	// target pointer
	MultiTarget* mtarget;

  	// pattern of processor availability (PPA) is stored in indices.
  	IntArray pIndex;

	// A list of communication nodes
	EGNodeList SCommNodes;

	// The number of interprocessor communications in the schedule.
	int commCount;

	// candidate processors
	IntArray candidate;

	// schedule a node of a parallel star
	virtual int scheduleParNode(ParNode*);

	// makes send and receive nodes for IPC.
	void findCommNodes(ParGraph*);

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
