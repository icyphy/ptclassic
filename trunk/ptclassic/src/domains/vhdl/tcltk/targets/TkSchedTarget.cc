static const char file_id[] = "TkSchedTarget.cc";
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

 Programmer: Michael C. Williamson

 VHDL target for interactive architecture planning.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "TkSchedTarget.h"
#include "KnownTarget.h"
#include "HashTable.h"

#include <fstream.h>

/*
#
#	The following code is added so that an interactive Tcl/Tk
#	application can be executed prior to generating the final
#	VHDL code.
#
*/

#include <tcl.h>
#include <tk.h>

#define COMMANDSIZE 1024

static VHDLFiringList* ourFiringList;
static char command[COMMANDSIZE];
Tk_Window mainWindow;
static int debug = 0;
static int exitRequest = 0;
const char *graph_file_name;
StringList theDestDir;
StringList theFilePrefix;

static int
handleExit(ClientData dummy, Tcl_Interp* interp, int argc, char* argv[])
{
  // To avoid a warning
  if (dummy) {}
  if (interp) {}
  if (argc) {}
  if (argv) {}

  exitRequest = 1;
  return TCL_OK;
}

static int
XErrorProc(ClientData data, XErrorEvent* errEventPtr)
{
  // To avoid a warning
  if (data) {}

  fprintf(stderr, "X protocol error: ");
  fprintf(stderr, "error=%d request=%d minor=%d\n",
	  errEventPtr->error_code, errEventPtr->request_code,
	  errEventPtr->minor_code);
  return 0;
}

/*
int RandomCmd(ClientData clientData,
	      Tcl_Interp *interp,
	      int argc, char *argv[]);
	      */

int SetProcCmd(ClientData clientData,
	       Tcl_Interp *interp,
	       int argc, char *argv[]);

int SetTimesCmd(ClientData clientData,
		Tcl_Interp *interp,
		int argc, char *argv[]);

void do_main(int argc, char *argv[], VHDLFiringList* theFiringList) {
  Tcl_Interp *interp;
  int error; char *trace;

  ourFiringList = theFiringList;

  // To avoid a warning
  if (argc) {}
  if (interp) {}

  interp = Tcl_CreateInterp();

  if (Tcl_Init(interp) == TCL_ERROR) {
    fprintf(stderr,
	    "%s: Error while trying to initialize tcl: %s\n",
	    argv[0], interp->result);
    exit(1);
  }
  if (Tk_Init(interp) == TCL_ERROR)  {
    fprintf(stderr,
	    "%s: Error while trying to get main window: %s\n",
	    argv[0], interp->result);
    exit(1);
  }

  Tcl_StaticPackage(interp, "Tk", Tk_Init, (Tcl_PackageInitProc *) NULL);
  mainWindow = Tk_MainWindow(interp);

  if (mainWindow == NULL) {
    fprintf(stderr, "%s\n", interp->result);
    exit(1);
  }
  Tk_CreateErrorHandler(Tk_Display(mainWindow), -1, -1, -1,
			XErrorProc, (ClientData)mainWindow);
  if (debug) {
    XSynchronize(Tk_Display(mainWindow), True);
  }
  Tk_GeometryRequest(mainWindow, 200, 200);
  Tk_SetWindowBackground(mainWindow,
			 WhitePixelOfScreen(Tk_Screen(mainWindow)));

  Tcl_CreateCommand(interp, "handleExit", handleExit,
		    (ClientData)Tk_MainWindow(interp),
		    (Tcl_CmdDeleteProc *)NULL);

  /*
  Tcl_CreateCommand(interp, "random", RandomCmd,
		    (ClientData) 0,
		    (Tcl_CmdDeleteProc *)NULL);
		    */

  Tcl_CreateCommand(interp, "setProc", SetProcCmd,
		    (ClientData) 0,
		    (Tcl_CmdDeleteProc *)NULL);

  Tcl_CreateCommand(interp, "setTimes", SetTimesCmd,
		    (ClientData) 0,
		    (Tcl_CmdDeleteProc *)NULL);

  // GRAPH_FILE must be set before the tcl script can be evaluated.
  StringList filNam = "";
  filNam << theDestDir;
  filNam << "/" ;
  filNam << theFilePrefix;
  filNam << ".gra";

  graph_file_name = filNam;

  sprintf(command, "set GRAPH_FILE {%s}", graph_file_name);

  Tcl_Eval(interp, command);

  // Path of the Tcl/Tk file is hardwired in here:
  const char* tclFile = "$PTOLEMY/src/domains/vhdl/tcltk/targets/TkSched.tcl";
  char* tclFileExp = expandPathName(tclFile);
  error = Tcl_EvalFile(interp, tclFileExp);
  if (error != TCL_OK) {
    fprintf(stderr, "%s: %s\n", tclFileExp, interp->result);
    trace = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    if (trace != NULL) {
      fprintf(stderr, "*** TCL TRACE ***\n");
      fprintf(stderr, "%s\n", trace);
    }
  }

  // Enter the custom event loop.
  exitRequest = 0;
  //  fprintf(stderr, "Entering main event loop\n");
  while (!exitRequest) {
    Tk_DoOneEvent(TK_ALL_EVENTS);
  }
  //  fprintf(stderr, "Exiting main event loop\n");
  // Exit the custom event loop.

  // Close the Tk window.
  Tcl_Eval(interp, "destroy .");
  return;
}

