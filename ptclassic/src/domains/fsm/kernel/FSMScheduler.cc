static const char file_id[] = "FSMScheduler.cc";

/*
Version $Id$

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
#pragma implementation
#endif

#include "FSMScheduler.h"
#include "utilities.h"

// Ptolemy kernel includes
#include "Galaxy.h"
#include "EventHorizon.h"
#include "Error.h"
#include "InfString.h"
#include "checkConnect.h"		// define warnIfNotConnected


	//////////////////////////////////////////
	// Methods for FSMScheduler
	//////////////////////////////////////////

FSMScheduler::FSMScheduler() {
    currentTime = 0;

    inputNameMap = 0;
    outputNameMap = 0;
    internalNameMap = 0;
    machineType = 0;

    myInPorts = 0;
    myOutPorts = 0;

    initialState = 0;
    curState = 0;
    nextState = 0;
}

// Destructor.
FSMScheduler::~FSMScheduler() {
    delete [] inputNameMap;
    delete [] outputNameMap;
    delete [] internalNameMap;
    delete [] machineType;
}

// Set the stopping time.
void FSMScheduler::setStopTime(double) {
    // reactive system takes no time to output.
}

// Set the stopping time when inside a Wormhole. */
void FSMScheduler::resetStopTime(double) {
    // reactive system takes no time to output.
}

// Initialization.
void FSMScheduler::setup() {
    clearHalt();
    currentTime = 0;

    if (!galaxy()) {
	Error::abortRun("FSMScheduler has no galaxy.");
	return;
    }

    if (warnIfNotConnected(*galaxy())) return;

    // Create the Tcl interpreter for evaluting the conditions in each state.
    // "myInterp" will be passed to each state, so it should be created
    // before galaxy()->initialize()
    if (!(myInterp = Tcl_CreateInterp())) {
      Error::abortRun("FSMScheduler:"," Couldn't create a Tcl interpreter!");
      return; 
    }

    // Setup InPorts/OutPorts. These information will be used in each state
    // setup method, so it should be before galaxy()->initialize().
    if (!setupIOPorts())  return;

    // Setup the internal event list. This list will be used in each state 
    // setup method, so it should be before galaxy()->initialize().
    if (!setupInternalEvent())  return;

    galaxy()->initialize();

    if (!checkStars()) return;

    // Set current state to be the initial state.
    curState = initialState;
    curEntryType = 1;

    // After checkStars(), myInPorts/myOutPorts are setup.
    // Now get the outer parent.
//    MPHIter nexti(*(myInPorts));
//    PortHole *p = nexti++;
//    EventHorizon * eh = p->far()->asEH();
//    myOuterParent = eh->ghostAsPort()->parent()->parent();
//    if (!myOuterParent) return;
}

int FSMScheduler::setupIOPorts() {
    myInPorts = 0;
    myOutPorts = 0;

    GalTopBlockIter next(*galaxy());
    Block *bl;
    while ((bl = next++) != 0) {
      if (bl->isA("FSMDataIn")) {
	bl->initialize();
	myInPorts = bl->multiPortWithName("input");
      }
      else if (bl->isA("FSMDataOut")) {
	bl->initialize();
	myOutPorts = bl->multiPortWithName("output");
      }
    }

    if (myInPorts == 0) {
      Error::abortRun("FSMScheduler:", " No data input star.");
      return FALSE;  
    } else if (myOutPorts == 0) {
      Error::abortRun("FSMScheduler:", " No data output star.");
      return FALSE;
    }

    // Set the name of each PortHole in the input/output MultiPortHole.
    if (!setNameMap(*myInPorts,inputNameMap))    return FALSE;
    if (!setNameMap(*myOutPorts,outputNameMap))  return FALSE;

    MPHIter nextp(*myOutPorts);
    PortHole* p;
    while ( (p = nextp++) != 0 ) {
      // Register each output name into Tcl.
      Tcl_SetVar(myInterp,(char*)p->name(),"0",TCL_GLOBAL_ONLY);
    }

    return TRUE;
}

int FSMScheduler::setNameMap(MultiPortHole& mph, const char* Name_Map) {
    int nNames = 0;
    char **parsedName = strParser(Name_Map,nNames,"double-quote"); 
    if (!parsedName)  return FALSE;

    if (nNames != mph.numberPorts()) {
      delete [] parsedName;

      InfString buf = "FSMScheduler: The number of specified ";
      buf << mph.isItInput() ? "input" : "output";
      buf << " names don't match the number of PortHoles in the ";
      buf << "MultiPortHole.";
      Error::abortRun(buf);
      return FALSE;
    }

    MPHIter nextp(mph);
    PortHole* p;
    int indx = 0;
    while ( (p = nextp++) != 0 ) {
      p->setName((const char*)parsedName[indx]);
      indx++;
    }

    delete [] parsedName;

    return TRUE;
}

