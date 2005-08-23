static const char file_id[] = "FSMStateStar.cc";
/******************************************************************
Version identification:
@(#)FSMStateStar.cc	1.19 10/08/98

Copyright (c) 1990-1999 The Regents of the University of California.
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

 Programmer:       Bilung Lee
 Date of creation: 3/4/96

 FSMStateStar methods.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "FSMStateStar.h"
#include "FSMScheduler.h"
#include "Domain.h"
#include "Target.h"
#include "Galaxy.h"
#include "Wormhole.h"
#include "KnownBlock.h"
#include "InfString.h"
#include "utilities.h"
#include "ptk.h"


ISA_FUNC(FSMStateStar,FSMStar);

int FSMStateStar::count = 0;

FSMStateStar::FSMStateStar ()
{
    addPort(stateIn.setPort("stateIn",this,FLOAT));
    addPort(stateOut.setPort("stateOut",this,FLOAT));

    addState(isInitState.setState("isInitState",this,"FALSE","Is this an initial state?"));

    addState(events.setState("events",this,"","The events to trigger the next transition state. Each event must be surrounded by a pair of curly-braces."));
    addState(conditions.setState("conditions",this,"","The conditions to determine the next transition state. Each condition must be surrounded by a pair of curly-braces."));
    addState(actEvents.setState("actEvents",this,"","The sets of output events to be emitted for all possible next transitions (outgoing arcs). Each set of output events for one transition must be surrounded by a pair of curly-braces, and each event in that set must be surrounded by a pair of double-quotes"));
    addState(actExprs.setState("actExprs",this,"","The sets of expressions to be evaluated corresponding to the sets of output events to be emitted for all possible next transitions (outgoing arcs). Each set of expressions for one transition must be surrounded by a pair of curly-braces, and each expression in that set must be surrounded by a pair of double-quotes"));

    addState(entryType.setState("entryType",this,"","Specify the entry type for each possible transition out of this state. Available choices are 0 (History) or 1 (Initial). Note that the # of specified types should be equal to the # of outgoing arcs."));
    addState(preemptive.setState("preemptive",this,"","Specify that each possible transition is preemptive or not. Note that the # of YES/NO should be equal to the # of outgoing arcs."));

    addState(slaveNm.setState("slaveNm",this,"","The file name of an Galaxy to be the slave process."));
    addState(where_is_defined.setState("where_is_defined",this,"","The path name of the Galaxy of the slave process."));

    parsedEvents = NULL;
    parsedConditions = NULL;
    parsedActEvents = NULL;
    parsedActExprs = NULL;
}

FSMStateStar::~FSMStateStar() {
    delete [] parsedEvents;
    delete [] parsedConditions;
}

void FSMStateStar::begin() {
    if (slave != NULL) {
      // Run the XXXWormhole::begin(), this eventually will invoke 
      // the begin method of each Star in the Galaxy.
      slave->begin();  
    }
}

Star* FSMStateStar::createWormhole(const char *galname,
				   const char* where_defined) {
    // Compile the specified galaxy into kernel.
    const char* classname = ptkCompile(galname, where_defined);
    if (!classname) return 0;

    // Find the current domain in which the galname is compiled.
    InfString command = "domain";
    Tcl_GlobalEval(ptkInterp, (char*)command);
    const char* currentDomain = hashstring(ptkInterp->result);
    // Should now have a master of the block -- clone it in currentDomain.
    Block *block = KnownBlock::clone(classname, currentDomain);
    if (!block) return 0;

    Galaxy* myGal = NULL;
    Target* myInTarget = NULL;
    if (block->isItWormhole()) {
      // Though it's a Wormhole, the outside domian may not be FSM.
      // Therefore, we explode the Wormhole as an Galaxy, and keep
      // its inner Target.
      // Note : Every inner FSM galaxy will be specified its own Target,
      // so "ptkCompile" will also make a wormhole for it.
      Wormhole* myWorm = (block->asStar()).asWormhole();
      myInTarget = myWorm->myTarget();
      myGal = myWorm->explode();

    } else {
      if (!block->isA("Galaxy")) {
	Error::abortRun(*this,"Can only use galaxies for ",
			"replacement blocks");
	return 0;
      }

      myGal = &(block->asGalaxy());

      // No Target is specified for a Galaxy, then a copy of default
      // Target of the inner domain will automatically be used.
    }

    // If the Galaxy has no name, then name it.
    if (!strcmp(myGal->name(),"")) {
      myGal->setName(classname);
    }

    // To create a wormhole in the FSM domain that will encapsulate 
    // the inner galaxy.
    Domain* od = Domain::named("FSM");
    if (!od) {
      Error::abortRun(*this,"FSM domain does not exist! Help!");
      return NULL;
    }
    Star *newWorm = &(od->newWorm(*myGal,myInTarget));

    // Choose a name for the block
    StringList instancename = classname;
    instancename << count++;
    const char* instance = hashstring(instancename);

    // Is this needed?
    KnownBlock::addEntry(*newWorm, instance, 1, "FSM");

    // Set the parent of the wormhole to be the parent of this star.
    newWorm->setBlock(instance,parent());

    return newWorm;
}

int FSMStateStar::execAction (int actNum) {
    if (actNum != -1) {
      StringListIter nextEvent(parsedActEvents[actNum]);
      StringListIter nextExpr(parsedActExprs[actNum]);
      const char* event;
      const char* expr;
      InfString buf;

      int numEvents = parsedActEvents[actNum].numPieces();
      for (int indx=0; indx<numEvents; indx++) {
        event = nextEvent++;
        expr = nextExpr++;

        // Set status of event to 1 (present).
	buf = event;
	buf << "(s)";
	Tcl_SetVar(myInterp,buf,"1",TCL_GLOBAL_ONLY);
	    
	if (!strcmp(expr,"")) {
	  // When expression is an empty string, set the value of event
	  // to 1. (In order to be able to represent "present" in SDF.
	  buf = event;
	  buf << "(v)";
	  Tcl_SetVar(myInterp,buf,"1",TCL_GLOBAL_ONLY);
	      
	} else {
	  buf = expr;
	  if (Tcl_ExprString(myInterp,buf) != TCL_OK) {
	    Error::abortRun(*this,"Tcl_ExprString failed: ",
                            myInterp->result) ;
	    return FALSE;
	  }
	  buf = event;
	  buf << "(v)";
	  Tcl_SetVar(myInterp,buf,myInterp->result,TCL_GLOBAL_ONLY);
	}

      } // end of for (int indx=0; indx<numEvents; indx++) 
    }   // end of if (actNum != -1)
      
    return TRUE;
}

int FSMStateStar::execSlave (int curEntryType) {
    ///////////////////////////////////////////////////////////////////
    // If the slave process exists, then execute it.
    if (slave != NULL) {

      // Grab inputs from Tcl Interp to PortHoles.
      PortHole* p;
      BlockPortIter next(*slave);
      while ((p = next++) != NULL)
      if (p->isItInput()) {
        if (!interp2port(myInterp, p, slaveInnerDomain)) return FALSE;
      }

      if (curEntryType == 1) {
        // Enter slave process by "Default" (initial state).
        slave->begin();
      }

      // Do the data processing.
      if (!(slave->run())) return FALSE;

      // Send outputs from PortHoles to Tcl Interp.
      next.reset();
      while ((p = next++) != NULL)
        if (p->isItOutput()) {
          if (!port2interp(p, myInterp, slaveInnerDomain)) return FALSE;
        }

    } // end of if (slave != NULL)

    return TRUE;
}

int FSMStateStar::getEntryType (int transNum) {
    ///////////////////////////////////////////////////////////////////
    // Get the entry type of a possible transition out of this state
    // corresponding to the transNum.

    if (transNum>=stateOut.numberPorts() || transNum<0) {
        Error::abortRun(*this,"The specified transition number is out ",
			"of bound.");
	return -1;
    }

    return entryType[transNum];
}

// Find the next state that meets the triggering event and condition
// depending on preemptive or not.
// Return (1) "transNum", # for the transition that is triggered.
// and (2) the corresponding connected state.
// If no transition is triggered, return (1) transNum = -1
// and (2) "this" state.
FSMStateStar * FSMStateStar::nextState (int& transNum, int preemption) {
      InfString buf;
      int* result = new int[numTrans];
      int checkResult = 0;
      int indx; 

      for (indx = 0; indx < numTrans; indx++) {
	if (preemptive[indx] != preemption) {
	  // No need to evaluate transition with unmatched preemptiveness.
	  result[indx] = 0;

	} else {
	  // If the preemptiveness of the indx'th arc is equal to "preemption",
	  // then evaluate the corresponding triggering event and condition. 
	  
	  // Evaluate triggering event.
	  buf = parsedEvents[indx];
	  if ( !strcmp(buf,"") ) {
	    result[indx] = 1;
	  } else if (Tcl_ExprBoolean(myInterp, (char*)buf, &(result[indx]))
		     != TCL_OK) {
	    buf = "Cannot evaluate the triggering event #";
	    buf << indx;
	    buf << " in ";
	    buf << this->name();
	    buf << ". The error message in Tcl : ";
	    buf << myInterp->result;
	    Error::abortRun(*this,(char*)buf);
	    delete [] result;
	    return NULL;
	  }
	  
	  if (result[indx] == 1) {
	    // If evaluation of triggering event is TRUE,
	    // then evaluate triggering condition.
	    buf = parsedConditions[indx];
	    if ( !strcmp(buf,"") ) {
	      result[indx] = 1;
	    } else if (Tcl_ExprBoolean(myInterp, (char*)buf, &(result[indx]))
		       != TCL_OK) {
	      buf = "Cannot evaluate the triggering condition #";
	      buf << indx;
	      buf << " in ";
	      buf << this->name();
	      buf << ". The error message in Tcl : ";
	      buf << myInterp->result;
	      Error::abortRun(*this,(char*)buf);
	      delete [] result;
	      return NULL;
	    }
	    // When both triggering event and condition are TRUE,
	    // the transition is supposed to be triggered.
	    if (result[indx] == 1) checkResult++;
	  }

	} // end of else of if (preemptive[indx] != preemption) 
      }   // end of for (indx = 0; indx < numTrans; indx++)

      if (checkResult == 1) {
	// The next state is found.
	
	// Find the index # corresponding to the next state.
	transNum = 0;
	while (result[transNum] != 1) transNum++;

	// Return the next state.
	MPHIter nextp(stateOut);
	PortHole* oport = 0;
	for (indx=0; indx<=transNum; indx++)  {
	  oport=nextp++;
	}
	
	delete [] result;
	return (FSMStateStar *)(oport->far()->parent());

      } else if (checkResult == 0) {

	// No transition is triggered. Remain current state.
	transNum = -1;
	delete [] result;
	return this;
      }  

      // checkResult > 1, this means "Transition Conflict".
      buf = "Transition conflict: Transitions ";
      for (indx = 0; indx < numTrans; indx++) {
	if (result[indx] == 1) {
	  buf << indx << ", ";
	}
      }
      buf << "are all triggered at this time.";
      Error::abortRun(*this, (char*)buf);
      delete [] result;
      return NULL;
}

const char* FSMStateStar::ptkCompile(const char *galname,
				     const char* where_defined) {
    InfString fullName = expandPathName(where_defined);
    fullName << "/";
    fullName << galname;
   
    InfString command = "file exists ";
    command << fullName;
    Tcl_GlobalEval(ptkInterp, (char*)command);
    if (strcmp(ptkInterp->result,"0") == 0) {
      Error::abortRun(*this,"Specified file name for the block (Galaxy) ",
		      "doesn't exist.");
      return 0;
    }

    command = "file isdirectory ";
    command << fullName;
    Tcl_GlobalEval(ptkInterp, (char*)command);
    if (strcmp(ptkInterp->result,"1") == 0) {
      // Specified file name is a directory, then consider it as a Oct facet.

      // Try compiling the facet specified in the where_defined and 
      // galname argument.
      command = "ptkOpenFacet ";
      command << fullName;
      if(Tcl_GlobalEval(ptkInterp, (char*)command) == TCL_ERROR) {
	Error::abortRun(*this,"Can't open the block (Galaxy) facet.");
	return 0;
      }
      const char* facetHandle = hashstring(ptkInterp->result);

      command = "ptkCompile ";
      command << facetHandle;
      if(Tcl_GlobalEval(ptkInterp, (char*)command) == TCL_ERROR) {
	InfString buf  = "Unable to compile the facet: ";
	buf << where_defined;
	buf << "/";
	buf << galname;
	buf << ". The error message in Tcl : ";
	buf << myInterp->result;
	Error::abortRun(*this,(const char*)buf);
	return 0;
      }

      return galname;

    } else {
      // Specified file name is a file, then consider it as a "std" file
      // and use ptkTychoLoadFSM to compile it.
      command = "ptkTychoLoadFSM ";
      command += fullName;
      if (Tcl_GlobalEval(ptkInterp, command) != TCL_OK) {
	InfString buf  = "Unable to compile the file: ";
	buf << where_defined;
	buf << "/";
	buf << galname;
	buf << ". The error message in Tcl : ";
	buf << myInterp->result;
	Error::abortRun(*this,(const char*)buf);
	return NULL;
      }
      const char* classname = hashstring(ptkInterp->result);

      return classname;
    }
}

void FSMStateStar::setup() {
    if (entryType.size() != stateOut.numberPorts()) {
      Error::abortRun(*this,"The number of specified entry types doesn't ",
		      "match the number of possible next transitions");
      return;
    }     

    if (preemptive.size() != stateOut.numberPorts()) {
      Error::abortRun(*this,"The number of specified preemptive doesn't ",
		      "match the number of possible next transitions");
      return;
    }     

    FSMScheduler *sched = (FSMScheduler *)scheduler();

    // Setup pointers to the copies of its master FSM.
    if ( (myInterp = sched->interp()) == NULL ) {
      Error::abortRun(*this,"FSMScheduler does not have a ",
		      "Tcl interpreter!");
      return; 
    }

    // Parse the events.
    delete [] parsedEvents;
    if (!(parsedEvents = strParser(events, numTrans, "curly-brace")))
      return;
    if (numTrans != stateOut.numberPorts()) {
      Error::abortRun(*this, "The number of specified triggering events ",
		      "doesn't match the number of possible ",
		      "next transitions.");
      return;
    }

    // Parse the conditions.
    delete [] parsedConditions;
    if (!(parsedConditions = strParser(conditions, numTrans, "curly-brace")))
      return;
    if (numTrans != stateOut.numberPorts()) {
      Error::abortRun(*this, "The number of specified triggering conditions ",
		      "doesn't match the number of possible ",
		      "next transitions.");
      return;
    }

    char** strings;
    char** subStrings;
    int numSubStrings;
    int i,j;
    // Parse the action events.
    if (!(strings = strParser(actEvents, numTrans, "curly-brace")))
      return;
    if (numTrans != stateOut.numberPorts()) {
      Error::abortRun(*this, "The number of sets of triggered action ",
		      "events doesn't match the number of possible ",
		      "next transitions.");
      delete [] strings;
      return;
    }
    parsedActEvents = new InfString[numTrans];
    for (i=0; i<numTrans; i++) {
      parsedActEvents[i].initialize();
      if (strcmp(strings[i],"")) {
	// if string[i] is not empty, i.e. action is specified.
	if (!(subStrings = 
	      strParser(strings[i], numSubStrings, "double-quote")))
	  return;
	for (j=0; j<numSubStrings; j++) {
	  parsedActEvents[i] << subStrings[j];
	}
	delete [] subStrings;
      }
    }
    delete [] strings;

    // Parse the action expressions.
    if (!(strings = strParser(actExprs, numTrans, "curly-brace")))
      return;
    if (numTrans != stateOut.numberPorts()) {
      Error::abortRun(*this, "The number of sets of triggered action ",
		      "expressions doesn't match the number of possible ",
		      "next transitions.");
      delete [] strings;
      return;
    }
    parsedActExprs = new InfString[numTrans];
    for (i=0; i<numTrans; i++) {
      parsedActExprs[i].initialize();
      if (strcmp(strings[i],"")) {
	// if string[i] is not empty, i.e. action is specified.
	if (!(subStrings = 
	      strParser(strings[i], numSubStrings, "double-quote")))
	  return;
	for (j=0; j<numSubStrings; j++) {
	  parsedActExprs[i] << subStrings[j];
	}
	delete [] subStrings;
      }
    }
    delete [] strings;

    if (!strcmp(slaveNm,"")) {
      slave = NULL;
    } else {
      // When slaveNm is specified, i.e. != "", 
      // then build up the slave wormhole.
      const char* galname = (const char*)slaveNm;
      const char* where_defined = (const char*)where_is_defined;
      if (!(slave = createWormhole(galname,where_defined)))
	return;

      // Set up Geodesic for the slave wormhole.
      if ( !(setupGeodesic(slave,slave->parent())) )
	return;

      slave->initialize();

      slaveInnerDomain = slave->scheduler()->domain();

    } // end of if (strcmp(slaveNm,""))

} // end of setup()  

int FSMStateStar::setupGeodesic (Star* worm, Block* parent) {
    PortHole* wp;
    PortHole* pp;
    BlockPortIter next(*worm);
    while ((wp = next++) != NULL) {
      // Seems that "portWithName" will automatically find real port in parent.
      pp = parent->portWithName(wp->name());
      if (pp != NULL) {
	// When a port with the same name found in the FSM galaxy,
	// (1) check whether they both are input or output.
	if (pp->isItInput() != wp->isItInput()) {
	  InfString buf = "The porthole with name ";
	  buf << pp->name();
	  buf << " is ";
	  buf << pp->isItInput()?"input":"output";
	  buf << " for FSM galaxy ";
	  buf << parent->name();
	  buf << " but ";
	  buf << wp->isItInput()?"input":"output";
	  buf << " for wormhole in state ";
	  buf << this->name();
	  buf << ".";
	  Error::abortRun(*this,buf);
	  return FALSE;
	}
	// (2) check whether they both have the same data types.
	if ( strcmp(pp->type(), wp->type()) ) {
	  // When they are NOT the same:
	  if (!strcmp(wp->type(),"ANYTYPE")) {
	    // If wp of the Wormhole is ANYTYPE, force it have same
	    // type as pp of the FSM galaxy.
	    wp->setPort(wp->name(), wp->parent(), pp->type());

	  } else {
	    // Otherwise, consider it as an error.
	    InfString buf = "The porthole with name ";
	    buf << pp->name();
	    buf << " is ";
	    buf << pp->type();
	    buf << " for FSM galaxy ";
	    buf << parent->name();
	    buf << " but ";
	    buf << wp->type();
	    buf << " for wormhole in state ";
	    buf << this->name();
	    buf << ".";
	    Error::abortRun(*this,buf);
	    return FALSE;
	  } 
	}
	
      } else {
	// When NO port with the same name found in the FSM galaxy,
	// check whether it is an internal event.
	FSMScheduler *sched = (FSMScheduler *)scheduler();
	StringListIter nextName(sched->intlEventNames);
	const char* name;
	while ((name = nextName++) != NULL) {
	  if (!strcmp(name,wp->name()))    break;
	}
	if (name == NULL) {
	  Error::abortRun(*this, "NO matched I/O port or internal event ",
			  "in FSM galaxy corresponding to the name: ",
			  wp->name());
	  return FALSE;
	}
      }

      // Create a geodesic to receive/send data from/to the wormhole.
      Geodesic* geo = wp->allocateGeodesic();
      if (!geo) {
	Error::abortRun(*this, "Failed to allocate geodesic for ",
			"the porthole named ", wp->name());
	return FALSE;
      }

      if (wp->isItInput()) {
	// Dummy output port used to fool the geodesic into thinking it
	// is fully connected, i.e. dummyPort -- Geodesic --> wp.
	OutFSMPort* dummyPort = new OutFSMPort;
	dummyPort->setPort("dummyOutPort",(Block*)0,wp->type());
	
	geo->setSourcePort(*dummyPort, 0, "");
	geo->setDestPort(*wp);
	geo->initialize();
      }
      else {
	// Dummy input port used to fool the geodesic into thinking it
	// is fully connected, i.e. wp -- Geodesic --> dummyPort.
	InFSMPort* dummyPort = new InFSMPort;
	dummyPort->setPort("dummyInPort",(Block*)0,wp->type());
	
	geo->setSourcePort(*wp,0,"");
	geo->setDestPort(*dummyPort);
	geo->initialize();
      }

    }   // while ((wp = next++) != NULL)

    return TRUE;
}

void FSMStateStar::wrapup() {
    if (slave != NULL)
      slave->wrapup();
}

void FSMStateStar::printInfo() {
    printf("**** In state %s ****\n",this->name());
    if (slave != NULL)    printf("slaveInnerDomain = %s\n",slaveInnerDomain);
    int indx;
    for (indx=0; indx<numTrans; indx++) {
        printf("event[%d] = %s\n",indx, parsedEvents[indx]);
    }
    for (indx=0; indx<numTrans; indx++) {
        printf("condition[%d] = %s\n",indx, parsedConditions[indx]);
    }

    for (indx=0; indx<numTrans; indx++) {
        printf("parsedActEvents[%d] =",indx);
	StringListIter nextEvent(parsedActEvents[indx]);
	const char* event;
	while ((event = nextEvent++) != NULL) {
	  printf(" %s",event);
	}
	printf("\n");

	printf("parsedActExprs[%d] =",indx);
	StringListIter nextExpr(parsedActExprs[indx]);
	const char* expr;
	while ((expr = nextExpr++) != NULL) {
	  if (strcmp(expr,"")) {
	    printf(" %s",expr);
	  } else {
	    printf(" ~");
	  }
	}
	printf("\n");
    }

}
