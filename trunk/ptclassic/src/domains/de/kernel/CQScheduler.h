#ifndef _CQScheduler_h
#define _CQScheduler_h 1

/*
Copyright (c) 1990-%Q% The Regents of the University of California.
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
*/
 
#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"
#include "Galaxy.h"
#include "DEStar.h"
#include "DEScheduler.h"
#include "CQEventQueue.h"
#include "CalendarQueue.h"


	////////////////////////////
	// CQScheduler
	////////////////////////////


class CQScheduler : public DEBaseSched {

	// stoping condition of the scheduler
	double stopTime;

	// detect the delay-free loop
	int checkLoop(PortHole* p, DEStar* s);

	// set the depth of the DEStars.
	int setDepth(PortHole* p, DEStar* s);

	// report delay-free-loop
	int errorDelayFree(PortHole* p);
	void errorUndefined(PortHole* p);

	// check deley free loop
	int checkDelayFreeLoop();

	// compute depth of all portholes
	int computeDepth();

public:
	// my domain
	const char* domain() const;

	// The global event queue is implemented as a calendar queue
	// in this DE scheduler.

	// Here, EventQueue inherits from CalendarQueue
	// rather than PriorityQueue
	CQEventQueue eventQ;

	// Set up the stopping condition.
	void setStopTime(double limit) {stopTime = limit ;}

	// redefine the wormhole stopping condition
	void resetStopTime(double limit) {
		setStopTime(limit * relTimeScale) ;
	}

	double getStopTime() { return stopTime;}

	// Set the current time
	void setCurrentTime(double val) {
		currentTime = val * relTimeScale ;
	}

	// The setup function initialize the global event queue
	// and notify the blockes of the queue name.
	void setup();

	// The run function resumes the run where it was left off.
	int run();

	// Display the static schedule for SDF wormholes...
	StringList displaySchedule(); 

	// Constructor sets default options
	CQScheduler () { stopTime = 100.0; }

	// output the stopTime
	double whenStop() { return stopTime ;}

	// synchronization mode. It is set by default.
	// If reset, the execution of the process star can be optimized.
	// Only knowledgeable user may reset this flag!
	int syncMode;

	// fetch an event on request.
	/*virtual*/ int fetchEvent(InDEPort* p, double timeVal);

	/*virtual*/ BasePrioQueue* queue() { return &eventQ; }

        // class identification
        int isA(const char*) const;
};

#endif
