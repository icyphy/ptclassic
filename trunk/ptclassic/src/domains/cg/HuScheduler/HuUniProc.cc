static const char file_id[] = "QSUniProc.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "QSUniProc.h"

// Constuctor.
QSUniProc :: QSUniProc() : timeFree(0) {}

int QSUniProc :: getNextFiringTime() {
	// For first call of this method
	if (lastFiringTime == 0) {
		ProcessorIter iter(*this);
		NodeSchedule* first = iter++;
		while ( first && first->isIdleTime()) 
			first = first->nextLink();
		if (first) {
			ParNode* n = first->getNode();
			nextFiringTime = n->getFinishTime();
			nextFired = first;
		} else {
			nextFiringTime = 0;
			nextFired = 0;
		}
	}
		
	// avoid repetitive computation
	else if (nextFiringTime ==  0) {

		// Compute once for each update.
		NodeSchedule* nextSched = lastFired->nextLink();
		while (nextSched) {
			if (!nextSched->isIdleTime()) {
				ParNode* n = nextSched->getNode();
				nextFiringTime = n->getFinishTime();
				nextFired = nextSched;
				break;
			}
			nextSched = nextSched->nextLink();
		}
	}
	return nextFiringTime;
}

