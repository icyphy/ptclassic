#ifndef _FSMScheduler_h
#define _FSMScheduler_h

/* @(#)FSMScheduler.h	1.9 10/08/98

@Copyright (c) 1996-1999 The Regents of the University of California.
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

    Author:	Bilung Lee
    Created:	3/3/96

*/

#ifdef __GNUG__
#pragma interface
#endif

#include "FSMStateStar.h"
#include "InfString.h"
#include "PortHole.h"
#include "Scheduler.h"
#include "tcl.h"

extern const char FSMdomainName[];

class FSMScheduler : public Scheduler
{
public:
    // Constructor.
    FSMScheduler();

    // Destructor
    ~FSMScheduler();

    // class identification
    int isA(const char*) const;

    // Domain identification.
    /*virtual*/ const char* domain() const { return FSMdomainName; }

    // Run (or continue) the simulation.
    /*virtual*/ int run();

    // Initialization.
    /*virtual*/ void setup();

    // Get the stopping time.
    /*virtual*/ double getStopTime() { return double(0); }

    // Set the stopping time.
    /*virtual*/ void setStopTime(double);

    // Set the stopping time when inside a Wormhole.
    /*virtual*/ void resetStopTime(double);

    // Internal event name maps.       //--|
    InfString intlEventNames;          //  |--These will be set by Target.
    InfString intlEventTypes;          //--|

     // Return my own Tcl interp.
    Tcl_Interp* interp() { return myInterp; }

   // Reset current state to be the initial state.
    // This needs to be invoked while hierarchical entry is initial entry.
    void resetInitState();

protected:
    // Check all stars.
    int checkStars();

    // Receive input data, and feed back internal events.
    int receiveData();

    // Send output data.
    int sendData();

    // (1) Create a Tcl interpreter 
    // (2) Register inputs, outputs, internal events in the Tcl interp
    int setupTclInterp();

    // my own Tcl interpreter
    Tcl_Interp* myInterp;

    // Initial state of this FSM.
    FSMStateStar* initialState;

    // Current state of this FSM.
    FSMStateStar* curState;
    
    // Keep the entry type of last transition that enters current state.
    int curEntryType;

    // Next transition state.
    FSMStateStar* nextState;

    // The domain name outside of this FSM.
    const char* outerDomain;	 

private:
    // Used for debugging
    void printTclVar();
};

#endif
