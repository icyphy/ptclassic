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

    addState(conditions.setState("conditions",this,"","The conditions to determine the next transition state. Each condition must be surrounded by a pair of dcurly-braces."));
    addState(actions.setState("actions",this,"","The actions to specify the indices of outputs for all possible next transitions (outgoing arcs). Each set of output indices for one transition must be surrounded by a pair of curly-braces."));

    addState(entryType.setState("entryType",this,"","Specify the entry type for each possible transition out of this state. Available choices are 0 (History) or 1 (Initial). Note that the # of specified types should be equal to the # of outgoing arcs."));
    addState(preemptive.setState("preemptive",this,"","Specify that each possible transition is preemptive or not. Note that the # of YES/NO should be equal to the # of outgoing arcs."));

    addState(slaveNm.setState("slaveNm",this,"","The file name of an Galaxy to be the slave process."));
    addState(where_is_defined.setState("where_is_defined",this,"","The path name of the Galaxy of the slave process."));

    parsedCond = NULL;
    parsedAct = NULL;
    slave = NULL;
}

FSMStateStar::~FSMStateStar() {
    delete [] parsedCond;
    delete [] parsedAct;
    delete slave;
}

void FSMStateStar::begin() {
    if (slave != NULL) {
      // Run the XXXWormhole::begin(), this eventually will invoke 
      // the begin method of each Star in the Galaxy.
      slave->worm->begin();  
    }
}

int FSMStateStar::compOneWriterCk() {
    for (int i=0; i<numConds; i++)
        if (!preemptive[i])
        for (int j=0; j<parsedAct[i].size(); j++)
	    for (int k=0; k<slave->outNmList.numPieces(); k++)
		if (parsedAct[i][j] == slave->outGeoIndx[k]) {
		    InfString buf = "The output \"";
		    StringListIter nextNm(slave->outNmList);
		    const char* str = NULL;
		    for (int l=0; l<=k; l++)   str = nextNm++;
		    buf << str;
		    buf << "\" in the action of the non-preemptive ";
		    buf << "transtion from state \"";
		    buf << this->name();
		    buf << "\" to state \"";
		    MPHIter nextp(stateOut);
		    PortHole* p = NULL;
		    for (l=0; l<=i; l++)   p = nextp++;
		    str = p->far()->parent()->name();
		    buf << str;
		    buf << "\" conflicts with the output of ";
		    buf << "the internal machine.";
		    Error::abortRun(*this,buf);
		    return FALSE;
		}

    return TRUE;
}  // end of compOneWriterCk()

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

int FSMStateStar::execAction (int) {
    Error::abortRun(*this, "Method \"execAction\" should be redefined ",
		    "in derived class.");
    return FALSE;
}

