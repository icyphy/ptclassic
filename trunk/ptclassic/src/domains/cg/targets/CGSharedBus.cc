static const char file_id[] = "CGSharedBus.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                        All Rights Reserved.

Programmer: Soonhoi Ha

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGSharedBus.h"
#include "KnownTarget.h"

CGSharedBus::CGSharedBus(const char* name,const char* starType,
	const char* desc) : CGFullConnect(name,starType,desc) {}

void CGSharedBus :: resetResources() {
	bus.initialize();
	bestBus.initialize();
}

void CGSharedBus::clearCommPattern() {
	bus.initialize();
}

// By taking one more argument specifying the range of the copying,
// we may save the time.
void CGSharedBus::saveCommPattern() {
	bestBus.copy(&bus);
}

void CGSharedBus::restoreCommPattern() {
	bus.copy(&bestBus);
}

			//////////////////////
			///  scheduleComm  ///
			//////////////////////
// Returns the earliest time the communication node can be scheduled
//	without contention for resources.
// cNode is the communication node to be scheduled
// when is the time its ancestors have finished execution 

int CGSharedBus::scheduleComm(ParNode *cNode, int when, int limit) {
	if ((limit > 0) && (when < bus.getAvailTime())) {
		int t = bus.filledInIdleSlot(cNode, when, limit);
		if (t < 0) return -1;
		else if (t >= bus.getAvailTime())
			bus.schedAtEnd(cNode, t, cNode->getExTime());
		else
			bus.schedInMiddle(cNode, t, cNode->getExTime());
	} else {
		bus.addNode(cNode, when);
	}
	return cNode->getScheduledTime();
}

			/////////////////////////
			///  candidateProces  ///
			/////////////////////////
// Returns a pointer to a list of candidate processors for the cluster.
// Since this is a shared bus architecture, one unused processor
//	is the same as any other, so it returns the used processors
// 	and one unused processor. (to reduce scheduling time)
// Same as default method defined in CGQuasiTarget.cc

Block* CGSharedBus::clone() const {
	LOG_NEW; CGSharedBus *t = 
		new CGSharedBus(readName(),starType(),readDescriptor());
	return &t->copyStates(*this);
}

			//////////////////
			///  backComm  ///
			//////////////////
// For a given communication node, find a comm. node scheduled
// just before the argument node on the same communication resource.

ParNode* CGSharedBus :: backComm(ParNode* n) {
	ProcessorIter iter(bestBus);
	ParNode* pn;
	ParNode* prev = 0;
	while ((pn = iter.nextNode()) != 0) {
		if (pn == n) break;
		else prev = pn;
	}
	if (prev) {
		if (prev->getType() >= 0) return 0;
		else return prev;
	}
	return 0;
}

static CGSharedBus targ("SharedBus","CGStar",
"a shared bus with user-selectable child targets");

static KnownTarget entry(targ,"SharedBus");
