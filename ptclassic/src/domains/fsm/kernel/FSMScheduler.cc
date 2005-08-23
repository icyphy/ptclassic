static const char file_id[] = "FSMScheduler.cc";

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
#pragma implementation
#endif

#include "FSMScheduler.h"
#include "Galaxy.h"
#include "EventHorizon.h"
#include "Error.h"
#include "InfString.h"
//#include "ptk.h"


extern const char FSMdomainName[];

// Constructor.
FSMScheduler::FSMScheduler() {
    inputNameMap = NULL;
    outputNameMap = NULL;
    machineType = NULL;
    currentTime = 0;

    myInPorts = NULL;
    myOutPorts = NULL;

    curState = NULL;
    nextState = NULL;
}

// Destructor.
FSMScheduler::~FSMScheduler() {
    delete [] inputNameMap;
    delete [] outputNameMap;
    delete [] machineType;
}


// Domain identification.
const char* FSMScheduler::domain() const {
    return FSMdomainName;
}

extern int warnIfNotConnected (Galaxy&);

// Initialization.
void FSMScheduler::setup() {
    clearHalt();
    currentTime = 0;

    if (!galaxy()) {
	Error::abortRun(domain(), " scheduler has no galaxy.");
	return;
    }

    if (warnIfNotConnected(*galaxy())) return;

    // Create the Tcl interpreter for evaluting the conditions in each state.
    // "myInterp" will be passed to each state, so it should be created
    // before galaxy()->initialize()
    if (!(myInterp = Tcl_CreateInterp())) {
      Error::abortRun("Couldn't creat a Tcl interpreter!");
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
//printf("outer parent name : %s\n",myOuterParent->name());
//printf("outer parent domain : %s\n",myOuterParent->domain());

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
//printf("\nmyInPorts address : %x\n",myInPorts);
//printf("input # ports : %d\n",myInPorts->numberPorts());
      } else if (bl->isA("FSMDataOut")) {
	bl->initialize();
	myOutPorts = bl->multiPortWithName("output");
//printf("myOutPorts address : %x\n",myOutPorts);
//printf("output # ports : %d\n",myOutPorts->numberPorts());
      }
    }
//printf("--------------------------------------\n");

    if (myInPorts == NULL) {
      Error::abortRun("No data input star.");
      return FALSE;  
    } else if (myOutPorts == NULL) {
      Error::abortRun("No data output star.");
      return FALSE;
    }

    // Set the name of each PortHole in the input/output MultiPortHole.
    if (!setNameMap(*myInPorts,inputNameMap))   return FALSE;
    if (!setNameMap(*myOutPorts,outputNameMap)) return FALSE;

    return TRUE;
}

int FSMScheduler::setNameMap(MultiPortHole& mph, const char* Name_Map) {
    int numQuote = 0;
    int start = 0;
    int end = 0;

    while (Name_Map[start] != '\0') {
      if (Name_Map[start] == '\"') numQuote++;
      start++;
    }
    if (numQuote%2 != 0) {
      InfString buf = mph.isItInput() ? "input" : "output";
      Error::abortRun("FSMScheduler : Cannot parse the ",buf,
		      "NameMap. Unmatched double-quote.");
      return FALSE;		      
    }

    if (numQuote/2 != mph.numberPorts()) {
      InfString buf = mph.isItInput() ? "input" : "output";
      buf += " names doesn't match the number of PortHole in";
      buf += " the MultiPortHole.";
      Error::abortRun("FSMScheduler : The number of specified ", buf);
      return FALSE;
    }
      
    char* name = 0;
    MPHIter nextp(mph);
    PortHole* p;
    start = 0;
    end = 0;
    while (1) {
      // Skip all the prefix spaces.
      while (Name_Map[start] == ' ') start++;
      
      if (Name_Map[start] == '\0') break ; // Hit end of string.
      
      // Find the left-side quote.
      if (Name_Map[start] != '\"') {
	InfString buf = "FSMScheduler : Cannot parse the ";
	buf += mph.isItInput() ? "input" : "output";
	buf += "NameMap. Each name must be embraced in a pair ";
	buf += "of double-quotes.";
	Error::abortRun(buf);
	return 0;
      }
      start++;
      end = start;
	
      // Find the right-side quote.
      while (Name_Map[end]!='\"') end++;
	
      name = (char *)new char[end-start+1];
      for (int i = 0; i<end-start; i++) {
	name[i] = Name_Map[start+i];
      }
      name[i] = '\0'; 

      p = nextp++;
      p->setName((const char*)name);

      start = end+1;
    }
    return TRUE;
}

