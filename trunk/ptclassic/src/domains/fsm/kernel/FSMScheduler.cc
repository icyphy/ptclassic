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
    evaluationType = NULL;
    oneWriterType = NULL;
}

// Destructor.
FSMScheduler::~FSMScheduler() {
    delete [] inputNameMap;
    delete [] outputNameMap;
    delete [] internalNameMap;
    delete [] machineType;
    delete [] evaluationType;
    delete [] oneWriterType;
}

// Set the stopping time.
void FSMScheduler::setStopTime(double) {
    // reactive system takes no time to output.
}

// Set the stopping time when inside a Wormhole. */
void FSMScheduler::resetStopTime(double) {
    // reactive system takes no time to output.
}

extern int warnIfNotConnected (Galaxy&);

// Check all stars.
// (1) Check if StateStar has the right type.
// (2) Check if there is only one initial state.
int FSMScheduler::checkStars() {
    int check = 0; // To check if only one initial state exists.
    GalTopBlockIter next(*galaxy());
    Block *bl;
    FSMStateStar *s;
    while ((bl = next++) != 0) {
      if (bl->isA("FSMStateStar")) {
          InfString buf = "FSM";
	  buf << evaluationType;
	  buf << machineType;
	  if ( !bl->isA(buf) ) {
	    Error::abortRun("FSMScheduler: \"", bl->name(),
                            "\" does not belong to this type of machine.");
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


void FSMScheduler::resetInitState() {
    // Reset current state to be the initial state.
    // This needs to be invoked while hierarchical entry is initial entry. 
    curState = initialState;
    curEntryType = 1; // By "Initial" entry.
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

    // Setup inputs, outputs and internal events. These information will
    // be used in each state setup method, so it should be before 
    // galaxy()->initialize().
    if (!setupIO())  return;

    galaxy()->initialize();

    // After galaxy()->initialize(), check all stars.
    if (!checkStars()) return;

    // After checkStars(), myInPorts/myOutPorts are setup.
    // Now get the outer domain.
    if (myInPorts != NULL) {
      MPHIter nexti(*(myInPorts));
      PortHole *p = nexti++;
      outerDomain = p->far()->parent()->domain();
      if (!outerDomain) return;
//printf("Galaxy = %s, outer domain = %s\n",galaxy()->name(),outerDomain);
    }

    // Set current state to be the initial state.
    curState = initialState;
    curEntryType = 1;
}

// Setup I/O mechanism.
int FSMScheduler::setupIO() {

    //---- Setup input/output ports. ----
    myInPorts = NULL;
    myOutPorts = NULL;

    InfString dataInNm = "FSMDataIn";
    InfString dataOutNm = "FSMDataOut";
    if (!strcmp(machineType,"Pure")) {
      dataInNm  << "Int";
      dataOutNm << "Int";
    }
    GalTopBlockIter next(*galaxy());
    Block *bl;
    while ((bl = next++) != 0) {      
	if (bl->isA(dataInNm)) {
	  bl->initialize();
	  myInPorts = bl->multiPortWithName("input");
	}
	else if (bl->isA(dataOutNm)) {
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

    // Setup the name of each PortHole in the input/output MultiPortHole.
    if (!setupNameMap(*myInPorts,inputNameMap))    return FALSE;
    if (!setupNameMap(*myOutPorts,outputNameMap))  return FALSE;

    //---- Setup internal events. ----
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

    //---- Setup all input/ output buffers. ----
    if (!setupIOBuf()) return FALSE;
    
    return TRUE;
}

int FSMScheduler::setupIOBuf() {
    Error::abortRun("FSMScheduler: Input/output buffers depend on the ",
                    "type of the machine. Should be setup in derived class.");
    return FALSE;
}

// Set each PortHole of a MultiPortHole with a name.
int FSMScheduler::setupNameMap(MultiPortHole& mph, const char* Name_Map) {
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
	// Methods for SPureSched
	//////////////////////////////////////////

SPureSched::SPureSched() {
    myAllInBuf = NULL;
    myAllOutBuf = NULL;
}

// Destructor.
SPureSched::~SPureSched() {
    delete [] myAllInBuf;
    delete [] myAllOutBuf;
}

void SPureSched::clearOutput(int value) {
    // Clear output buffers corresponding to the internal events.
    for (int indx=0; indx<internalEventNm.numPieces(); indx++)
      myAllOutBuf[indx] = value;

    // For output portholes.
    PortHole* p;
    MPHIter nexto(*(myOutPorts));
    while ((p = nexto++) != 0) {
      // Get and initialize enough Particles in PortHole.
      p->receiveData();

      // Clear output buffers corresponding to the output ports.
      myAllOutBuf[indx] = value;

      indx++;
    } 
}

void SPureSched::receiveData() {
    // Internal events are fed back from previous iteration.
    for (int indx=0; indx<internalEventNm.numPieces(); indx++)
      myAllInBuf[indx] = myAllOutBuf[indx];

    // Grab data for input portholes.
    PortHole* p;
    MPHIter nexti(*(myInPorts));
    while ((p = nexti++) != 0) {
      // Get input data.
      if (!p->geo()->size()) {
	// There is no token: The only possibility is that there is
	// no event from outer DE domain, then it is considered absent.
	(*p)%0 << 0;

      } else {
	// There exist tokens: Then grab data from Geodesic to PortHole.
	p->receiveData(); 

	// Clear those Particles that is still in Geodesic.
	while (p->geo()->size()) {
	  Particle* pp = p->geo()->get();
	  pp->die();
	}

	if (!strcmp(outerDomain,"DE")) {
	  // If outer domain is DE, it is considered present as long as
	  // there is a token, and we don't care about its value.
	  (*p)%0 << 1;
	}
      }

      // Then put the data into input buffers.
      myAllInBuf[indx] = int((*p)%0);

      indx++;
    }
}

// Run (or continue) the simulation.
int SPureSched::run() {
    if (SimControl::haltRequested() || !galaxy()) {
	Error::abortRun("FSMScheduler has no galaxy to run");
	return FALSE;
    }

    // For highlighting star in graphic debug mode.
    if (!SimControl::doPreActions(curState)) return FALSE;

    // Grab input data and get internal events.
    receiveData();
    // Clear output buffers.
    clearOutput(0);
    
    int condNum;
    // Find the next state by examining the preemptive arcs.
    if (!(nextState = curState->nextState(condNum,1)))  return FALSE;
    if (condNum == -1) {
      // If not found, then execute the slave process if it exists.
      if (!curState->execSlave(curEntryType)) return FALSE;

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
    if (!curState->execAction(condNum))      return FALSE;

    // Send output data.
    sendData();

    // Update the entry type of last transition.
    if ( (curEntryType = curState->getEntryType(condNum)) == -1 )
      return FALSE;

    // Set the next state to be current state for next iteration.
    curState = nextState;

    // For highlighting star in graphic debug mode.
    if (!SimControl::doPreActions(curState)) return FALSE;

    return !SimControl::haltRequested();
}

void SPureSched::sendData() {
    // For output portholes.
    int indx = internalEventNm.numPieces();
    PortHole* p;
    MPHIter nexto(*(myOutPorts));
    while ((p = nexto++) != 0) {
      // Put data from buffers to PortHole.
      (*p)%0 << myAllOutBuf[indx];

      // Send data to Geodesic from PortHole.
      p->sendData();

      indx++;
    }
}

// Setup input/output/internal event buffers.
int SPureSched::setupIOBuf() {
    int numAllInputs = internalEventNm.numPieces()+myInPorts->numberPorts();
    delete [] myAllInBuf;
    myAllInBuf = new int[numAllInputs];
    for (int i=0; i<numAllInputs; i++) {
      myAllInBuf[i] = 0;

      InfString buf = "input(";
      buf << i;
      buf << ")";
      Tcl_LinkVar(myInterp, buf, (char *)&(myAllInBuf[i]),TCL_LINK_INT);
    }

    int numAllOutputs = internalEventNm.numPieces()+myOutPorts->numberPorts();
    delete [] myAllOutBuf;
    myAllOutBuf = new int[numAllOutputs];
    for (i=0; i<numAllOutputs; i++)
      myAllOutBuf[i] = 0;

    return TRUE;
}

// isA
ISA_FUNC(SPureSched,FSMScheduler);
