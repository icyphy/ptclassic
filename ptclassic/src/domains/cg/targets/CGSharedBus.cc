static const char file_id[] = "CGSharedBus.cc";

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

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGSharedBus.h"
#include "KnownTarget.h"

CGSharedBus::CGSharedBus(const char* name,const char* starType,
	const char* desc) : CGMultiTarget(name,starType,desc) {}

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
// Same as default method defined in CGMultiTarget.cc

Block* CGSharedBus::makeNew() const {
	LOG_NEW; return	new CGSharedBus(name(),starType(),descriptor());
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
