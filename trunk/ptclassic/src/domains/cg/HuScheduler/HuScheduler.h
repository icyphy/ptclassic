#ifndef _QuasiScheduler_h
#define _QuasiScheduler_h
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
#include "QSParProcs.h"


///////////////////////////
// class QuasiScheduler //
///////////////////////////

class QuasiScheduler : public ParScheduler {
	// temporal hack to prevent createSubGals if wormhole exists
	int wormFlag;

	QSGraph* myGraph;

protected:
   	// The aggregate firing that represents the
   	// schedule.
   	QSParProcs* parSched;

	// makespan
	int getMakespan();

public:
  	// Constructor
  	QuasiScheduler(BaseMultiTarget* t, const char* log) : 
		ParScheduler(t, log), parSched(0) {
		INC_LOG_NEW; myGraph = new QSGraph; exGraph = myGraph; }
	~QuasiScheduler();

  	// set-up Processors
  	void setUpProcs(int num);

  	// display schedule with "numProcs" processors.
  	StringList displaySchedule();

  	// main body of the schedule
  	int scheduleIt();

	// temporal hack
	void createSubGals();
};

#endif
