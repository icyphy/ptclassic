#ifndef _HuScheduler_h
#define _HuScheduler_h
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

#include "ParScheduler.h"
#include "HuParProcs.h"


///////////////////////////
// class HuScheduler //
///////////////////////////

class HuScheduler : public ParScheduler {

public:
  	// Constructor
  	HuScheduler(MultiTarget* t, const char* log) : 
		ParScheduler(t, log), parSched(0) {
		INC_LOG_NEW; myGraph = new HuGraph; exGraph = myGraph; }
	~HuScheduler();

  	// set-up Processors
  	void setUpProcs(int num);

  	// display schedule with "numProcs" processors.
  	StringList displaySchedule();

  	// main body of the schedule
  	int scheduleIt();

	// temporal hack
	int createSubGals();

protected:
   	// The aggregate firing that represents the
   	// schedule.
   	HuParProcs* parSched;

	// makespan
	int getMakespan();

private:
	// temporal hack to prevent createSubGals if wormhole exists
	int wormFlag;

	HuGraph* myGraph;
};

#endif