int FSMScheduler::setupInternalEvent() {
    // Parse the names specified in "internalNameMap", and store 
    // all parsed names in a StringList "internalEventNm".
    int numNm = 0;
    char** tmpNmList = strParser(internalNameMap, numNm, "double-quote");
    if (!tmpNmList)  return FALSE;

    for (int i=0; i<numNm; i++) {
      internalEventNm << tmpNmList[i];
      
      // Register internal event name into Tcl.
      Tcl_SetVar(myInterp,tmpNmList[i],"0",TCL_GLOBAL_ONLY);
    }

    delete [] tmpNmList;

    return TRUE;  
}

int FSMScheduler::checkStars() {
    int check = 0; // To check if only one initial state exists.
    GalTopBlockIter next(*galaxy());
    Block *bl;
    FSMStateStar *s;
    while ((bl = next++) != 0) {
      if (bl->isA("FSMStateStar")) {
	if (!bl->isA("FSMMixed")) {
	  Error::abortRun("FSMScheduler: ",
			  "Non-supported state star found in your state ",
			  "transition diagram. Should be FSMMixed.");
	  return FALSE;  
	}
	s = (FSMStateStar *)bl;
	if (s->isInit()) {
	  initialState = s;
	  check++;
	}
      } // end of if isA("FSMStateStar")
    }   // end of while ((bl = next++) != 0) 

    if (check > 1) {
      Error::abortRun("FSMScheduler: ",
		      "Can't have more than one initial state.");
      return FALSE;  
    }
    else if (check == 0) {
      Error::abortRun("FSMScheduler: ",
		      "No initial state. At least one is necessary.");
      return FALSE;  
    }

    return TRUE;
}

// Run (or continue) the simulation.
int FSMScheduler::run() {
    if (SimControl::haltRequested() || !galaxy()) {
	Error::abortRun("FSMScheduler has no galaxy to run");
	return FALSE;
    }

    PortHole* p;

    // For input portholes.
    MPHIter nexti(*(myInPorts));
    while ((p = nexti++) != 0) {
      // Get data from Geodesic to PortHole.
      p->receiveData(); 

      // Register the input data into Tcl interpreter.
      // FIXME: Only the first Particle will be registered.
      InfString buf = double((*p)%0);
      Tcl_SetVar(myInterp,(char *)p->name(),(char *)buf,TCL_GLOBAL_ONLY);
    }

    // For output portholes.
    MPHIter nexto(*(myOutPorts));
    while ((p = nexto++) != 0) {
      // Get and initialize enough Particles in PortHole.
      p->receiveData();  
    }

    int condNum;

    // Find the next state by examining the preemptive arcs.
    if (!(nextState = curState->nextState(condNum,1)))  return FALSE;
    if (condNum == -1) {
      // If not found, then execute the internal machine if it exists.
      if (!curState->doInMach(curEntryType)) return FALSE;

      // Find the next state by examining the NOT preemptive arcs.
      if (!(nextState = curState->nextState(condNum,0)))  return FALSE;
      if (condNum == -1) {
	InfString buf = "FSMScheduler: No transition is triggered ";
	buf << "at state \"";
	buf << curState->name();
	buf << "\".";
	Error::abortRun(buf);
	return FALSE;
      }
    }

    // Do the action for the next transition corresponding to "actNum".
    if (!curState->doAction(condNum))      return FALSE;

    // For output portholes.
    nexto.reset();
    while ((p = nexto++) != 0) {
      // Send data to Geodesic from PortHole.
      // Only new data will be sent out.
      p->sendData();
    }

    // For input portholes.
    nexti.reset();
    while ((p = nexti++) != 0) {
      // Clear unused particles in Geodesic.
      p->sendData();
    }

    // Update the entry type of last transition.
    if ( (curEntryType = curState->getEntryType(condNum)) == -1 )
      return FALSE;

    // Set current state to be next state.
    curState = nextState;

    return !SimControl::haltRequested();
}

void FSMScheduler::resetInitState() {
    // Reset current state to be the initial state.
    // This needs to be invoked while hierarchical entry is initial entry. 
    curState = initialState;
    curEntryType = 1; // By "Initial" entry.
}

// isA
ISA_FUNC(FSMScheduler,Scheduler);
