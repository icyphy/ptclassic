#ifndef _QSUniProc_h
#define _QSUniProc_h
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

#include "QSGraph.h"
#include "UniProcessor.h"

////////////////////////
// class QSUniProc //
////////////////////////

// This class simulates a single processor.

class QSUniProc : public UniProcessor {

private:
	// The scheduled time marched with the global clock, including
	// the idle time at the end.
	int timeFree;

        // The previous available time.
        int prevTime;

public:
	// constructor
	QSUniProc();

	// get the currently executed node and schedule
	EGNode* getCurrentNode() 
		{ return curSchedule ? curSchedule->getNode() : 0; }
	NodeSchedule* getCurrentSchedule() { return curSchedule; }

	// remove the currently executed node
	void removeLastSchedule()
		{ removeLink(curSchedule); }

  	// set, get, increment the timeFree field.
  	void setTimeFree(int t) { prevTime = timeFree; timeFree = t; }
  	void incTimeFree(int t) { prevTime = timeFree; timeFree += t; }
  	int getTimeFree() { return timeFree; }

	// get idle time
	int getIdleTime() { return timeFree - availTime; }

	// get the prevTime
	int getPrevTime() { return prevTime; }

	// deallocate the links of this list.
	// reset the members.
	void initialize() { UniProcessor::initialize();
			    timeFree = 0; }

};

#endif
