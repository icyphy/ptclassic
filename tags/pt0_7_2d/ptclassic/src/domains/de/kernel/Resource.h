#ifndef _Resource_h
#define _Resource_h 1

/**************************************************************************
Version identification:	@(#)Resource.h	1.11 08/27/99

Copyright (c) 1997-1999 The Regents of the University of California.
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
              Claudio Passerone
 Date of creation: 11/11/97
 Revisions: 4/15/98 version 2, removed the seperate q for pending events. 
     Instead store the pending events in the RC star
*/


#ifdef __GNUG__
#pragma interface
#endif

#ifndef DE_ROUNDROBIN
#define DE_ROUNDROBIN    0
#endif

#ifndef DE_NONPREEMPTIVE
#define DE_NONPREEMPTIVE 1
#endif

#ifndef DE_PREEMPTIVE
#define DE_PREEMPTIVE  2
#endif

#ifndef DE_FIFO
#define DE_FIFO    3
#endif

#ifndef DE_FIFONONPREEMPTIVE
#define DE_FIFONONPREEMPTIVE 4
#endif

#ifndef DE_FIFOPREEMPTIVE
#define DE_FIFOPREEMPTIVE  5
#endif

#include "type.h"
#include "Galaxy.h"
#include "DataStruct.h"
#include "DERCStar.h"
#include "CQEventQueue.h"
#include "DERCScheduler.h"
#include "HashTable.h"

class DERCScheduler;
class ResLLCell;
class DERCStar;

////////////////////////////////////////////////////////////////////////////
// Resource : to be used to simulate resource contention
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

    // Returns scheduling policy code
    int getSchedPolicy(const char*);

    // returns int so it can pass along value from DEStar.run()
    int newEventFromEventQ(CqLevelLink* , double); 
    int canAccessResource(CqLevelLink*);
    // used to get the destination of the event as a DERC Star
    DERCStar* getDERCStar(CqLevelLink*);

    void removeFinishedStar(DERCStar*);
    ResLLCell* getTopCell();
    double getECT(DERCStar*);

    // Register priority so that lastFired can be updated
    int * registerPriority(const char *prkey);
 
    // Register clock frequency for resource
    void registerClock(double clock);

    const char* name;
    int schedPolicy;
    double timeWhenFree; // set to ECT of last event in LL, or else -1
    DERCScheduler* mysched;   // the DERCScheduler which created this object
    
    SequentialList* getOtherLinks(CqLevelLink*, double);
    // used to store information about interrupted processes
    SequentialList* intStarList; 
    
    // Pointers to the eventQ of the DERCScheduler controlling the simulation
    CQEventQueue* eventQ;

    // Added by Claudio Passerone 09/24/1998
    // indexValue of last fired star
    HashTable lastFired;

    // Clock frequency of the resource
    double clkfreq;
};



////////////////////////////////////////////////////////////////////////////
// ResLLCell : to be used in Links in the Linked List.
//
// It stores information about the Star that is using the resource, and 
// the stars that are waiting to finish executing on the resource ( ie stars 
// that were preempted while executing)
//
////////////////////////////////////////////////////////////////////////////

class ResLLCell {
public: 
    DERCStar* star;
    double ECT;     // Expected Completion Time
    int priority;

    // For some reason, under Cygwin32, we need to the body in the .cc file
    ResLLCell( DERCStar* e, double time, int prio); 
};

#endif
