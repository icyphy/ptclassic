static const char file_id[] = "FSMStateStar.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

    addState(conditions.setState("conditions",this,"","The conditions to determine the next transition state. Each condition must be surrounded by a pair of double-quotes."));
    addState(actions.setState("actions",this,"","The values/expressions to specify the outputs associated with all possible next transitions (outgoing arcs). Those values/expressions corresponding to one transition must be surrounded by a pair of curly-braces, and each value/expression must be surrounded by a pair of double-quotes."));

    addState(entryType.setState("entryType",this,"","Specify the entry type for each possible transition out of this state. Available choices are 0 (History) or 1 (Initial). Note that the # of specified types should be equal to the # of outgoing arcs."));
    addState(preemptive.setState("preemptive",this,"","Specify that each possible transition is preemptive or not. Note that the # of YES/NO should be equal to the # of outgoing arcs."));

    parsedCond = 0;
    parsedAct = 0;

}

FSMStateStar::~FSMStateStar() {
    delete [] parsedCond;
    delete [] parsedAct;
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

    // Setup the list of internal event names as same as the scheduler.
    internalEventNm = sched->getInternalEventNm();

    if ( (myInterp = sched->interp()) == 0 ) {
      Error::abortRun("FSMScheduler does not have a Tcl interpreter!");
      return; 
    }

    // Parse the conditions.
    delete [] parsedCond;
    if (!(parsedCond = strParser(conditions, numConds, "double-quote")))
      return;
    if (numConds != stateOut.numberPorts()) {
      InfString buf  = "The number of specified conditions ";
      buf << "do not match the number of possible next transition states.";
      Error::abortRun(*this, buf);
      return;
    }

    // Parse the actions.
    int numStrs = 0;
    char** tmpParsedAct = strParser(actions, numStrs, "curly-brace");
    if (!tmpParsedAct) return;
    if (numStrs != stateOut.numberPorts()) {
      delete [] tmpParsedAct;
      Error::abortRun(*this,"The number of action values/expressions in ",
		      "curly-braces doesn't match the number of possible ",
		      "next transitions.");
      return;
    }

    delete [] parsedAct;
    parsedAct = new char**[numStrs];
    for (int indx = 0; indx < stateOut.numberPorts(); indx++) {
      parsedAct[indx] = strParser(tmpParsedAct[indx], numStrs, "double-quote");
      if (!parsedAct[indx]) return;
      if ( numStrs != 
	   (sched->outPorts()->numberPorts()+internalEventNm->numPieces()) ) {
	InfString buf = "The number of action values/expressions ";
	buf << "for transition #";
	buf << indx;
	buf << " doesn't match the number of outputs plus internal events ";
	buf << "in this FSM.";
        delete [] tmpParsedAct;
	Error::abortRun(*this,buf);
	return;
      }
    }

    delete [] tmpParsedAct;
}

