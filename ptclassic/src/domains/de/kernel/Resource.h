#ifndef _Resource_h
#define _Resource_h 1

/**************************************************************************
Version identification:
@(#)Resource.h	$Id$


Copyright (c) 1998- The Regents of the University of California.
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


**************************************************************************/


/* 
 Programmer:  Neil Smyth
 Date of creation: 11/11/97
 Revisions:
*/


#ifdef __GNUG__
#pragma interface
#endif

#ifndef RoundRobin
#define RoundRobin    0
#endif

#ifndef NonPreemptive
#define NonPreemptive 1
#endif

#ifndef Preemptive
#define Preemptive  2
#endif

#include "type.h"
#include "Galaxy.h"
#include "DataStruct.h"
#include "DERCStar.h"
#include "CQEventQueue.h"
#include "DERCEventQ.h"
#include "DERCScheduler.h"

class DERCEventQ;
class DERCEvent;
class DERCScheduler;

////////////////////////////////////////////////////////////////////////////
// Resource : to be used in a POLIS simulation.
//
// A Resource is used to control access to a simulated resource (eg CPU,
// data bus etc.) during a POLIS simulation. It is designed to be used in 
// conjunction with a DERCScheduler. 
// It looks after keeping track of interrupted processes (more accurately 
// their output Events), as well as deciding when a Star can access this
// resource.
//
////////////////////////////////////////////////////////////////////////////


class Resource {
    public:
    Resource(const char*, int, DERCScheduler* );

    
    void newEventFromEventQ(DERCEvent* , double);
    void newEventFromInterruptQ(DERCEvent*, double);
    void intQupdate(DERCEvent*, double, double);
    int canAccessResource(DERCEvent*);
    // used to get the destination of the event as a DERC Star
    DERCStar* getDERCStar(Event*);
    
    const char* name;
    int schedPolicy;
    double timeWhenFree; // set to ECT of last event in LL, or else -1
    DERCScheduler* mysched;   // the DERCScheduler which created this object
    
    SequentialList* getOtherEvents(DERCEvent*, double);
    // used to store information about interrupted processes
    SequentialList* intEventList; 
    
    // Pointers to the event queues of the DERCScheduler controlling the simulation
    DERCEventQ* eventQ;
    DERCEventQ* interruptQ;
};



////////////////////////////////////////////////////////////////////////////
// ResLLCell : to be used in Links in the Linked List.
//
// It stores information about the interrupted Events, and the output Events
// of the Star which is using the Resource, but which have not yet been 
// outputted.
//
////////////////////////////////////////////////////////////////////////////

class ResLLCell {
public: 
    DERCEvent* event;
    double ECT;     // Expected Completion Time
    int priority;

    ResLLCell( DERCEvent* e, double time, int prio) 
         : event(e), ECT(time), priority(prio) {}

};

#endif
