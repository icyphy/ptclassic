/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

 Class DERepeatStar's member functions, formerly here, now have
 their own file DERepeatStar.cc

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DEStar.h"
#include "DEScheduler.h"
#include "StringList.h"
#include "Output.h"
#include "PriorityQueue.h"

/*******************************************************************

	class DEStar methods

********************************************************************/

// initialize DE-specific members.
void DEStar :: prepareForScheduling() {
	arrivalTime = 0.0;
	completionTime = 0.0;
}

void DEStar :: fire() {
	Star::fire();
	BlockPortIter next(*this);
	for (int k = numberPorts(); k > 0; k--) 
		(next++)->sendData();
}

// Set firing mode
void DEStar :: setMode(FiringMode m) {

	mode = m;
	if (mode == PHASE) {
		BlockPortIter next(*this);
		PortHole* p;
		while ((p = next++) != 0) {
			if (p->isItInput()) {
				InDEPort* inp = (InDEPort*) p;
				inp->createQue();
			}
		}
	}
}

// new phase
void DEStar :: startNewPhase() {

	BlockPortIter next(*this);
	PortHole* p;
	while ((p = next++) != 0) {
		if (p->isItInput()) {
			InDEPort* inp = (InDEPort*) p;
			inp->cleanIt();
		}
	}
}

// The following is defined in DEDomain.cc -- this forces that module
// to be included if any DE stars are linked in.
extern const char DEdomainName[];

const char* DEStar :: domain() const {
	return DEdomainName;
}

