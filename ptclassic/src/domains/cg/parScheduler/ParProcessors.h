#ifndef _ParProcessors_h
#define _ParProcessors_h

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

	// prepare code generation
	void prepareCodeGen();

	// code generation
	void generateCode();

	// Match comm. nodes with the comm. stars created during subGalaxy
	// generation.
	ParNode* matchCommNodes(DataFlowStar*, EGGate*, PortHole*);

protected:
	// number of processors.
	int numProcs;

	// target pointer
	MultiTarget* mtarget;

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
