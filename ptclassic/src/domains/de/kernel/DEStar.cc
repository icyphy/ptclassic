static const char file_id[] = "DEStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

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

	// in case of PHASE mode, create internal queue.
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

int DEStar :: fire() {
	int status = Star::fire();
	sendOutput();
	return status;
}

void DEStar :: sendOutput() {
        BlockPortIter next(*this);
        for (int k = numberPorts(); k > 0; k--)
                (next++)->sendData();
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

// isA function
ISA_FUNC(DEStar,Star);