/*
int Tcl_AppInit(Tcl_Interp *interp) {
  //  char* tcl_RcFileName;

  printf("Tcl_AppInit called!\n");

  if (Tcl_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }
  if (Tk_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }
  
  // Formerly:
  //		    (ClientData)Tk_MainWindow(interp),
  Tcl_CreateCommand(interp, "random", RandomCmd,
		    (ClientData) 0,
		    (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateCommand(interp, "setProc", SetProcCmd,
		    (ClientData) 0,
		    (Tcl_CmdDeleteProc *)NULL);
  Tcl_SetVar(interp, "tcl_rcFileName", "~/.myapp.tcl", TCL_GLOBAL_ONLY);

  tcl_RcFileName = "~/.myapp.tcl";

  return TCL_OK;
}
*/

/*
int RandomCmd(ClientData clientData,
	      Tcl_Interp *interp,
	      int argc, char *argv[])
{
  // To avoid a warning
  if (clientData) {}

  int rand, error;
  int limit = 0;
  if (argc > 2) {
    interp->result =  "Usage: random ?range?";
    return TCL_ERROR;
  }
  if (argc == 2) {
    error = Tcl_GetInt(interp, argv[1], &limit);
    if (error != TCL_OK) {
      return error;
    }
  }
  rand = 4949;
  if (limit != 0) {
    rand = rand % limit;
  }
  sprintf(interp->result, "%d", rand);
  return TCL_OK;
}
*/

int SetProcCmd(ClientData clientData,
	      Tcl_Interp *interp,
	      int argc, char *argv[])
{
  // To avoid a warning
  if (clientData) {}

  int error;
  char* firingName;
  int procNum;
  if (argc != 3) {
    interp->result =  "Usage: setProc ?firingName? ?procNum?";
    return TCL_ERROR;
  }
  if (argc == 3) {
    firingName = argv[1];
    error = Tcl_GetInt(interp, argv[2], &procNum);
    if (error != TCL_OK) {
      return error;
    }
  }
  //  printf("Setting procNum for %s to %d\n", firingName, procNum);

  VHDLFiring* firing = new VHDLFiring;
  firing = ourFiringList->vhdlFiringWithName(firingName);
  if (firing) {
    firing->groupNum = procNum;
  }
  else {
    printf("\n");
    printf("Couldn't find firing with name %s\n", firingName);
    printf("Size of ourFiringList is %d\n", ourFiringList->size());
    printf("\n");
  }

  return TCL_OK;
}

int SetTimesCmd(ClientData clientData,
		Tcl_Interp *interp,
		int argc, char *argv[])
{
  // To avoid a warning
  if (clientData) {}

  int error;
  char* firingName;
  int startTime, endTime, latency;
  if (argc != 5) {
    interp->result = 
      "Usage: setTimes ?firingName? ?startTime? ?endTime? ?latency?";
    return TCL_ERROR;
  }
  if (argc == 5) {
    firingName = argv[1];
    error = Tcl_GetInt(interp, argv[2], &startTime);
    if (error != TCL_OK) {
      return error;
    }
    error = Tcl_GetInt(interp, argv[3], &endTime);
    if (error != TCL_OK) {
      return error;
    }
    error = Tcl_GetInt(interp, argv[4], &latency);
    if (error != TCL_OK) {
      return error;
    }
  }
  /*
  printf("Setting timing for %s to %d, %d, %d\n",
	 firingName, startTime, endTime, latency);
	 */

  VHDLFiring* firing = new VHDLFiring;
  firing = ourFiringList->vhdlFiringWithName(firingName);
  if (firing) {
    firing->startTime = startTime;
    firing->endTime = endTime;
    firing->latency = latency;
  }
  else {
    printf("\n");
    printf("Couldn't find firing with name %s\n", firingName);
    printf("Size of ourFiringList is %d\n", ourFiringList->size());
    printf("\n");
  }

  return TCL_OK;
}

