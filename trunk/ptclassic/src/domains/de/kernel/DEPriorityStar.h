/******************************************************************
Version identification:
$Id$

@Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer:  Soonhoi Ha
 Date of creation: 10/2/90 (formerly part of DEStar)

DEPriorityStar.

*******************************************************************/

#ifndef _DEPriorityStar_h
#define _DEPriorityStar_h 1

#include "DERepeatStar.h"
#include "PriorityQueue.h"

	////////////////////////////////////
	// class DEPriorityStar
	////////////////////////////////////

class DEPriorityStar : public DERepeatStar {

protected:
	int numEvent;		// number of queued event

	Queue* inQueue;		// queue for each input ports
	IntState queueSize;	// size of inQueue

	PriorityQueue priorityList;	// priority list of input ports

public:
	// setup priorityList
	void priority(InDEPort& p, int level)
		{priorityList.levelput(&p, float(level)) ;}

	// define start() method to initialize queues, etc.
	void start();

	// when an event is coming while the star is executing the previous
	// event, queue that event and return FALSE.
	// If the incoming event is runnable, return TRUE.
	int isItRunnable();

	// get the particle associated with an input port
	Particle* fetchData(InDEPort& p);

	// go to the next execution. Usually called at the end of go().
	void goToNext() {if (numEvent > 0) refireAtTime(completionTime) ;}

	// constructor
	DEPriorityStar();
};

#endif
