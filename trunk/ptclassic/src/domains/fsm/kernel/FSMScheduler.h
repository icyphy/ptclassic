#ifndef _FSMScheduler_h
#define _FSMScheduler_h

/* $Id$

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

    // Initialization.
    /*virtual*/ void setup();


    // Get the stopping time.
    /*virtual*/ double getStopTime() { return double(0); }

    // Set the stopping time.
    /*virtual*/ void setStopTime(double);

    // Set the stopping time when inside a Wormhole.
    /*virtual*/ void resetStopTime(double);

    // Input/Output name maps.        //--|
    char* inputNameMap;               //  |
    char* outputNameMap;              //  |
    // Internal event name maps.      //  |--These will be set by Target.
    char* internalNameMap;            //  |				  
                                      //  |
    // Machine type.                  //  |
    char* machineType;                //  |
    // Evaluation type.               //  |
    char* evaluationType;             //  |
    // One-writer rule checking type. //  |
    char* oneWriterType;              //--|

    // Return the input/output multiport of this FSM.
    MultiPortHole* inPorts() { return myInPorts; }
    MultiPortHole* outPorts() { return myOutPorts; }

    // Return the name list of the internal events.
    const StringList* getInternalEventNm() { return &internalEventNm; } 

     // Return my own Tcl interp.
    Tcl_Interp* interp() { return myInterp; }


   // Reset current state to be the initial state.
    // This needs to be invoked while hierarchical entry is initial entry.
    void resetInitState();

protected:
    // Check all stars.
    int checkStars();

    // Setup inputs, outputs and internal events.
    int setupIO();
    // Setup input/output/internal event buffers.
    // Note that: These buffers depend on the type of the machine,
    // so this method should be redefined in derived class.
    virtual int setupIOBuf();
    // Setup the name of each PortHole in the input/output MultiPortHole.
    int setupNameMap(MultiPortHole& mph, const char* Name_Map);

    // The input multiport of this FSM.
    MultiPortHole* myInPorts;

    // The output multiport of this FSM.
    MultiPortHole* myOutPorts;

    // The list of names of internal events. 
    StringList internalEventNm; 

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
};

class SPureSched : public FSMScheduler
{
public:
    // Constructor.
    SPureSched();

    // Destructor
    ~SPureSched();

    // class identification
    int isA(const char*) const;

    // Run (or continue) the simulation.
    /*virtual*/ int run();

    // Return my myAllInBuf and myAllOutBuf pointers;
    int* allInBuf()  { return myAllInBuf; }
    int* allOutBuf() { return myAllOutBuf; }
    
protected:
    // Setup input/output/internal event buffers.
    /* virtual */ int setupIOBuf();

    // Clear output buffers, and set to deafult values.
    void clearOutput(int value);

    // Receive input data, and feed back internal events.
    void receiveData();

    // Send output data.
    void sendData();


    // Input buffers for the internal events and input ports of this FSM.
    int* myAllInBuf;

    // Output buffers for the internal events and output ports of this FSM.
    int* myAllOutBuf;
};

#endif
