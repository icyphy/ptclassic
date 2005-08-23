static const char file_id[] = "PendingEventList.cc";
/**************************************************************************
Version identification:
@(#)PendingEventList.cc	1.12 04/20/98

Copyright (c) 1997 The Regents of the University of California.
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

 Programmer: John Davis
 Date: 12/15/97
        The PendingEventList class is used to store CqLevelLinks
        of the Calendar Queue. Each DEStar which has its starIsMutable
        parameter set to true has a PendingEventList associated with
        it. When a star dies, its PendingEventList removes pending
        events destined for the star.


**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include "Error.h"
#include "PendingEventList.h"
#include "MutableCQEventQueue.h"


PendingEventList::PendingEventList(MutableCQEventQueue *eventQueue) {
	if( eventQueue == 0 ) {
	    Error::abortRun("Null MutableEventQueue pass to PendingEventList.");
	    return;
	}
	myQueue = eventQueue;
}

PendingEventList::~PendingEventList() {
	initialize();
}

Link * PendingEventList::appendGet( CqLevelLink * obj ) 
{
	return (Link *)LinkedList::appendGet( obj );
}

void PendingEventList::removeHeadAndFree() {
	CqLevelLink *aboutToDieLevelLink;
	CqLevelLink *before;
	CqLevelLink *next;
        myQueue->decrementEventCount(); 
	aboutToDieLevelLink = getHeadAndRemove();
        if( aboutToDieLevelLink ) {
            aboutToDieLevelLink->destinationRef = 0; 
	    before = aboutToDieLevelLink->before; 
	    next = aboutToDieLevelLink->next; 
	    if( before ) { 
		before->next = next;
            }
            if( next ) {
                next->before = before;
            }
	    myQueue->putFreeLink(aboutToDieLevelLink); 
	}

}

CqLevelLink * PendingEventList::remove( Link * obj )
{
	return (CqLevelLink *)LinkedList::eraseLinkNotElement( *obj );
}


void PendingEventList::freeEvent( CqLevelLink * aboutToDieLevelLink ) {
	if( aboutToDieLevelLink->destinationRef != 0 ) {
	    Error::abortRun("Attempt to free CqLevelLink that has"
                    "a non-null destinationRef!");
	}
	CqLevelLink *before;
	CqLevelLink *next;
	before = aboutToDieLevelLink->before; 
	next = aboutToDieLevelLink->next; 
	if( before ) { 
	    before->next = next;
        }
        if( next ) {
            next->before = before;
        }

        myQueue->decrementEventCount(); 
	myQueue->putFreeLink(aboutToDieLevelLink); 
}





