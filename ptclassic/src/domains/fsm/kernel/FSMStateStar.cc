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

    parsedCond = 0;
}

FSMStateStar::~FSMStateStar() {
    delete [] parsedCond;
}

void FSMStateStar::setup() {
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

    if (!(myInterp = ((FSMScheduler *)scheduler())->interp())) {
      Error::abortRun("FSMScheduler does not have a Tcl interpreter!");
      return; 
    }
}

// Find the next state which meets the condition.
// Return (1) "condNum", condition #, which is TRUE at this transition,
// and (2) the corresponding connected state.
// If no condition is TRUE, return (1) condNum = -1 and (2) "this" state.
FSMStateStar * FSMStateStar::nextState (int& condNum, int) {
    InfString buf;

    int* result = new int[numConds];
    int checkResult = 0;
    for (int i = 0; i < numConds; i++) {
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
	return 0;
      }
      if (result[i] == 1) checkResult++;
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

    // To create a wormhole in the FSM domain that will encapsulate 
    // the inner galaxy.
    Domain* od = Domain::named("FSM");
    if (!od) {
      Error::abortRun(*this,"FSM domain does not exist! Help!");
      return NULL;
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
	return NULL;
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

    if (pNmList.numPieces() != mph->numberPorts()) {
      InfString buf = "The number of ";
      buf << mph->isItInput() ? "input" : "output";
      buf << " PortHoles in the Wormhole \"";
      buf << worm->name();
      buf << "\" does not match the number in this FSM.";
      Error::abortRun(*this,buf);
      return NULL;
    }

    Geodesic** myGeoArray = new (Geodesic*) [pNmList.numPieces()];

    int index = 0;
    StringListIter nextNm(pNmList);
    const char* pNm;
    while ((pNm = nextNm++) != 0) {
	MPHIter nextp(*mph);
	PortHole* p;
	while ((p = nextp++) != 0) {
	  if (!strcmp(p->name(),pNm))  break;
	}
	if (p == 0) {
	  Error::abortRun(*this, "There does not exist a matched Porthole in ",
			  "this FSM corresponding to the name: ", pNm);
	  delete [] myGeoArray;
	  return NULL;
	}
	pList.put(*p);

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

	// Dummy output port used to fool the geodesic into thinking it
	// is fully connected.
	PortHole* dummyPort = new PortHole;

	// Set the desination port for the geodesic.
	// We also have to set a source port, although we don't use it.
	dummyPort->setPort("dummyPort",(Block*)0,FLOAT);
	if (mph->isItInput()) {
	  geo->setSourcePort(*dummyPort, 0, "");
	  geo->setDestPort(*wp);
	  geo->initialize();
	}
	else {
	  geo->setSourcePort(*wp,0,"");
	  geo->setDestPort(*dummyPort);
	  geo->initialize();
	}
	
	myGeoArray[index] = geo;
	index++;
    }   // end of while ((pNm = nextNm++) != 0) 

    return myGeoArray;
}

int FSMStateStar::doAction (int) {
    Error::abortRun(*this, "doAction() is virtual and needs to be ",
		    "redefined in derived class.");
    return FALSE;
}

int FSMStateStar::doInMach (int) {
    Error::abortRun(*this, "There is no internal machine in this type ",
		    "of machine.");
    return FALSE;
}

int FSMStateStar::getEntryType (int) {
    Error::abortRun(*this, "There is no entry type for each transition ",
		    "in this type of machine.");
    return FALSE;
}

void FSMStateStar::printOut () {
    int indx;
    printf("state with name : %s\n",this->name());
    printf("numConds = %d\n",numConds);
    for (indx = 0; indx < numConds; indx++) { 
      printf("condition %d : %s\n", indx, parsedCond[indx]);
    }
}