// Find the next state that meets the condition depending on preemptive or not.
// Return (1) "condNum", condition #, which is TRUE at this transition,
// and (2) the corresponding connected state.
// If no condition is TRUE, return (1) condNum = -1 and (2) "this" state.
FSMStateStar * FSMStateStar::nextState (int& condNum, int preemption) {
      InfString buf;

      int* result = new int[numConds];
      int checkResult = 0;
      for (int i = 0; i < numConds; i++) {
	if (preemptive[i] == preemption) {
	  // If the preemptiveness of the i'th arc is equal to "preemption",
	  // then evaluate the corresponding condition. 
	  buf = parsedCond[i];
	  if (Tcl_ExprBoolean(myInterp, (char*)buf, &(result[i])) != TCL_OK) {
	    buf = "Cannot evaluate the condition #";
	    buf << i;
	    buf << " in ";
	    buf << this->name();
	    buf << ". The error message in Tcl : ";
	    buf << myInterp->result;
	    Error::abortRun(*this,(char*)buf);
	    delete [] result;
	    return 0;
	  }
	  if (result[i] == 1) checkResult++;

	} else {
	  result[i] = 0;
	}
      }

      if (checkResult == 1) {
	// The next state is found.
	
	// Find the index # corresponding to the next state.
	condNum = 0;
	while (result[condNum] != 1) condNum++;

	// Return the next state.
	MPHIter nextp(stateOut);
	PortHole* oport = 0;
	for (int i=0; i<=condNum; i++)  {
	  oport=nextp++;
	}
	
	delete [] result;
	return (FSMStateStar *)(oport->far()->parent());

      } else if (checkResult == 0) {

	// No condition satisfy. Remain current state.
	// ??Or issue error?
	condNum = -1;
	delete [] result;
	return this;
      }  

      // checkResult > 1, this means "Transition Conflict".
      buf = "Transition conflict. Conditions ";
      for (i = 0; i < numConds; i++) {
	if (result[i] == 1) {
	  buf << i << ", ";
	}
      }
      buf << "are all satisfied at the same time.";
      Error::abortRun(*this, (char*)buf);
      delete [] result;
      return 0;
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

    Galaxy* myGal = 0;
    Target* myInTarget = 0;
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

    // To create a wormhole in the FSM domain that will encapsulate 
    // the inner galaxy.
    Domain* od = Domain::named("FSM");
    if (!od) {
      Error::abortRun(*this,"FSM domain does not exist! Help!");
      return 0;
    }
    Star *newWorm = &(od->newWorm(*myGal,myInTarget));

    // Choose a name for the block
    StringList instancename = "FSM_";
    instancename << galname;
    instancename << count++;
    const char* instance = hashstring(instancename);

    // Is this needed?
    KnownBlock::addEntry(*newWorm, instance, 1, "FSM");

    // Set the parent of the wormhole to be the parent of this star.
    newWorm->setBlock(instance,parent());

    return newWorm;
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
	return 0;
      }
      const char* classname = hashstring(ptkInterp->result);

      return classname;
    }
}

int FSMStateStar::ioNmLists(StringList& inNmList, StringList& outNmList, Star* worm) {
    // Find the input/output port names of the worm, 
    // and put them into inNmList/outNmList.
    
    // Check the PortHoles in the worm.
    BlockPortIter nextp(*worm);
    PortHole* p;
    while ((p = nextp++) != 0) {
      if (p->isItInput())  inNmList << p->name();
      else  outNmList << p->name();
    }

     // Check the MultiPortHoles in the worm.
    BlockMPHIter nextmp(*worm);
    MultiPortHole* mp;
    while ((mp = nextmp++) != 0) {
      if (mp->isItInput())   inNmList << mp->name();
      else  outNmList << mp->name();
    }

    return TRUE;
}