int FSMScheduler::checkStars() {
    int check = 0; // To check if only one initial state exists.
    GalTopBlockIter next(*galaxy());
    Block *bl;
    FSMStateStar *s;
    while ((bl = next++) != 0) {
      if (bl->isA("FSMStateStar")) {
	if (!strcmp(machineType,"Moore")) {
	  if (!bl->isA("FSMMoore")) {
	    Error::abortRun("Non-supported state star found in your state ",
			    "transition diagram. Should be FSMMoore.");
	    return FALSE;  
	  }
	} else {
	  if (!bl->isA("FSMMealy")) {
	    Error::abortRun("Non-supported state star found in your state ",
			    "transition diagram. Should be FSMMealy.");
	    return FALSE;  
	  }
	}
	s = (FSMStateStar *)bl;
//s->printOut();	  
	if (s->isInit()) {
	  curState = s;
	  check++;
	}
      } // end of if isA("FSMStateStar")
    }   // end of while ((bl = next++) != 0) 

    if (check > 1) {
      Error::abortRun("Can't have more than one initial state.");
      return FALSE;  
    } else if (check == 0) {
      Error::abortRun("No initial state. At least one is necessary.");
      return FALSE;  
    }

    return TRUE;
}


// Run (or continue) the simulation.
int FSMScheduler::run() {
    if (SimControl::haltRequested() || !galaxy()) {
	Error::abortRun(domain(), " Scheduler has no galaxy to run");
	return FALSE;
    }

    InfString buf;

    PortHole* p;
    // For input portholes.
    MPHIter nexti(*(myInPorts));
    while ((p = nexti++) != 0) {
      // Get data from Geodesic to PortHole.
      p->receiveData(); 
//printf("input name %s with value : %f\n",p->name(),double((*p)%0));

      //register the input data into Tcl interpreter.
      buf = double((*p)%0);
      Tcl_SetVar(myInterp,(char *)p->name(),(char *)buf,TCL_GLOBAL_ONLY);
    }
    // For output portholes.
    MPHIter nexto(*(myOutPorts));
    while ((p = nexto++) != 0) {
      // Get and initialize enough Particles in PortHole.
      p->receiveData();  
    }

//printf("curState : %s\n",curState->name());
    int condNum;
    if (!(nextState = curState->nextState(condNum)))  return FALSE;
//printf("next curState : %s, condNum : %d\n",nextState->name(),condNum);

    if (!strcmp(machineType,"Moore")) {
      // Moore machine. It's the entry action.
      if (!nextState->doAction()) return FALSE;
    } else {
      // Mealy machine
      if (condNum == -1) {
	Error::abortRun("No IMPLICIT reflexive transition is allowed",
			"in Mealy machine.");
	return FALSE;
      } else if (!curState->doAction(condNum)) return FALSE;
    }

    // For output portholes.
    nexto.reset();
    while ((p = nexto++) != 0) {
//printf("output name %s with value : %f\n",p->name(),double((*p)%0));
//printf("---------------------------------\n");
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

// Set the stopping time.
void FSMScheduler::setStopTime(double) {
    // reactive system takes no time to output.
}

// Set the stopping time when inside a Wormhole. */
void FSMScheduler::resetStopTime(double) {
    // reactive system takes no time to output.
}
