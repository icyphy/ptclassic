static const char file_id[] = "HuUniProc.cc";

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
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "HuUniProc.h"

// Constuctor.
HuUniProc :: HuUniProc() : timeFree(0) {}

int HuUniProc :: getNextFiringTime() {
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

