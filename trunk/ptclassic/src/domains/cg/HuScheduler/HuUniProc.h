#ifndef _HuUniProc_h
#define _HuUniProc_h
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
