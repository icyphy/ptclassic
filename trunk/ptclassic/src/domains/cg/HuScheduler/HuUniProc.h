#ifndef _HuUniProc_h
#define _HuUniProc_h
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

#include "HuGraph.h"
#include "UniProcessor.h"

////////////////////////
// class HuUniProc //
////////////////////////

// This class simulates a single processor.

class HuUniProc : public UniProcessor {

public:
	// constructor
	HuUniProc();

	// get the currently executed node and schedule
	EGNode* getCurrentNode() 
		{ return curSchedule ? curSchedule->getNode() : 0; }
	NodeSchedule* getCurrentSchedule() { return curSchedule; }

	// set last firing information
	void setFiringInfo(NodeSchedule* n, int t) {
		lastFired = n; lastFiringTime = t; nextFiringTime = 0;
		nextFired = 0;
	}
	int getNextFiringTime();
	NodeSchedule* nextNodeToBeFired() { return nextFired; }

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
			    nextFiringTime = 0; lastFiringTime = 0;
			    timeFree = 0; }

private:
	// The scheduled time marched with the global clock, including
	// the idle time at the end.
	int timeFree;

        // The previous available time.
        int prevTime;

	// scheduled finished time of the last fired star.
	int lastFiringTime;
	int nextFiringTime;
	NodeSchedule* lastFired;
	NodeSchedule* nextFired;
};

#endif
