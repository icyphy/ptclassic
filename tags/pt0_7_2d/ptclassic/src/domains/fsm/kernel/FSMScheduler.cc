static const char file_id[] = "FSMScheduler.cc";

/*
Version @(#)FSMScheduler.cc	1.13 10/26/98

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
}

// Destructor.
FSMScheduler::~FSMScheduler() {
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
// (1) Check if there is only one initial state.
int FSMScheduler::checkStars() {
    int check = 0; // To check if only one initial state exists.
    GalTopBlockIter next(*galaxy());
    Block *bl;
    FSMStateStar *s;
    while ((bl = next++) != 0) {
      if (bl->isA("FSMStateStar")) {
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

int FSMScheduler::receiveData() {
    PortHole* p;
    BlockPortIter next(*galaxy());
    while ((p = next++) != NULL)
      if (p->isItInput()) {
	// For input port, update the data in Tcl interp.

	// Set p point to its real port.
	p = (PortHole *)p->alias();

	// Register new input from PortHole to Tcl Interp.
	if (!port2interp(p, myInterp, outerDomain)) return FALSE;

      } else {
	// For output port, clear the data in Tcl interp.
	
	// Set p point to its real port.
	p = (PortHole *)p->alias();

	// Set both status and value to 0 in Tcl interp.
	InfString buf = p->name();
	buf << "(s)";
	Tcl_SetVar(myInterp,buf,"0",TCL_GLOBAL_ONLY);
	buf = p->name();
	buf << "(v)";
	Tcl_SetVar(myInterp,buf,"0",TCL_GLOBAL_ONLY);
      }
        
    return TRUE;
}

void FSMScheduler::resetInitState() {
    // Reset current state to be the initial state.
    // This needs to be invoked while hierarchical entry is initial entry. 
    curState = initialState;
    curEntryType = 1; // By "Initial" entry.
}

// Run (or continue) the simulation.
int FSMScheduler::run() {
    if (SimControl::haltRequested() || !galaxy()) {
	Error::abortRun("FSMScheduler has no galaxy to run");
	return FALSE;
    }

    // For highlighting star in graphic debug mode.
    if (!SimControl::doPreActions(curState)) return FALSE;

    // Grab input data and get internal events.
    if (!receiveData()) return FALSE;

    int transNum;
    // Find the next state by examining the preemptive arcs.
    if (!(nextState = curState->nextState(transNum,1)))    return FALSE;
    if (transNum == -1) {
      // If not found, then execute the slave process if it exists.
      if (!curState->execSlave(curEntryType))    return FALSE;

      // Find the next state by examining the NOT preemptive arcs.
      if (!(nextState = curState->nextState(transNum,0)))  return FALSE;
    }
    // Do the action for the next transition corresponding to "transNum".
    if (!curState->execAction(transNum))    return FALSE;

    // Send output data.
    if (!sendData())    return FALSE;

    if (transNum == -1) {
      // If implicit transition is triggered,
      // entry type would be always 0 (history entry).
      curEntryType = 0;
    } else {
      // If explicit transition is triggered,
      // update the entry type to depend on that transition.
      if ((curEntryType = curState->getEntryType(transNum)) == -1)
	return FALSE;
    }

    // Set the next state to be current state for next iteration.
    curState = nextState;

    // For highlighting star in graphic debug mode.
    if (!SimControl::doPreActions(curState)) return FALSE;

    return !SimControl::haltRequested();
}

int FSMScheduler::sendData() {
    PortHole* p;
    BlockPortIter next(*galaxy());
    while ((p = next++) != NULL)
      if (p->isItOutput()) {
	// Set p point to its real port.
	p = (PortHole *)p->alias();

	// Send output from Tcl Interp to PortHole.
	if (!interp2port(myInterp, p, outerDomain)) return FALSE; 

      } // end of if (p->isItOutput())

  return TRUE;
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

    // "myInterp" will be passed to each state, so it should be created
    // before galaxy()->initialize()
    if (!setupTclInterp()) return;

    galaxy()->initialize();

    // After galaxy()->initialize(), check all stars.
    if (!checkStars()) return;

    // Set current state to be the initial state.
    curState = initialState;
    curEntryType = 1;

    // Set the outerDomain.
    // galaxy() points to FSM galaxy;
    // galaxy()->parent() points to Wormhole containning FSM galaxy.
    // galaxy()->parent()->parent() points to Galaxy containing Wormhole.
    outerDomain = galaxy()->parent()->parent()->domain();
    if (!outerDomain) return;
// printf("Galaxy = %s, outer domain = %s\n",galaxy()->name(),outerDomain);
}

// (1) Create a Tcl interpreter 
// (2) Register inputs, outputs, internal events in the Tcl interp
int FSMScheduler::setupTclInterp() {
    // Create the Tcl interpreter for evaluting the conditions in each state.
    if (!(myInterp = Tcl_CreateInterp())) {
      Error::abortRun("FSMScheduler:"," Couldn't create a Tcl interpreter!");
      return FALSE; 
    }

    // Register inputs and outputs in the Tcl interp
    PortHole* p;
    BlockPortIter next(*galaxy());
    while ((p = next++) != NULL) {
      // First, set real port's name to be consistent with galaxy port's name.
      p->alias()->setName(p->name());

      // Register inputs and outputs in the Tcl interp.
      // For each PortHole name, we create two variable,
      // "name(s)" and "name(v)".
      // "name(s)"/"name(v)" is used to keep the status/value respectively. 
      InfString buf = p->name();
      buf << "(s)";
      Tcl_SetVar(myInterp,buf,"0",TCL_GLOBAL_ONLY);
      buf = p->name();
      buf << "(v)";
      Tcl_SetVar(myInterp,buf,"0",TCL_GLOBAL_ONLY);
    }

    // Register internal events in the Tcl interp
    StringListIter nextName(intlEventNames);
    const char* name;
    while ((name = nextName++) != NULL) {
      InfString buf = name;
      buf << "(s)";
      Tcl_SetVar(myInterp,buf,"0",TCL_GLOBAL_ONLY);
      buf = name;
      buf << "(v)";
      Tcl_SetVar(myInterp,buf,"0",TCL_GLOBAL_ONLY);
    }

    return TRUE;
}

void FSMScheduler::printTclVar() {
    PortHole* p;
    BlockPortIter next(*galaxy());
    while ((p = next++) != NULL) {
      p = (PortHole *)p->alias();
      if (p->isItInput()) {
	printf("input port name = %s\n",p->name());
      } else {
	printf("output port name = %s\n",p->name());
      }

      InfString buf = p->name();
      buf << "(s)";
      InfString statusStr = Tcl_GetVar(myInterp,buf,TCL_GLOBAL_ONLY);
      buf = p->name();
      buf << "(v)";
      InfString valueStr = Tcl_GetVar(myInterp,buf,TCL_GLOBAL_ONLY);
      printf("statusStr = %s, valueStr = %s\n",
	     (char*)statusStr,(char*)valueStr);
    }
}

// isA
ISA_FUNC(FSMScheduler,Scheduler);





