#ifndef _DLScheduler_h
#define _DLScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

/****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                        All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision:

*****************************************************************/

#include "ParScheduler.h"
#include "DLParProcs.h"

///////////////////////
// class DLScheduler //
///////////////////////

class DLScheduler : public ParScheduler {
	DLGraph* myGraph;

protected:
   	// The aggregate firing that represents the
   	// schedule.
   	DLParProcs* parSched;

public:
  	// Constructor
  	DLScheduler(BaseMultiTarget* t, const char* log) : 
		ParScheduler(t, log), parSched(0) {
		INC_LOG_NEW; myGraph = new DLGraph; exGraph = myGraph; }
	~DLScheduler();

  	// set-up Processors
  	void setUpProcs(int num);

  	// display schedule with "numProcs" processors.
  	StringList displaySchedule();

  	// main body of the schedule
  	int scheduleIt();
};

#endif
