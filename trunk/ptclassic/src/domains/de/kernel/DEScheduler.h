/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

 DE Scheduler for single computer is implemented.

 ********************************************************************/
 
#ifndef _DEScheduler_h
#define _DEScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"
#include "Galaxy.h"
#include "DEStar.h"
#include "Scheduler.h"
#include "EventQueue.h"

	////////////////////////////
	// DEScheduler
	////////////////////////////


class DEScheduler : public Scheduler {

	// stoping condition of the scheduler
	float stopTime;

	// detect the delay-free loop
	int checkLoop(PortHole* p, DEStar* s);

	// set the depth of the DEStars.
	int setDepth(PortHole* p, DEStar* s);

	// report delay-free-loop
	int errorDelayFree(PortHole* p);
	void errorUndefined(PortHole* p);

	// If output events are generated at the "start" phase,
	// send them to the global event queue
	void initialFire(Galaxy&);

	// check deley free loop
	int checkDelayFreeLoop(Galaxy&);

	// compute depth of all portholes
	int computeDepth(Galaxy& g);

public:
	// my domain
	const char* domain() const;

	// The global event queue is implemented as a priority queue
	// in DE scheduler.
	EventQueue eventQ;

	// Set up the stopping condition.
	void setStopTime(float limit) {stopTime = limit ;}

	// redefine the wormhole stopping condition
	void resetStopTime(float limit) {
		setStopTime(limit * relTimeScale) ;
	}

	float getStopTime() { return stopTime;}

	// Set the current time
	void setCurrentTime(float val) {
		currentTime = val * relTimeScale ;
	}

	// The setup function initialize the global event queue
	// and notify the blockes of the queue name.
	int setup(Galaxy& galaxy);

	// The run function resumes the run where it was left off.
	int run(Galaxy&);

	// Display the static schedule for SDF wormholes...
	StringList displaySchedule(); 

	// Constructor sets default options
	DEScheduler () { stopTime = 100.0; }

	// output the stopTime
	float whenStop() { return stopTime ;}

	// relative time scale to the outer domain when it is a inner domain.
	float relTimeScale;

	// synchronization mode. It is set by default.
	// If reset, the execution of the process star can be optimized.
	// Only knowledgeable user may reset this flag!
	int syncMode;

	// fetch an event on request.
	int fetchEvent(InDEPort* p, float timeVal);
};

#endif
