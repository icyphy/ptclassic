/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

 Definition of DEStar and DERepeatStar (which fires itself repeatedly).

*******************************************************************/
#ifndef _DEStar_h
#define _DEStar_h 1

#include "type.h"
#include "DEConnect.h"
#include "Block.h"
#include "Output.h"
#include "Star.h"

class PriorityQueue;

	////////////////////////////////////
	// class DEStar
	////////////////////////////////////

class DEStar : public Star {

public:
	// Redefine method setting internal data in the Block
	// so that various DE-specific initilizations can be performed.
	// If the parent pointer is not provied, it defaults to NULL
	Block& setBlock(char* starName, Block* parent = NULL);

	// Store the completion time of the current execution, which in turn
	// the next free time. But, it may not be the start time of the next
	// execution since some users may allow several executions 
	// time-overlapped. The start time of the next execution will be
	// either event-arriveTime or MAX { availTime, event-arriveTime },
	// which is user's choice
	// It will be set by go() method in each  star definition.
	float completionTime;
	float arrivalTime;

	// Pointer to the event queue of the universe or warmhole
	// in which the DEstar is. It is set by the Scheduler :
	// initialize().
	PriorityQueue *eventQ;

};

	////////////////////////////////////
	// class DERepeatStar
	////////////////////////////////////

class DERepeatStar : public DEStar {

protected:
	// specify the feedback links to fire itself.
	InDEPort feedbackIn;
	OutDEPort feedbackOut;

public:
	// define the common start function for this class
	// completionTime should be setup beforehand.
	void start() {
		feedbackOut.put(completionTime) << 0.0;
		feedbackOut.sendData();
	}

	// constructor
	DERepeatStar();
};

#endif
