#ifndef _QSParProcs_h
#define _QSParProcs_h

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
#include "QSUniProc.h"
#include "QSGraph.h"
#include "ParProcessors.h"

/////////////////////////
// class QSParProcs //
/////////////////////////

// A class for managing the parallel processor schedules.

class QSParProcs : public ParProcessors {

private:
	// global clock
	int clock;

	// schedule a node into a designated processor
	void assignNode(QSNode* pd, int leng, int pNum);

	// The program graph to be scheduled
	QSGraph* myGraph;

	// processors
	QSUniProc* schedules;

	// advance the global clock, and identify the nodes which become
	// runnable at that time.
	void advanceClock(int ix);

	// set the processor index for big blocks.
	void setIndex(int v);

	// fire a node and add runnable descendants into the list.
	void fireNode(QSNode*, int);

public:
	// constructor
	QSParProcs(int pNum, BaseMultiTarget* t);

	~QSParProcs();

	// initialize
	void initialize(QSGraph*);

	// get the i-th processor. Processors are indexed from 0 to
	// numProcs-1.
	QSUniProc* getSchedule(int num) { return &(schedules[pId[num]]); }

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
	void scheduleSmall(QSNode* pd);
	void scheduleBig(QSNode* node, int opt, IntArray& avail);
	int scheduleIdle();
};

#endif
