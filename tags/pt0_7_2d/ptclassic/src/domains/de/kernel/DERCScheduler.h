#ifndef _DERCScheduler_h
#define _DERCScheduler_h 1

/**************************************************************************
                                                                           Version identification: @(#)DERCScheduler.h	1.8 05/28/98

 
Author: Mudit Goel, Neil Smyth

Copyright (c) 1997-1998 The Regents of the University of California.
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
#include "Resource.h"
#include <string.h>


	////////////////////////////////////
	//  DERCScheduler
	///////////////////////////////////

class DERCScheduler : public DEBaseSched {

	// stoping condition of the scheduler
	double stopTime;

public:
	// Generate a list of all the distinct resources required by
	// RC stars
	SequentialList *getResources();

	// Stores the resource LinkedList
	SequentialList *resourceList;

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
	DERCScheduler () { stopTime = 100.0; }

	// output the stopTime
	double whenStop() { return stopTime ;}

	// fetch an event on request.
	/*virtual*/ int fetchEvent(InDEPort* p, double timeVal);
	/*virtual*/ BasePrioQueue* queue() { return &eventQ; }

        // class identification
        int isA(const char*) const;

};

#endif