/*
#
#	End Tcl/Tk support code.
#
*/

// Constructor.
TkSchedTarget :: TkSchedTarget(const char* name,const char* starclass,
			 const char* desc) :
SynthArchTarget(name,starclass,desc) {
  // Maybe uncomment the following if not simulating:
  // Set the default to display the code.
  //  analyze.setInitValue("NO");

  // Set the default to not use loop scheduling.
  loopingLevel.setInitValue("0");

  addCodeStreams();
  initCodeStreams();
}

// Clone the Target.
Block* TkSchedTarget :: makeNew() const {
  LOG_NEW; return new TkSchedTarget(name(), starType(), descriptor());
}

static TkSchedTarget proto("TkSched-VHDL", "VHDLStar",
			 "VHDL code generation target with structural style");
static KnownTarget entry(proto,"TkSched-VHDL");

// Begin method.
void TkSchedTarget :: begin() {
  SynthArchTarget::begin();
}

// Setup the target.
void TkSchedTarget :: setup() {
  if (galaxy()->isItWormhole()) {
    // Currently this does not seem to get invoked:
    Error::warn("This galaxy is a wormhole!");
  }
  if (galaxy()) setStarIndices(*galaxy()); 
  
  SynthArchTarget::setup();
}

// Write the code to a file.
void TkSchedTarget :: interact() {
  StringList graph_data;

  int lat = 10;
  // Iterate over firing list and print names.
  VHDLFiringListIter fireNext(masterFiringList);
  VHDLFiring* nextFire;
  while ((nextFire = fireNext++) != 0) {
    graph_data << "Node " << nextFire->name << " latency "
	       << lat << "\n";
    lat += 10;
  //	       << nextFire->latency << "\n";
  }
  // Iterate over dependency list and print names.
  VHDLDependencyListIter depNext(dependencyList);
  VHDLDependency* nextDep;
  while ((nextDep = depNext++) != 0) {
    graph_data << "Conn " << nextDep->source->name << " -> "
	       << nextDep->sink->name << ";" << "\n";
  }
  // Iterate over dependency list and print names.
  VHDLDependencyListIter iterDepNext(iterDependencyList);
  VHDLDependency* nextIterDep;
  while ((nextIterDep = iterDepNext++) != 0) {
    graph_data << "IterConn " << nextIterDep->source->name << " -> "
	       << nextIterDep->sink->name << ";" << "\n";
  }
  // Iterate over token list and print names of source/dest tokens.
  VHDLTokenListIter tokenNext(tokenList);
  VHDLToken* token;
  while ((token = tokenNext++) != 0) {
    // If the token has no source firing, place it at the top
    // of the iteration cycle.
    if (!token->getSourceFiring()) {
      graph_data << "TopToken " << token->getName() << "\n";
      if (token->getDestFirings()->size()) {
	VHDLFiringListIter nextFiring(*(token->getDestFirings()));
	VHDLFiring* firing;
	while ((firing = nextFiring++) != 0) {
	  graph_data << "Conn " << token->getName() << " -> "
		     << firing->getName() << ";" << "\n";
	}
      }
    }
    /*
    // If the token has no dest firings, place it at the bottom
    // of the iteration cycle.
    if (!token->getDestFirings()->size()) {
      graph_data << "BottomToken " << token->getName() << "\n";
      if (token->getSourceFiring()) {
	graph_data << "Conn " << token->getSourceFiring()->getName()
	  << " -> " << token->getName() << ";" << "\n";
      }
    }
    */
  }

  theDestDir = expandPathName(destDirectory);
  theFilePrefix = filePrefix;
  
  writeFile(graph_data, ".gra", displayFlag);

  char* args[1];
  args[0] = "TkSched";

  // Setup and run the Tcl/Tk interactive graph display.
  do_main(1, args, &masterFiringList);
}

// Write the code.
void TkSchedTarget :: writeCode() {
  SynthArchTarget::writeCode();
}

// Compile the code.
int TkSchedTarget :: compileCode() {
  return SynthArchTarget::compileCode();
}

// Run the code.
int TkSchedTarget :: runCode() {
  return SynthArchTarget::runCode();
}

ISA_FUNC(TkSchedTarget,SynthArchTarget);
