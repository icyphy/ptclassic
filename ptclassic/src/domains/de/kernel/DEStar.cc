static const char file_id[] = "DEStar.cc";
/******************************************************************
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

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DEStar.h"
#include "DEScheduler.h"
#include "StringList.h"
#include "PriorityQueue.h"

/*******************************************************************

	class DEStar methods

********************************************************************/

DEStar :: DEStar()
: delayType(FALSE), mode(SIMPLE), arrivalTime(0.0), completionTime(0.0) {}

// initialize DE-specific members.
void DEStar :: initialize() {
	Star::initialize();
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

int DEStar :: run() {
	int status = Star::run();
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
