static const char file_id[] = "DEStar.cc";
/******************************************************************
Version identification:
@(#)DEStar.cc	2.25 03/27/98

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

 Mutability Modifications: John S. Davis II
			   2/12/98

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include "DEScheduler.h"
#include "DEStar.h"
#include "DEWormhole.h"
#include "StringList.h"
#include "PriorityQueue.h"
#include "MutableCQEventQueue.h"
#include "Error.h"



/*******************************************************************

	class DEStar methods

********************************************************************/

DEStar::DEStar() {

    completionTime = 0.0; 
    arrivalTime = 0.0; 
    eventQ = 0; 
    delayType = FALSE; 
    mode = SIMPLE;
    _pendingEventList = 0;
    _mutabilitySet = 0;
    isRCStar = FALSE;
}

DEStar::~DEStar() {
    if( this->isMutable() ) { 
	DETarget *tar = (DETarget *)this->target(); 
	if( tar->isMutable() ) {
	    clearAllPendingEvents();
	    assert( _pendingEventList->size() == 0 );
            delete _pendingEventList;
        }
    }
}

void DEStar::makeMutable() {
    DETarget *tar = (DETarget *)this->target(); 
    if( tar->isMutable() ) { 
	delete _pendingEventList; 
	BasePrioQueue *theQ = ((DEBaseSched *)scheduler())->queue();
	_pendingEventList = new PendingEventList( (MutableCQEventQueue*)theQ);
	_pendingEventList->initialize();
    }
    _mutabilitySet = 1;
}

// initialize DE-specific members.
void DEStar :: initialize() {
	Star::initialize();
	if (SimControl::haltRequested()) return;
	arrivalTime = 0.0;
	completionTime = 0.0;

	// Set the internal pointer to the event queue
	Scheduler *mysched;
	// Need different access methods to the scheduler for
	// wormholes and for stars, unfortunately
	if (isItWormhole()) {
	  mysched = ((DEWormhole*)this)->outerSched();
	} else {
	  mysched = scheduler();
	}
	if (!mysched || !(mysched->isA("DEBaseSched"))) {
	  Error::abortRun(*this,"Scheduler is not a recognized type");
	  return;
	}
	eventQ = ((DEBaseSched*)mysched)->queue();

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

	if( this->isMutable() ) {
	    DETarget *tar = (DETarget *)this->target();
	    if( tar->isMutable() ) {
	       delete _pendingEventList; 
	       _pendingEventList = new 
		       PendingEventList( (MutableCQEventQueue*)eventQ );
	       _pendingEventList->initialize();
	    }
	}
}

// Return TRUE if this star is mutable
int DEStar::isMutable() {
	return _mutabilitySet;
}

int DEStar :: run() {
	int status = Star::run();
	sendOutput();
	return status;
}

// new phase
void DEStar :: startNewPhase() {
	BlockPortIter next(*this);
	DEPortHole* p;
	while ((p = (DEPortHole*) next++) != 0) {
		p->cleanIt();
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

Link* DEStar::addToPendingEventList(CqLevelLink* link) {
	return _pendingEventList->appendGet(link);
}

CqLevelLink * DEStar::removePendingEvent(Link* eventToRemove) {
    return _pendingEventList->remove(eventToRemove);
}

void DEStar::clearAllPendingEvents() {
    if( this->isMutable() ) {
        DETarget *target = (DETarget *)this->target(); 
	if( target->isMutable() && _pendingEventList != 0 ) {
            while( _pendingEventList->size() != 0 ) { 
		_pendingEventList->removeHeadAndFree(); 
            }
	}
    }
}


void DEStar::clearPendingEventsOfPortHole(DEPortHole *dePortHole) {
    if( this->isMutable() ) {
        DETarget *target = (DETarget *)this->target(); 
	if( target->isMutable() && _pendingEventList != 0 ) {
	    int size = _pendingEventList->size();
	    int count = 0;
	    CqLevelLink *cqlLink;
	    // PortHole *portHole = dePortHole;
	    while( count < size ) {
		count++;

		// Take out the head of the Pending Event List
		cqlLink = _pendingEventList->getHeadAndRemove();

		// Check to see if it matches the PortHole
		if( ((Event*)cqlLink->e)->dest == dePortHole ) {
		    cqlLink->destinationRef = 0;
		    _pendingEventList->freeEvent(cqlLink);
		}

		// If not, put it back in the Pending Event List
		else {
		    cqlLink->destinationRef =
			    this->addToPendingEventList(cqlLink);
		}
	    }
	}
    }
}