Geodesic** FSMStateStar::setupGeodesic (PortList& pList, MultiPortHole* mph, Star* worm, StringList& pNmList) {
      // Set up the Geodesics for the portholes of the worm with the names 
      // in the pNmList. In addition, it collects all the corresponding 
      // portholes in mph and puts them into pList.
      
      Geodesic** myGeoArray = new Geodesic*[pNmList.numPieces()];

      int index = 0;
      StringListIter nextpNm(pNmList);
      const char* pNm;
      MPHIter nextp(*mph);
      PortHole* p;
      StringListIter nextiNm(*internalEventNm);
      const char* iNm;
      while ((pNm = nextpNm++) != 0) {
	nextp.reset();
	while ((p = nextp++) != 0) {
	  if (!strcmp(p->name(),pNm))  break;
	}
	if (p != 0) {
	  // The name is found as an PortHole in outer MultiPortHole.
	  pList.put(*p);

	} else {
	  // Try to find the name in the list of internal events.
	  nextiNm.reset();
	  while ((iNm = nextiNm++) != 0) {
	    if (!strcmp(iNm,pNm))  break;
	  }
	  if (iNm == 0) {
	    Error::abortRun(*this, "There does not exist a matched I/O port ",
		  "or internal event in this FSM corresponding to the name: ",
		  pNm);
	    delete [] myGeoArray;
	    return 0;
	  }
	}

        PortHole* wp = worm->portWithName(pNm);
	if (!wp) {
	  Error::abortRun(*this, "There is no PortHole named \"",pNm,
			  "\" in the wormhole.");
	  return 0;
	}
	// Create a geodesic to send data to the wormhole.
	Geodesic* geo = wp->allocateGeodesic();
	if (!geo) {
	  Error::abortRun(*this, "Failed to allocate geodesic for ",
			  "the porthole named ", pNm);
	  delete [] myGeoArray;
	  return 0;
	}

	if (mph->isItInput()) {
	  // Dummy output port used to fool the geodesic into thinking it
	  // is fully connected, i.e. dummyPort -- Geodesic --> wp.
	  OutFSMPort* dummyPort = new OutFSMPort;
	  dummyPort->setPort("dummyOutPort",(Block*)0,FLOAT);

	  geo->setSourcePort(*dummyPort, 0, "");
	  geo->setDestPort(*wp);
	  geo->initialize();
	}
	else {
	  // Dummy input port used to fool the geodesic into thinking it
	  // is fully connected, i.e. wp -- Geodesic --> dummyPort.
	  InFSMPort* dummyPort = new InFSMPort;
	  dummyPort->setPort("dummyInPort",(Block*)0,FLOAT);

	  geo->setSourcePort(*wp,0,"");
	  geo->setDestPort(*dummyPort);
	  geo->initialize();
	}
	
	myGeoArray[index] = geo;
	index++;
      }   // end of while ((pNm = nextpNm++) != 0) 

      return myGeoArray;
}

int FSMStateStar::doAction (int actNum) {
      ///////////////////////////////////////////////////////////////////
      // Do the action for the next transition corresponding to "actNum".

      FSMScheduler *sched = (FSMScheduler *)scheduler();

      // Evaluate the actions for outer output portholes and send the outputs.
      OutFSMPort* p;
      MPHIter next(*(sched->outPorts()));
      int index = 0;
      double result = 0;
      while ((p = (OutFSMPort*)next++) != 0) {
	if (strcmp(parsedAct[actNum][index],"-")) {
	  // If the action is NOT "-", then it's a value/expression.
	  if(Tcl_ExprDouble(myInterp, parsedAct[actNum][index], &result) 
	     != TCL_OK) {
	    Error::abortRun(*this,"Cannot evaluate the value. ",
			    "The error message in Tcl : ",
			    myInterp->result);
	    return FALSE;
	  }
	  // Send the result to outer output porthole.
	  p->put() << result;

	  // In addition, set the result into Tcl.
	  InfString buf = result;
	  Tcl_SetVar(myInterp,(char*)p->name(),buf,TCL_GLOBAL_ONLY);
	} // end of if (strcmp(parsedAct[actNum][index],"-")) 

	index++;
      }   // end of while ((p = (OutFSMPort*)next++) != 0)

      // Evaluate the actions for internal events and set the values.
      // NOTE: index # for the actions of internal events continues
      // after the actions of outer output portholes.
      StringListIter nextiNm(*internalEventNm);
      const char* iNm;
      while ((iNm = nextiNm++) != 0) {
	if (strcmp(parsedAct[actNum][index],"-")) {
	  // If the action is NOT "-", then it's a value/expression.
	  if(Tcl_ExprDouble(myInterp, parsedAct[actNum][index], &result) 
	     != TCL_OK) {
	    Error::abortRun(*this,"Cannot evaluate the value. ",
			    "The error message in Tcl : ",
			    myInterp->result);
	    return FALSE;
	  }
	  // Set the result into Tcl.
	  InfString buf = result;
	  Tcl_SetVar(myInterp,(char *)iNm,buf,TCL_GLOBAL_ONLY);
	}

	index++;
      }

      return TRUE;
}

int FSMStateStar::doInMach (int) {
    Error::abortRun(*this, "There is no internal machine in this type ",
		    "of machine.");
    return FALSE;
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
