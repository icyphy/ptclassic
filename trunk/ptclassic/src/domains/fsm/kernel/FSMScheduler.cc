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
#include "Galaxy.h"
#include "EventHorizon.h"
#include "Error.h"
#include "InfString.h"
#include "utilities.h"

	//////////////////////////////////////////
	// Methods for FSMScheduler
	//////////////////////////////////////////

FSMScheduler::FSMScheduler() {
    currentTime = 0;

    inputNameMap = NULL;
    outputNameMap = NULL;
    internalNameMap = NULL;
    machineType = NULL;

    myInPorts = NULL;
    myOutPorts = NULL;
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

int FSMScheduler::setupIOPorts() {
    myInPorts = NULL;
    myOutPorts = NULL;

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

    if (myInPorts == NULL) {
      Error::abortRun("FSMScheduler:", " No data input star.");
      return FALSE;  
    } else if (myOutPorts == NULL) {
      Error::abortRun("FSMScheduler:", " No data output star.");
      return FALSE;
    }

    // Set the name of each PortHole in the input/output MultiPortHole.
    if (!setNameMap(*myInPorts,inputNameMap))    return FALSE;
    if (!setNameMap(*myOutPorts,outputNameMap))  return FALSE;

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
    while ( (p = nextp++) != NULL ) {
      p->setName((const char*)parsedName[indx]);
      indx++;
    }

    delete [] parsedName;

    return TRUE;
}

// isA
ISA_FUNC(FSMScheduler,Scheduler);


	//////////////////////////////////////////
	// Methods for BasicScheduler
	//////////////////////////////////////////

// Constructor.
BasicScheduler::BasicScheduler() {
    curState = NULL;
    nextState = NULL;
}

extern int warnIfNotConnected (Galaxy&);

// Initialization.
void BasicScheduler::setup() {
    clearHalt();
    currentTime = 0;

    if (!galaxy()) {
	Error::abortRun("BasicScheduler has no galaxy.");
	return;
    }

    if (warnIfNotConnected(*galaxy())) return;

    // Create the Tcl interpreter for evaluting the conditions in each state.
    // "myInterp" will be passed to each state, so it should be created
    // before galaxy()->initialize()
    if (!(myInterp = Tcl_CreateInterp())) {
      Error::abortRun("BasicScheduler: ", 
		      "Couldn't create a Tcl interpreter!");
      return; 
    }

    // Setup InPorts/OutPorts. These information will be used in each state
    // setup method, so it should be before galaxy()->initialize().
    if (!setupIOPorts()) return;

    galaxy()->initialize();

    if (!checkStars()) return;

    // After checkStars(), myInPorts/myOutPorts are setup.
    // Now get the outer parent.
//    MPHIter nexti(*(myInPorts));
//    PortHole *p = nexti++;
//    EventHorizon * eh = p->far()->asEH();
//    myOuterParent = eh->ghostAsPort()->parent()->parent();
//    if (!myOuterParent) return;
}

int BasicScheduler::checkStars() {
    int check = 0; // To check if only one initial state exists.
    GalTopBlockIter next(*galaxy());
    Block *bl;
    FSMStateStar *s;
    while ((bl = next++) != 0) {
      if (bl->isA("FSMStateStar")) {
	if (!strcmp(machineType,"Moore")) {
	  if (!bl->isA("FSMMoore")) {
	    Error::abortRun("BasicScheduler: ",
			    "Non-supported state star found in your state ",
			    "transition diagram. Should be FSMMoore.");
	    return FALSE;  
	  }
	} else {
	  if (!bl->isA("FSMMealy")) {
	    Error::abortRun("BasicScheduler: ",
			    "Non-supported state star found in your state ",
			    "transition diagram. Should be FSMMealy.");
	    return FALSE;  
	  }
	}
	s = (FSMStateStar *)bl;
	if (s->isInit()) {
	  curState = s;
	  check++;
	}
      } // end of if isA("FSMStateStar")
    }   // end of while ((bl = next++) != 0) 

    if (check > 1) {
      Error::abortRun("BasicScheduler: ",
		      "Can't have more than one initial state.");
      return FALSE;  
    }
    else if (check == 0) {
      Error::abortRun("BasicScheduler: ",
		      "No initial state. At least one is necessary.");
      return FALSE;  
    }

    return TRUE;
}


// Run (or continue) the simulation.
int BasicScheduler::run() {
    if (SimControl::haltRequested() || !galaxy()) {
	Error::abortRun("BasicScheduler has no galaxy to run");
	return FALSE;
    }

    PortHole* p;

    // For input portholes.
    MPHIter nexti(*(myInPorts));
    while ((p = nexti++) != 0) {
      // Get data from Geodesic to PortHole.
      p->receiveData(); 

      //register the input data into Tcl interpreter.
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
    if (!(nextState = curState->nextState(condNum)))  return FALSE;

    if (!strcmp(machineType,"Moore")) {
      // Moore machine. It's the entry action.
      if (!nextState->doAction()) return FALSE;
    }
    else {
      // Mealy machine
      if (condNum == -1) {
	Error::abortRun("BasicScheduler: ",
			"No IMPLICIT reflexive transition is allowed",
			"in Mealy machine.");
	return FALSE;
      } else if (!curState->doAction(condNum)) return FALSE;
    }

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

    // Set current state to be next state.
    curState = nextState;

    return !SimControl::haltRequested();
}

// isA
ISA_FUNC(BasicScheduler,FSMScheduler);