int FSMStateStar::execSlave (int) {
    Error::abortRun(*this, "Method \"execSlave\" should be redefined ",
		    "in derived class.");
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

// Find the next state that meets the condition depending
// on preemptive or not.
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
	  if(Tcl_ExprBoolean(myInterp, (char*)buf, &(result[i])) != TCL_OK) {
	    buf = "Cannot evaluate the condition #";
	    buf << i;
	    buf << " in ";
	    buf << this->name();
	    buf << ". The error message in Tcl : ";
	    buf << myInterp->result;
	    Error::abortRun(*this,(char*)buf);
	    delete [] result;
	    return NULL;
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

    // Setup some pointers to the copies of its master FSM.
    oneWriterType = sched->oneWriterType;
    internalEventNm = sched->getInternalEventNm();
    if ( (myInterp = sched->interp()) == NULL ) {
      Error::abortRun(*this,"Master FSMScheduler does not have a ",
		      "Tcl interpreter!");
      return; 
    }

    // Parse the conditions.
    delete [] parsedCond;
    if (!(parsedCond = strParser(conditions, numConds, "curly-brace")))
      return;
    if (numConds != stateOut.numberPorts()) {
      Error::abortRun(*this, "The number of specified conditions ",
		      "doesn't match the number of possible ",
		      "next transitions.");
      return;
    }

    // Parse the actions.
    int numStrs = 0;
    char** tmpParsedAct = strParser(actions,numStrs,"curly-brace");
    if (!tmpParsedAct) return;
    if (numStrs != stateOut.numberPorts()) {
      Error::abortRun(*this,"The number of specified actions ",
		      "doesn't match the number of possible ",
		      "next transitions.");
      return;
    }

    delete [] parsedAct;
    parsedAct = new IntArrayState[numStrs];
    for (int indx=0; indx<numStrs; indx++) {
      if (!strcmp(tmpParsedAct[indx],"-")) {
	// No output for this transition.
	parsedAct[indx].setState("actOfTrans",this,"");
	parsedAct[indx].initialize();
      } else {
	parsedAct[indx].setState("actOfTrans",this,tmpParsedAct[indx]);
	parsedAct[indx].initialize();
      }
    }
    delete [] tmpParsedAct;

    if (!strcmp(slaveNm,"")) {
      delete slave;
      slave = NULL;
    } else {
      // When slaveNm is specified, i.e. != "", 
      // then build up the slave process.
      delete slave;
      slave = new SlaveProcess;

      const char* galname = (const char*)slaveNm;
      const char* where_defined = (const char*)where_is_defined;
      if (!(slave->worm = createWormhole(galname,where_defined))) {
	return;
      }

      if ( !ioNmLists(slave->inNmList, slave->outNmList, slave->worm) )
	return;

      slave->inGeoIndx = new int[slave->inNmList.numPieces()];
      if ( !(slave->inGeo =
	     setupGeodesic(slave->inGeoIndx, sched->inPorts(),
			   slave->worm, slave->inNmList)) )
	return;

      slave->outGeoIndx = new int[slave->outNmList.numPieces()];
      if ( !(slave->outGeo = 
	     setupGeodesic(slave->outGeoIndx, sched->outPorts(),
			   slave->worm, slave->outNmList)) )
	return;

      slave->worm->initialize();

      // After the slave process is setup, if oneWriterType of the master FSM
      // is "Compile", then check the one-writer rule here.
      if (!strcmp(oneWriterType,"Compile")) {
	if (!compOneWriterCk()) return;
      }

    } // end of if (strcmp(slaveNm,""))

} // end of setup()  

Geodesic** FSMStateStar::setupGeodesic (int* indxArray, MultiPortHole* mph, Star* worm, StringList& pNmList) {
      // Input: MultiPortHole* mph, Star* worm, StringList& pNmList.
      // Output: Geodesic**, int* indxArray.
      // Set up the Geodesics for the portholes of the worm with the names 
      // in the pNmList. In addition, it collects all the corresponding 
      // input/output index # in (mph + internal events) and puts them 
      // into indxArray.
      
      Geodesic** myGeoArray = new Geodesic*[pNmList.numPieces()];

      int indxOfIO = 0;
      int indxOfArray = 0;
      StringListIter nextpNm(pNmList);
      const char* pNm;
      MPHIter nextp(*mph);
      PortHole* p;
      StringListIter nextiNm(*internalEventNm);
      const char* iNm;
      while ((pNm = nextpNm++) != 0) {
	// Note: I/O buffers starts with internal event first.
	indxOfIO = internalEventNm->numPieces();

	nextp.reset();
	while ((p = nextp++) != 0) {
	  if (!strcmp(p->name(),pNm))  break;
	  indxOfIO++;
	}
	if (p != 0) {
	  // The name is found as an PortHole in outer MultiPortHole.
	  indxArray[indxOfArray] = indxOfIO;

	} else {
	  // Try to find the name in the list of internal events.
	  indxOfIO = 0;
	  nextiNm.reset();
	  while ((iNm = nextiNm++) != 0) {
	    if (!strcmp(iNm,pNm))  break;
	    indxOfIO++;
	  }
	  if (iNm != 0) {
	    // The name is found in the internal event list.
	    indxArray[indxOfArray] = indxOfIO;
	  } else {
	    Error::abortRun(*this, "There does not exist a matched I/O port ",
		  "or internal event in this FSM corresponding to the name: ",
		  pNm);
	    delete [] myGeoArray;
	    return NULL;
	  }
	}

        PortHole* wp = worm->portWithName(pNm);
	if (!wp) {
	  Error::abortRun(*this, "There is no PortHole named \"",pNm,
			  "\" in the wormhole.");
	  return NULL;
	}
	// Create a geodesic to send data to the wormhole.
	Geodesic* geo = wp->allocateGeodesic();
	if (!geo) {
	  Error::abortRun(*this, "Failed to allocate geodesic for ",
			  "the porthole named ", pNm);
	  delete [] myGeoArray;
	  return NULL;
	}

	if (mph->isItInput()) {
	  // Dummy output port used to fool the geodesic into thinking it
	  // is fully connected, i.e. dummyPort -- Geodesic --> wp.
	  OutFSMPort* dummyPort = new OutFSMPort;
	  dummyPort->setPort("dummyOutPort",(Block*)0,mph->type());

	  geo->setSourcePort(*dummyPort, 0, "");
	  geo->setDestPort(*wp);
	  geo->initialize();
	}
	else {
	  // Dummy input port used to fool the geodesic into thinking it
	  // is fully connected, i.e. wp -- Geodesic --> dummyPort.
	  InFSMPort* dummyPort = new InFSMPort;
	  dummyPort->setPort("dummyInPort",(Block*)0,mph->type());

	  geo->setSourcePort(*wp,0,"");
	  geo->setDestPort(*dummyPort);
	  geo->initialize();
	}
	
	myGeoArray[indxOfArray] = geo;
	indxOfArray++;
      }   // end of while ((pNm = nextpNm++) != 0) 

      return myGeoArray;
}

void FSMStateStar::wrapup() {
    if (slave != NULL)
      slave->worm->wrapup();
}
