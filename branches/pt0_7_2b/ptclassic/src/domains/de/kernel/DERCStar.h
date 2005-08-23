#ifndef _DERCStar_h
#define _DERCStar_h 1
/**************************************************************************
Version identification:	@(#)DERCStar.h	1.11 04/19/99

Author: Mudit Goel, Neil Smyth, Claudio Passerone

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
*/
 

/* Base class for stars used in a resource contention simulation in 
   the DE domain. It defines several methods that must be provided in 
   the derived stars.
*/

#ifdef __GNUG__
#pragma interface
#endif

#include <assert.h>
#include <stdio.h>
#include "Particle.h"
#include "DERepeatStar.h"
#include "Resource.h"
#include "DERCScheduler.h"
class Resource;

class DERCStar : public DERepeatStar {
 public:
    DERCStar();
    /* virtual */ virtual Block* makeNew() const;
    /* virtual */ virtual const char* className() const;
    /* virtual */ virtual int isA(const char*) const;
    /* virtual */ virtual double getDelay();
    /* virtual */ virtual void emitEvent(Event*, double ) {
        Error::abortRun("Method emitEvent must be overridden in derived classes");
    };
    /* virtual */ virtual int emitEventToIntQ(int, double ) {
        Error::abortRun("Method emitEventtoIntQ must be overridden in derived classes");
        return 0;
    };
    Resource* resourcePointer;
    char resource[1024];
    double timeOfArrival;
    int priority;
    int * lastFiredPr;
    SequentialList* emittedEvents;
    SequentialList* storeList;
    FILE * Openoverflow ( char *name );
    void Printoverflow ( char *st );
    FILE * Openfiring ( char *name );
    void Printfiring ( char *st );
    void Closeflow ();
    int needsSharedResource;
    int schedPolicy;
        
 protected:
    static FILE *fpoverflow;
    static FILE *fpfiring;
};

class StarLLCell {
 public: 
    Event* event;
    double time;    // time when event should be emitted
    int outputPort;   // POLIS specific
    
    StarLLCell(double, int);
};

#endif




