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
    if (!(parsedCond = strParser((const char*)conditions,numConds)))
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
FSMStateStar * FSMStateStar::nextState (int& condNum) {
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
	buf << i+1 << ", ";
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
    KnownBlock::addEntry(*newWorm, instance, 1);

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
	Error::abortRun(*this,(const char*)buf);
	return NULL;
      }
      const char* classname = hashstring(ptkInterp->result);

      return classname;
    }
}

Geodesic** FSMStateStar::setupGeodesic (Star* worm, MultiPortHole& mph) {
    Geodesic** myGeoArray = (Geodesic**) new (Geodesic*) [mph.numberPorts()];

    MPHIter nextp(mph);
    PortHole* p;
    int index = 0;
    while ((p = nextp++) != 0) {
      PortHole* ph = worm->portWithName(p->name());
      if (!ph) {
	Error::abortRun(*this, "Cannot find porthole named ", p->name(),
			" in the replacement block.");
	delete [] myGeoArray;
	return 0;
      }

      // Create a geodesic to send data to the wormhole.
      Geodesic* geo = ph->allocateGeodesic();
      if (!geo) {
	Error::abortRun(*this, "Failed to allocate geodesic for ",
			"the porthole named ", ph->name());
	delete [] myGeoArray;
	return 0;
      }

      // Dummy output port used to fool the geodesic into thinking it
      // is fully connected.
      PortHole* dummyPort = new PortHole;

      // Set the desination port for the geodesic.
      // We also have to set a source port, although we don't use it.
      dummyPort->setPort("dummyPort",(Block*)0,FLOAT);
      if (mph.isItInput()) {
	geo->setSourcePort(*dummyPort, 0, "");
	geo->setDestPort(*ph);
	geo->initialize();
      }
      else {
	geo->setSourcePort(*ph,0,"");
	geo->setDestPort(*dummyPort);
	geo->initialize();
      }

      myGeoArray[index] = geo;
      index++;
    }

    return myGeoArray;
}

char** FSMStateStar::strParser(const char* strings,int& numStr) {
    char** parsedStr = 0;
    int numQuote = 0;
    int start = 0;
    int end = 0;

    while (strings[start] != '\0') {
      if (strings[start] == '\"') numQuote++;
      start++;
    }
    if (numQuote%2 != 0) {
      Error::abortRun(*this, "Cannot parse the strings. ",
		      "Unmatched double-quote.");
      return 0;	      
    }
    parsedStr = (char **) new (char *)[numQuote/2];
    
    start = 0;
    end = 0;
    numStr = 0;
    while (1) {
      // Skip all the prefix spaces.
      while (strings[start] == ' ') start++;
      
      if (strings[start] == '\0') break ; // Hit end of string.
      
      // Find the left-side quote.
      if (strings[start] != '\"') {
	Error::abortRun(*this, "Cannot parse the strings. ",
		"Each string must be surrounded by a pair of double-quotes.");
	return 0;
      }
      start++;
      end = start;
      
      // Find the right-side quote.
      while (strings[end]!='\"') end++;
	
      parsedStr[numStr] = (char *) new char[end-start+1];
      for (int i = 0; i<end-start; i++) {
	parsedStr[numStr][i] = strings[start+i];
      }
      parsedStr[numStr][i] = '\0'; 
	
      numStr++;
	
      start = end+1;
    }
    return parsedStr;
}

double* FSMStateStar::str2values(const char* string,int& numValues) {
    int start = 0;
    int index;

    numValues = 0;
    while (1) {
      // Skip all the prefix spaces.
      while (string[start] == ' ') start++;

      if (string[start] == '\0') break ; // Hit end of string.
      else numValues++;

      while (string[start] != ' ' && string[start] != '\0') start++;
    }
    double* values = new double[numValues];

    start = 0;
    for (index=0; index<numValues; index++) {
      while (string[start] == ' ') start++;

      InfString buf ="";
      while (string[start] != ' ' && string[start] != '\0') {
	buf << string[start];
	start++;
      }

      if (Tcl_ExprDouble(myInterp, (char*)buf, &(values[index])) != TCL_OK) {
	Error::abortRun(*this,"Cannot convert the string into a float array.");
	delete [] values;
	return 0;
      }
    }

    return values;
}

int FSMStateStar::doAction (int) {
    Error::abortRun(*this, "doAction() is virtual and needs to be ",
		    "redefined in derived class.");
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
