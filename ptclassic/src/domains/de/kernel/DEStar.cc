/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

*******************************************************************/
#include "DEStar.h"
#include "StringList.h"
#include "Output.h"

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

void DERepeatStar :: repeat(float when) {
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


// constructor
DEPriorityStar :: DEPriorityStar()
{
	// initialize the members
	numEvent = 0;
	addState(queueSize.setState("queueSize", this, "1", "queue size"));
	inQueue = 0;		// no queue is initialized.
}

// redefine the start method to initialize the queues.

void DEPriorityStar :: start()
{
	DERepeatStar :: start();
	
	// initialize queues and states
	numEvent = 0;
	if (inQueue)
		for (int i = priorityList.length() - 1; i >= 0; i --)
			inQueue[i].initialize();
	else {
		int l = priorityList.length();
		inQueue = new Queue[l];
	}
}

// set up priorityList
int DEPriorityStar :: IsItRunnable()
{
	priorityList.reset();
	int preNum = numEvent;

	// if event arrives, queue it 
	if (!canGetFired()) {
	   for (int i = priorityList.length() - 1; i >= 0; i--) {
		InDEPort* p = (InDEPort*) priorityList.next()->e;
		if (p->dataNew) {
			numEvent++;
			// queue if queue size is not greater than limit
			if (inQueue[i].length() < int (queueSize)) {
				Particle& pp = p->get();
				Particle* newp = pp.clone();
				*newp = pp;
				inQueue[i].put(newp);
			} else {
				errorHandler.error("warning : queue overflow",
				"class DEPriorityQueue");
			}
		}
	   }
	   if (arrivalTime < completionTime) {
		// launch the feedback event.
	   	if (preNum <= 0) repeat(completionTime);	
	   	return FALSE;
	   }
	} else if (numEvent == 0) return FALSE;
	return TRUE;
}

Particle* DEPriorityStar :: fetchData(InDEPort& p) 
{
	priorityList.reset();
	for (int i = priorityList.length() - 1; i >= 0; i--) {
		if (((InDEPort*) priorityList.next()->e) == &p) {
			if (inQueue[i].length() > 0) {
				numEvent--;
				Particle* pp = (Particle*) inQueue[i].get();
				pp->die();
				return pp;
			}
		}
	}
	// if no particle associated with the input port
	return 0;
}


