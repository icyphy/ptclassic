#ifndef _FSMScheduler_h
#define _FSMScheduler_h

/*  Version $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

    Author:	Bilung Lee
    Created:	3/3/96

*/

#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"
#include "PortHole.h"
#include "tcl.h"
#include "FSMStateStar.h"
#include "FSMPortHole.h"

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

    // Get the stopping time.
    /*virtual*/ double getStopTime() { return double(0); }

    // Set the stopping time.
    /*virtual*/ void setStopTime(double);

    // Set the stopping time when inside a Wormhole.
    /*virtual*/ void resetStopTime(double);

    // Input/Output name maps.     //--|
    char* inputNameMap;            //  |
    char* outputNameMap;           //  |
    // Internal event name maps.   //  |--These four will be set by Target.
    char* internalNameMap;         //  |				  
                                   //  |
    // Type of this machine.       //  |
    char* machineType;             //--|

    // Return the input/output multiport of this FSM.
    MultiPortHole* inPorts() { return myInPorts; }
    MultiPortHole* outPorts() { return myOutPorts; }

    // Return my own Tcl interp.
    Tcl_Interp* interp() { return myInterp; }

protected:
    // Setup InPorts/OutPorts.
    int setupIOPorts();
    int setNameMap(MultiPortHole& mph, const char* Name_Map);

    // The input multiport of this FSM.
    MultiPortHole* myInPorts;

    // The output multiport of this FSM.
    MultiPortHole* myOutPorts;

    // my own Tcl interpreter
    Tcl_Interp* myInterp;
};

	//////////////////////////////////////////
	// class BasicScheduler
	//////////////////////////////////////////

// This is the scheduler for basic FSM model. (Moore or Mealy machine.)

class BasicScheduler : public FSMScheduler
{
public:
    // Constructor.
    BasicScheduler();

    // class identification
    int isA(const char*) const;

    // Initialization.
    /*virtual*/ void setup();

    // Run (or continue) the simulation.
    /*virtual*/ int run();

protected:
    // Check if the stars in galaxy match the specific machine.
    int checkStars();

private:
    // Current state of this FSM.
    FSMStateStar* curState;

    // Next transition state.
    FSMStateStar* nextState;
};

#endif
