#ifndef _HuParProcs_h
#define _HuParProcs_h

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
#include "HuUniProc.h"
#include "HuGraph.h"
#include "ParProcessors.h"

/////////////////////////
// class HuParProcs //
/////////////////////////

// A class for managing the parallel processor schedules.

class HuParProcs : public ParProcessors {

public:
	// constructor
	HuParProcs(int pNum, MultiTarget* t);

	~HuParProcs();

	// initialize
	void initialize(HuGraph*);

	// get the i-th processor. Processors are indexed from 0 to
	// numProcs-1.
	HuUniProc* getSchedule(int num) { return &(schedules[pId[num]]); }

	UniProcessor* getProc(int num); 

  	// determine the pattern of processor availability before scheduling
  	// a dynamic construct (or non-atomic node).
  	void determinePPA(IntArray& avail);

	// renew PPA since the processor of "spot" index got
	// assigned a new node.
	void renewPatternIndex(int spot);

	// Before schedule an atomic block, check whether the preferred
	// processor is available or not.  If available, assign the node
	// to the processor.  If not available, but exchangeable, exchange
	// the scheduled node.
	void checkPreferredProc(int pNum);

	// schedule blocks.  Depending on the blocks, there are four
	// different methods. Return the current scheduler clock.
	// scheduleSmall: schedule a normal atomic block on a first
	//		  available processor.
	// scheduleBig: schedule a non-atomic block (such as dynamic 
	//		construct) using more than one processors.
	// scheduleIdle: When there is no runnable block at current time,
	//		 increase the scheduler clock to make the first
	//		 available processor finish the current execution
	//		 and provide some runnable nodes.
	void scheduleSmall(HuNode* pd);
	void scheduleBig(HuNode* node, int opt, IntArray& avail);
	int scheduleIdle();

private:
	// global clock
	int clock;

	// schedule a node into a designated processor
	void assignNode(HuNode* pd, int leng, int pNum);

	// The program graph to be scheduled
	HuGraph* myGraph;

	// processors
	HuUniProc* schedules;

	// advance the global clock, and identify the nodes which become
	// runnable at that time.
	void advanceClock(int ix);

	// set the processor index for big blocks.
	void setIndex(int v);

	// fire a node and add runnable descendants into the list.
	void fireNode(HuNode*, int);
};

#endif
