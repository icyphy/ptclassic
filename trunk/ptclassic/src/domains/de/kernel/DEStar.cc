/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

*******************************************************************/
#include "DEStar.h"
#include "DERepeatStar.h"
#include "StringList.h"
#include "Output.h"
#include "PriorityQueue.h"

extern  Error errorHandler;

/*******************************************************************

	class DEStar methods

********************************************************************/

// Redefine method setting internal data in the Block
// so that various DE-specific initilizations can be performed.
// If the parent pointer is not provied, it defaults to NULL
Block& DEStar :: setBlock(char* s, Block* parent = NULL) {
	// First invoke the generic setBlock
	Block::setBlock(s,parent);

	// Then, perform the DE specific initialization.
	completionTime = 0.0;
	arrivalTime = 0.0;

	return *this;
}

// initialize DE-specific members.
void DEStar :: prepareForScheduling() {
	arrivalTime = 0.0;
	completionTime = 0.0;
}

void DEStar :: fire() {
	go();
	for (int k = numberPorts(); k > 0; k--) 
		nextPort().sendData();
}

// DERepeatStar constructor

// we need a method (or constructor) to link the feedback connection.
// something like, feedbackIn.connect(feedbackOut, ..)

DERepeatStar :: DERepeatStar() {
	addPort(feedbackIn.setPort("feedbackIn", this));
	addPort(feedbackOut.setPort("feedbackOut", this));

	// make a feedback connection
	feedbackOut.connect(feedbackIn, 0);
}

void DERepeatStar :: refireAtTime(float when) {
	feedbackOut.put(when) << 0.0 ;
}

int DERepeatStar :: canGetFired() {
	int i = feedbackIn.dataNew;
	if (i == TRUE)
		feedbackIn.dataNew = FALSE;	// reset the flag.
	return i;
}

// The following is defined in DEDomain.cc -- this forces that module
// to be included if any DE stars are linked in.
extern const char DEdomainName[];

const char* DEStar :: domain() const {
	return DEdomainName;
}

// start method for DERepeatStar

void DERepeatStar :: start() {
	feedbackOut.put(completionTime) << 0.0;
	feedbackOut.sendData();
}


