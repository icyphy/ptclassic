static const char file_id[] = "StructTarget.cc";
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

 Base target for VHDL code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "StructTarget.h"
#include "KnownTarget.h"
#include "HashTable.h"

// Constructor.
StructTarget :: StructTarget(const char* name,const char* starclass,
			 const char* desc) :
SimVSSTarget(name,starclass,desc) {
  systemClockUsed = 0;
  regIntsUsed = 0;
  regRealsUsed = 0;
  muxIntsUsed = 0;
  muxRealsUsed = 0;
  sorIntsUsed = 0;
  sorRealsUsed = 0;

  // Maybe uncomment the following if not simulating:
  // Set the default to display the code.
  //  analyze.setInitValue("NO");

  // Set the default to not use loop scheduling.
  loopingLevel.setInitValue("0");

  addCodeStreams();
  initCodeStreams();
}

// Clone the Target.
Block* StructTarget :: makeNew() const {
  LOG_NEW; return new StructTarget(name(), starType(), descriptor());
}

static StructTarget proto("struct-VHDL", "VHDLStar",
			 "VHDL code generation target with structural style");
static KnownTarget entry(proto,"struct-VHDL");

void StructTarget :: begin() {
  SimVSSTarget::begin();
}

// Setup the target.
void StructTarget :: setup() {
  if (galaxy()->isItWormhole()) {
    // Currently this does not seem to get invoked:
    Error::warn("This galaxy is a wormhole!");
  }
  if (galaxy()) setStarIndices(*galaxy()); 
  
  SimVSSTarget::setup();

  clockList.initialize();

  // Need to make this conditional depending on if doing sim or synth.
  setSystemClockUsed();
}

// Main routine.
int StructTarget :: runIt(VHDLStar* s) {
  // Change the default stream temporarily.
  defaultStream = &firingAction;
  // Initialize lists for new firing.
  initFiringLists();
  // Process action; running star modifies myCode.
  int status = ((CGStar*) s)->CGStar::run();
  // Change the default stream back.
  defaultStream = &myCode;

  // Do not create firing data for forks.
  if (s->isItFork()) {
    return status;
  }

  // Create a new VHDLFiring to load up.
  VHDLFiring* fi = new VHDLFiring;

  // Begin constructing the components of the firing's code.
  StringList tempName = s->fullName();

  StringList clsName = s->className();
  if (!strcmp(clsName,"VHDLCReceive") || !strcmp(clsName,"VHDLCSend")) {
    fi->noSensitivities = TRUE;
    fi->noOutclocking = TRUE;
    // Do not tack on the firing number to the name.
  }
  else {
    fi->noSensitivities = FALSE;
    fi->noOutclocking = FALSE;
    // Tack on the firing number to the name.
    tempName << "_F" << s->firingNum();
  }
  StringList fiName = sanitize(tempName);

  // Search for a firing in the list with the same name as this one.
  // We're trying to find repeated firings of the same send/receive stars
  // so that they can be handled specially.
  int foundFiring = 0;
  VHDLCluster* ncl;
  if ((ncl = clusterList.vhdlClusterWithName(fiName)) != 0) {
    foundFiring = 1;
    VHDLFiring* nfi = ncl->firingList->head();
    // We found a cluster (firing) with the same name.
    // Presume it to be a send/receive star, since only those
    // have non-unique names from firing to firing.
    // Add to the generic list.
    nfi->genericList->addList(firingGenericList);
    // Add to the port list.
    nfi->portList->addList(firingPortList);
    // Add to the generic map list.
    nfi->genericMapList->addList(firingGenericMapList);
    // Add to the port map list.
    nfi->portMapList->addList(firingPortMapList);
    // Add to the signal list.
    nfi->signalList->addList(firingSignalList);
    // Add to the declarations.
    nfi->decls << mainDecls;
    // Add to the local variable declarations.
    nfi->decls << addVariableDecls(&localVariableList);
    // Add to the variable list.
    nfi->variableList->addList(firingVariableList);
    // Add to the PortVar list.
    nfi->portVarList->addList(firingPortVarList);
    // Add to the action list.
    StringList naction = "";
    naction << preSynch;
    naction << firingAction;
    naction << postSynch;
    nfi->action << naction;
    preSynch.initialize();
    firingAction.initialize();
    postSynch.initialize();
    // Add to the VarPort list.
    nfi->varPortList->addList(firingVarPortList);
  }

  if (!foundFiring) {
    fi->setName(fiName);
    fi->starClassName = s->className();
    fi->genericList = firingGenericList.newCopy();
    fi->portList = firingPortList.newCopy();
    fi->genericMapList = firingGenericMapList.newCopy();
    fi->portMapList = firingPortMapList.newCopy();
    fi->signalList = firingSignalList.newCopy();
    fi->decls = mainDecls;
    mainDecls.initialize();

    // If there are local/temporary variables, put them into
    // mainDecls here.  Then clear the localVariableList.
    fi->decls << addVariableDecls(&localVariableList);
    localVariableList.initialize();

    fi->variableList = firingVariableList.newCopy();
    fi->portVarList = firingPortVarList.newCopy();

    StringList action = "";
    action << preSynch;
    action << firingAction;
    action << postSynch;
  
    //  fi->action = firingAction;
    fi->action = action;

    preSynch.initialize();
    firingAction.initialize();
    postSynch.initialize();

    fi->varPortList = firingVarPortList.newCopy();

    // Create a new VHDLFiringList and put the firing in the list.
    VHDLFiringList* fl = new VHDLFiringList;
    fl->put(*fi);

    // Create a new VHDLCluster and put the firing list in the cluster.
    VHDLCluster* cl = new VHDLCluster;
    cl->setName(fi->name);
    cl->firingList = fl;
  
    // Add the cluster to the main list of clusters.
    clusterList.put(*cl);
  }

  // Vestigial code - see original for reasoning behind this, then change it.
  if (!status) {
    return status;
  }
  return status;
}

// Initial stage of code generation (done first).
void StructTarget :: headerCode() {
}

// Merge all firings into one cluster.
void StructTarget :: allFiringsOneCluster() {
  // Master cluster to hold all firings.
  VHDLCluster* masterC = new VHDLCluster;
  // Master firing list of master cluster.
  VHDLFiringList* masterFL = new VHDLFiringList;

  // loop through clusters in main cluster list
  VHDLClusterListIter nextCluster(clusterList);
  VHDLCluster* cl;
  while ((cl = nextCluster++) != 0) {
    // loop through firings in firing list of each cluster
    VHDLFiringList* fl = cl->firingList;
    VHDLFiringListIter nextFiring(*fl);
    VHDLFiring* fi;
    while ((fi = nextFiring++) != 0) {
      masterFL->put(*fi);
    }
  }

  // Put master firing list in master cluster.
  masterC->setName("MasterCluster");
  masterC->firingList = masterFL;

  // Set main cluster list to this one cluster.
  clusterList.initialize();
  clusterList.put(*masterC);
}

// Trailer code (done last).
void StructTarget :: trailerCode() {
  // Iterate through the state list and connect registers and
  // initial value multiplexors for each referenced state.
  VHDLStateListIter nextState(stateList);
  VHDLState* state;
  while ((state = nextState++) != 0) {
    // If its a constant source, need to do things a bit differently:
    // only have a source with a signal, and no mux or reg.
    if (state->constant) {
      mainSignalList.put(state->name, state->type);
      connectSource(state->initVal, state->name, state->type);
    }
    // If firings change state, need to connect a reg and a mux
    // between the lastRef to the state and the firstRef to the state.
    if (!(state->constant)) {
      StringList tempName = state->name;
      tempName << "_Temp";
      mainSignalList.put(tempName, state->type);

      StringList initName = state->name;
      initName << "_Init";
      mainSignalList.put(initName, state->type);

      connectMultiplexor(state->lastRef, state->firstRef, initName,
			 state->type);
      connectSource(state->initVal, initName, state->type);
    }
  }

  // Iterate through the arc list.
  // Track the read/write references made on each arc.
  VHDLArcListIter nextArc(arcList);
  VHDLArc* arc;
  while ((arc = nextArc++) != 0) {
    // Determine which signals need to be fed back through registers.
    // All signals which were read, but weren't written, must be latched.
    for (int ix = arc->lowRead; ix < arc->lowWrite; ix++) {
      StringList sourceName = arc->name;
      StringList destName = arc->name;
      int sx = (ix + (arc->highWrite - arc->lowWrite) + 1);
      if (sx >= 0) {
	sourceName << "_" << sx;
      }
      else { /* (sx < 0) */
	sourceName << "_N" << (-sx);
      }
      if (ix >= 0) {
	destName << "_" << ix;
      }
      else { /* (ix < 0) */
	destName << "_N" << (-ix);
      }

      // NEWER: Commenting these suckers out as of 7/24/96.
      // NEW: Even though these are "source" and "dest", the naming
      // convention is that any latched output of an exu is being
      // called "sink" (for now). 4/12/96.
      //      sourceName << "_sink";
      //      destName << "_sink";

      // sourceName is input to register, destName is output of register.
      connectRegister(sourceName, destName, "feedback_clock", arc->type);
      toggleClock("feedback_clock");

      // Must also create signals for those lines which are neither read nor
      // written by a $ref() - e.g. if more delays than tokens read.
      // However, do not create a signal if it's a wormhole input.
      // Will have created a system port input instead.

      // If no system port by the given name, go ahead and make the signal.
      if (sx < arc->lowWrite) {
	mainSignalList.put(sourceName, arc->type);
      }
      if (ix < arc->lowWrite) {
	mainSignalList.put(destName, arc->type);
      }
    }
  }

  // NEW 7/18/96 -- Relocate the addition of registers after all outputs of
  // firings to here.  Attempt to consolidate this as a postaction in prelude
  // to having wholesale rearrangement of structure by intermediate tool.
  VHDLClusterListIter nCluster(clusterList);
  VHDLCluster* ncl;
  while ((ncl = nCluster++) != 0) {
    VHDLFiringListIter nFiring(*(ncl->firingList));
    VHDLFiring* fi;
    while ((fi = nFiring++) != 0) {
      // An exception for those firings that wish to disable
      // output clocking:
      if (fi->noOutclocking) continue;
      // Otherwise, proceed.
      StringList fiName = fi->name;
      StringList clkName = fiName;
      clkName << "_clk";
      VHDLPortListIter nextPort(*(fi->portList));
      VHDLPort* po;
      while ((po = nextPort++) != 0) {
	if (!strcmp((po->direction),"OUT")) {
	  StringList oldMapping = "";
	  StringList newMapping = "";
	  VHDLPort* poMap;
	  const char* poName = po->name;
	  /*
	  {
	    printf("poName is:  %s\n", poName);
	    VHDLPortListIter pomNext(*(fi->portMapList));
	    VHDLPort* pom;
	    while ((pom = pomNext++) != 0) {
	      const char* nameo = pom->name;
	      printf("Port Map Name:  %s\n", nameo);
	    }
	  }
	  */
	  poMap = fi->portMapList->vhdlPortWithName(poName);
	  if (!poMap) {
	    Error::abortRun(poName, ": port not in firing's PortMapList.");
	    return;
	  }
	  oldMapping = poMap->mapping;
	  newMapping = oldMapping;
	  newMapping << "_new";
	  poMap->mapping = newMapping;
	  fi->signalList->put(newMapping, po->type);
	  connectRegister(newMapping, oldMapping, clkName, po->type);
	}
      }
    }
  }


  // Add in the entity, architecture, entity declaration, and
  // component mapping for the controller.
  VHDLFiring* ctlerFi = new VHDLFiring;
  StringList ctlName = hashstring(filePrefix);
  ctlName << "controller";
  ctlerFi->setName(ctlName);
  ctlerFi->starClassName = "Sequencer";
  ctlerFi->genericList = ctlerGenericList.newCopy();
  ctlerFi->portList = ctlerPortList.newCopy();
  ctlerFi->genericMapList = ctlerGenericMapList.newCopy();
  ctlerFi->portMapList = ctlerPortMapList.newCopy();
  ctlerFi->signalList = ctlerSignalList.newCopy();
  ctlerFi->variableList = ctlerVariableList.newCopy();
  ctlerFi->portVarList = ctlerPortVarList.newCopy();
  ctlerFi->action = ctlerAction;
  ctlerAction.initialize();
  ctlerFi->varPortList = ctlerVarPortList.newCopy();
  ctlerFi->noOutclocking = TRUE;

  // Create a new VHDLFiringList and put the firing in the list.
  VHDLFiringList* ctlerFl = new VHDLFiringList;
  ctlerFl->put(*ctlerFi);

  // Create a new VHDLCluster and put the firing list in the cluster.
  VHDLCluster* ctlerCl = new VHDLCluster;
  ctlerCl->setName(ctlerFi->name);
  ctlerCl->firingList = ctlerFl;
  
  // Add the cluster to the main list of clusters.
  clusterList.put(*ctlerCl);

  // Special Action: merge all firings into one cluster
  //  allFiringsOneCluster();

  int level = 0;
  // Iterate through the cluster list.
  // Generate entity, architecture for each firing.
  VHDLClusterListIter nextCluster(clusterList);
  VHDLCluster* cl;
  while ((cl = nextCluster++) != 0) {
    // Begin constructing the cluster's code in myCode.
    myCode << "\n-- Cluster " << cl->name << "\n";
    myCode << addFiringComments(cl->firingList, level);

    myCode << "\n";
    myCode << useLibs;
    myCode << "\n";
    myCode << "entity " << cl->name << " is\n";

    myCode << addGenericRefs(cl->genericList(), level);
    myCode << addPortRefs(cl->portList(), level);

    myCode << indent(level) << "end " << cl->name << ";\n";
    myCode << "\n";
    myCode << "architecture " << "behavior" << " of "
	   << cl->name << " is\n";
    myCode << "begin\n";
    myCode << "process\n";

    // To please the Synopsys synthesis:
    // it insists on there being a sensitivity list.
    myCode << addSensitivities(cl, level);

    myCode << addDeclarations(cl, level);
    myCode << addVariableRefs(cl, level);

    myCode << "begin\n";

    // To please the Synopsys simulation:
    // It refuses to initialize ports and variables, and if you
    // don't put a wait statement at the beginning of a process in lieu
    // of explicit initialization for everything (which is unsynthesizable)
    // then you get arithmetic overflow when you hit arithmetic expressions
    // whose inputs are unitiialized variables.
    // Currently we are handling this by doing an explicit global
    // initialization to zero of these values in the simulation
    // command file.
    //   myCode << addWaitStatement(cl, level);

    myCode << addPortVarTransfers(cl, level);
    myCode << addActions(cl, level);
    myCode << addVarPortTransfers(cl, level);

    myCode << "end process;\n";
    myCode << "end behavior;\n";
    
    registerAndMerge(cl);
  }
  
  buildEntityDeclaration(level);
  buildArchitectureBodyOpener(level);
  buildComponentDeclarations(level);
  signal_declarations << addSignalDeclarations(&mainSignalList, level);
  component_mappings << addComponentMappings(&mainCompMapList, level);
  buildArchitectureBodyCloser(level);
  buildConfigurationDeclaration(level);
}

// Combine all sections of code.
void StructTarget :: frameCode() {
  StringList code = headerComment();

  myCode << cli_models;

  if (systemClock()) {
    myCode << clockGenCode();
  }
  if (registerInts()) {
    myCode << regCode("INTEGER");
  }
  if (registerReals()) {
    myCode << regCode("REAL");
  }
  if (multiplexorInts()) {
    myCode << muxCode("INTEGER");
  }
  if (multiplexorReals()) {
    myCode << muxCode("REAL");
  }
  if (sourceInts()) {
    myCode << sourceCode("INTEGER");
  }
  if (sourceReals()) {
    myCode << sourceCode("REAL");
  }
  
  myCode << "\n-- entity_declaration\n";
  myCode << "\n" << entity_declaration;
  myCode << "\n-- architecture_body_opener\n";
  myCode << "\n" << architecture_body_opener;
  myCode << "\n-- component_declarations\n";
  myCode << "\n" << component_declarations;
  myCode << "\n-- signal_declarations\n";
  myCode << "\n" << signal_declarations;
  myCode << "\n" << "begin";
  myCode << "\n-- component_mappings\n";
  myCode << "\n" << component_mappings;
  myCode << "\n-- architecture_body_closer\n";
  myCode << "\n" << architecture_body_closer;
  myCode << "\n-- configuration_declaration\n";
  myCode << "\n" << configuration_declaration;

  // Prepend the header, declarations, and initialization.
  prepend(code, myCode);
}

// Write the code to a file.
void StructTarget :: writeCode() {
  SimVSSTarget::writeCode();
}

// Compile the code.
int StructTarget :: compileCode() {
  return SimVSSTarget::compileCode();
}

// Run the code.
int StructTarget :: runCode() {
  return SimVSSTarget::runCode();
}

// Merge the Star's signal list with the Target's signal list.
void StructTarget :: mergeSignalList(VHDLSignalList* starSignalList) {
  VHDLSignalListIter starSignalNext(*starSignalList);
  VHDLSignal* nStarSignal;
  // Scan through the list of signals from the star firing.
  while ((nStarSignal = starSignalNext++) != 0) {
    // Search for a match from the existing list.
    if (!(mainSignalList.inList(nStarSignal))) {
      // Allocate memory for a new VHDLSignal and put it in the list.
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal = nStarSignal->newCopy();
      mainSignalList.put(*newSignal);
    }
  }
}

// Register the State reference.
void StructTarget :: registerState(State* state, const char* varName,
				   int thisFiring/*=-1*/, int pos/*=-1*/) {
  StringList ref = varName;
  StringList stType = stateType(state);
  StringList root;
  StringList initVal;
  
  // Initialize the state and check for inconsistencies.
  if (pos >= 0) {
    if (!(state->isArray())) {
      Error::error(*state, " reference of non-arrayState as an arrayState");
    }
    else {
      if (pos >= state->size()) {
	Error::error(*state, " attempt to reference arrayState past its size");
      }
      else {
	state->initialize();
	ref << "_P" << pos;
	if (state->isA("FloatArrayState")) {
	  initVal << (*((FloatArrayState *) state))[pos];
	}
	else if (state->isA("IntArrayState")) {
	  initVal << (*((IntArrayState *) state))[pos];
	}
	else {
	  Error::error(*state, "is not a Float or Int ArrayState");
	}
      }
    }
  }
  else {
    state->initialize();
    initVal << state->currentValue();
  }
  
  // Root is ref, without marking for any particular firing.
  root = ref;

  StringList refIn = sanitize(ref);
  refIn << "_F" << thisFiring << "_In";
  StringList refOut = sanitize(ref);
  refOut << "_F" << thisFiring << "_Out";
  StringList reg = sanitize(root);
  reg << "_Reg";

  // look for state in stateList
  // if don't find it, enter it with -1 as state->lastFiring
  if (!(stateList.inList(root))) {
    // Allocate memory for a new VHDLState and put it in the list.
    VHDLState* newState = new VHDLState;
    newState->setName(root);
    newState->firstRef = refIn;
    newState->lastRef = refOut;
    newState->initVal = initVal;
    newState->type = stType;
    newState->lastFiring = -1;
    stateList.put(*newState);
  }

  int stateLastFiring = -1;
  VHDLState* listState = new VHDLState;
  if ((listState = stateList.vhdlStateWithName(root)) != 0) {
    stateLastFiring = listState->lastFiring;
  }
  
  int isFirstStateRef = (stateLastFiring == -1);
  
  // If this is the first reference to this state in this firing
  // need to put it in the proper lists and make connections.
  if (listState->lastFiring != thisFiring) {
    // Inside this if clause, this state is being referenced
    // for the first time in this firing, perhaps the first time ever.

    int constState = 0;
    
    // If state is constant, according to its attributes, do things
    // a bit differently: have all references refer only to the single
    // constant source, rather than passing through values from one firing
    // to the next, which is useless and restrictive.
    // Also, there should be no output on which to pass on the value,
    // since it isn't changed by this exu.

    if (state->attributes() & AB_CONST) {
      constState = 1;
      listState->constant = 1;
    }
    else {
      listState->constant = 0;
    }
    
    // Contstruct the name of the last ref to the state.
    StringList state_in = root;
    state_in << "_F" << listState->lastFiring;
    StringList state_out = root;
    state_out << "_F" << thisFiring;

    VHDLPort* inPort = new VHDLPort;
    inPort->setName(refIn);
    inPort->setType(stType);
    inPort->setDirec("IN");

    firingPortList.put(*inPort);

    firingVariableList.put(ref, stType, "");
    firingPortVarList.put(refIn, ref);

    if (constState) {
      if (isFirstStateRef) {
	// Create a new signal.
	VHDLSignal* inSignal = new VHDLSignal;
	inSignal->setName(root);
	inSignal->setType(stType);

	topSignalList.put(*inSignal);
	firingSignalList.put(*inSignal);

	inPort->connect(inSignal);

	firingPortMapList.put(inPort->name, "", "", inSignal->name);
      }
      if (!isFirstStateRef) {
	// Need to find the previous signal to connect to
	// That signal should have been created during the previous firing's
	// first reference to the state.
	// HEY! won't find it in mainSignalList, that doesn't go until trailerCode()!
	VHDLSignal* inSignal = topSignalList.vhdlSignalWithName(root);
	if (!inSignal) {
	  Error::abortRun(*this, "Not first state ref, but can't find any signal created for it");
	  return;
	}

	// FIXME: Should be able to comment these two out:
        topSignalList.put(*inSignal);
        firingSignalList.put(*inSignal);

	inPort->connect(inSignal);

	firingPortMapList.put(inPort->name, "", "", inSignal->name);
      }
    }

    if (!(constState)) {
      // Create an output signal to propagate the updated state value.
      VHDLPort* outPort = new VHDLPort;
      outPort->setName(refOut);
      outPort->setType(stType);
      outPort->setDirec("OUT");

      VHDLSignal* outSignal = new VHDLSignal;
      outSignal->setName(state_out);
      outSignal->setType(stType);

      outPort->connect(outSignal);

      firingPortMapList.put(outPort->name, "", "", outSignal->name);
      //      firingPortMapList.put(refOut, "", "", state_out);

      if (isFirstStateRef) {
	// Create a new signal.
	VHDLSignal* inSignal = new VHDLSignal;
	inSignal->setName(refIn);
	inSignal->setType(stType);

	topSignalList.put(*inSignal);
	firingSignalList.put(*inSignal);

	inPort->connect(inSignal);

	firingPortMapList.put(inPort->name, "", "", inSignal->name);
	//	firingPortMapList.put(refIn, "", "", refIn);
      }
      if (!isFirstStateRef) {
	// Need to find the previous signal to connect to
	// FIXME: Still don't like this, depends on knowing the name already!!
	VHDLSignal* inSignal = mainSignalList.vhdlSignalWithName(state_in);
	if (!inSignal) {
	  Error::abortRun(*this, "Not first state ref, but can't find any signal created for it");
	  return;
	}

	inPort->connect(inSignal);

	firingPortMapList.put(inPort->name, "", "", inSignal->name);
	//	firingPortMapList.put(refIn, "", "", state_in);
      }

      topSignalList.put(*outSignal);
      firingSignalList.put(*outSignal);

      firingPortList.put(*outPort);
      firingVarPortList.put(refOut, ref);

      // Data clock name.  Needs to be the name of the firing, not the star.
      StringList clockName = sanitize(state->parent()->fullName());
      clockName << "_F" << thisFiring;
      clockName << "_clk";

      // NEW: Stil need to get that clock timing right.
      toggleClock(clockName);
    }

    // Reset state's lastRef name.
    listState->lastRef = state_out;
  }

  // Reset state's lastFiring tag just before exiting.
  listState->lastFiring = thisFiring;
}

// Connect a source of the given value to the given signal.
void StructTarget :: connectSource(StringList initVal, StringList initName,
				   StringList type) {
      registerSource(type);
      StringList label = initName;
      label << "_SOURCE";
      StringList name = "Source";
      name << "_" << type;

      VHDLGenericList* genMapList = new VHDLGenericList;
      VHDLPortList* portMapList = new VHDLPortList;
      genMapList->initialize();
      portMapList->initialize();
      
      genMapList->put("value", "", initVal);
      portMapList->put("output", "", "", initName);
      mainCompMapList.put(label, portMapList, genMapList, name, portMapList, genMapList);
}

// Add a source component declaration.
void StructTarget :: registerSource(StringList type/*="INTEGER"*/) {
  // Set the flag indicating sources are needed.
  if (!strcmp(type,"INTEGER")) {
    setSourceInts();
  }
  if (!strcmp(type,"REAL")) {
    setSourceReals();
  }

  StringList name = "Source";
  name << "_" << type;

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;
  genList->initialize();
  portList->initialize();
  
  genList->put("value", type);
  portList->put("output", "OUT", type);

  mainCompDeclList.put(name, portList, genList, "", portList, genList);
}

// Connect a multiplexor between the given input and output signals.
void StructTarget :: connectMultiplexor(StringList inName, StringList outName,
					StringList initVal, StringList type) {
  assertClock("control");

  registerMultiplexor(type);
  StringList label = outName;
  label << "_MUX";
  StringList name = "Mux";
  name << "_" << type;

  VHDLGenericList* genMapList = new VHDLGenericList;
  VHDLPortList* portMapList = new VHDLPortList;
  genMapList->initialize();
  portMapList->initialize();
      
  portMapList->put("input", "", "", inName);
  portMapList->put("output", "", "", outName);
  portMapList->put("init_val", "", "", initVal);
  portMapList->put("control", "", "", "control");
  //  systemPortList.put("control", "IN", "boolean");
  ctlerPortList.put("control", "OUT", "boolean");
  ctlerPortMapList.put("control", "", "", "control");
  ctlerSignalList.put("control", "boolean");
  ctlerPortList.put("system_clock", "IN", "boolean");
  ctlerPortMapList.put("system_clock", "", "", "system_clock");
  // If using a system clock generator, then need a signal.
  if (systemClock()) {
    mainSignalList.put("system_clock", "boolean");
    connectClockGen("system_clock");
  }
  else {
    systemPortList.put("system_clock", "IN", "boolean");
  }
  mainCompMapList.put(label, portMapList, genMapList, name, portMapList, genMapList);
}

// Add a multiplexor component declaration.
void StructTarget :: registerMultiplexor(StringList type/*="INTEGER"*/) {
  // Set the flag indicating multiplexors and sources are needed.
  if (!strcmp(type,"INTEGER")) {
    setMultiplexorInts();
  }
  if (!strcmp(type,"REAL")) {
    setMultiplexorReals();
  }

  StringList name = "Mux";
  name << "_" << type;

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;
  genList->initialize();
  portList->initialize();
  
  portList->put("init_val", "IN", type);
  portList->put("input", "IN", type);
  portList->put("output", "OUT", type);
  portList->put("control", "IN", "boolean");

  mainCompDeclList.put(name, portList, genList, "", portList, genList);
}

// Connect a register between the given input and output signals.
void StructTarget :: connectRegister(StringList inName, StringList outName,
				     StringList clkName, StringList type) {
  registerRegister(type);
  StringList label = outName;
  label << "_REG";
  StringList name = "Reg";
  name << "_" << type;

  VHDLGenericList* genMapList = new VHDLGenericList;
  VHDLPortList* portMapList = new VHDLPortList;
  genMapList->initialize();
  portMapList->initialize();
      
  portMapList->put("D", "", "", inName);
  portMapList->put("Q", "", "", outName);
  portMapList->put("C", "", "", clkName);
  //  systemPortList.put(clkName, "IN", "boolean");
  ctlerPortList.put(clkName, "OUT", "boolean");
  ctlerPortMapList.put(clkName, "", "", clkName);
  ctlerSignalList.put(clkName, "boolean");
  ctlerPortList.put("system_clock", "IN", "boolean");
  ctlerPortMapList.put("system_clock", "", "", "system_clock");
  // If using a system clock generator, then need a signal.
  if (systemClock()) {
    mainSignalList.put("system_clock", "boolean");
    connectClockGen("system_clock");
  }
  else {
    systemPortList.put("system_clock", "IN", "boolean");
  }
  mainCompMapList.put(label, portMapList, genMapList, name, portMapList, genMapList);
}

// Connect a clock generator driving the given signal.
void StructTarget :: connectClockGen(StringList clkName) {
      registerClockGen();
      StringList label = clkName;
      label << "_Clock";
      StringList name = "ClockGen";

      VHDLGenericList* genMapList = new VHDLGenericList;
      VHDLPortList* portMapList = new VHDLPortList;
      genMapList->initialize();
      portMapList->initialize();
      
      portMapList->put("system_clock", "", "", clkName);
      portMapList->put("iter_clock", "", "", "iter_clock");
      mainCompMapList.put(label, portMapList, genMapList, name, portMapList, genMapList);
}

// Add a clock generator declaration.
void StructTarget :: registerClockGen() {
  // Set the flag indicating clock generator used.
  setSystemClockUsed();

  StringList name = "ClockGen";

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;
  genList->initialize();
  portList->initialize();
  
  portList->put("system_clock", "out", "boolean");
  portList->put("iter_clock", "in", "boolean");
  mainCompDeclList.put(name, portList, genList, "", portList, genList);
}

// Add a register component declaration.
void StructTarget :: registerRegister(StringList type/*="INTEGER"*/) {
  // Set the flag indicating registers are needed.
  if (!strcmp(type,"INTEGER")) {
    setRegisterInts();
  }
  if (!strcmp(type,"REAL")) {
    setRegisterReals();
  }

  StringList name = "Reg";
  name << "_" << type;

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;
  genList->initialize();
  portList->initialize();
  
  portList->put("D", "IN", type);
  portList->put("Q", "OUT", type);
  portList->put("C", "IN", "boolean");
  mainCompDeclList.put(name, portList, genList, "", portList, genList);
}

// Register PortHole reference.
void StructTarget :: registerPortHole(VHDLPortHole* port, const char* dataName,
				      int firing,
				      int tokenNum/*=-1*/,
				      const char* part/*=""*/) {
  // I want to pass along the offset info as well as whether it's
  // a read or a write so I can keep tabs on the production window and
  // the read window, and then do nice things at the end based on that.
  // Also, must do special things if it's a wormhole input.

  // Continue to do normal signal naming and portmapping.

  // The registry keeps track of all refed arcs, and their min/max R/W offsets.
  registerArcRef(port, tokenNum);

  StringList ref = dataName;
  ref << part;

  // Data clock name.  Needs to be the name of the firing, not the star.
  StringList clockName = sanitize(port->parent()->fullName());
  clockName << "_F" << firing;
  clockName << "_clk";

  // Look for a port with the given name already in the list.
  VHDLPort* existPort = firingPortList.vhdlPortWithName(ref);
  if (!existPort) {
    // Create a port and a port mapping for this firing entity.
    VHDLPort* newPort = new VHDLPort;
    newPort->setName(ref);
    newPort->setType(port->dataType());
    newPort->setDirec(port->direction());

    // If it's an input port, find the signal generating the data.
    if (!strcmp(newPort->direction,"IN")) {
      VHDLSignal* mySignal = topSignalList.vhdlSignalWithName(ref);
      // Check for signal already existing.
      if (mySignal) {
	// mySignal exists with ref as name, so connect to it.
	newPort->connect(mySignal);

	firingPortMapList.put(newPort->name, "", "", mySignal->name);
      }
      else {
	// tokenNum is negative, so create a signal to connect to.
	if (tokenNum < 0) {
	  mySignal = new VHDLSignal;
	  mySignal->setName(ref);
	  mySignal->setType(port->dataType());

	  topSignalList.put(*mySignal);
	  firingSignalList.put(*mySignal);

	  newPort->connect(mySignal);

	  firingPortMapList.put(newPort->name, "", "", mySignal->name);
	}
	// tokenNum not negative: signal should already exist, but doesn't.
	else {
	  Error::abortRun(ref, ": Need to connect to signal for data which hasn't",
			  " been generated yet - firing order error?");
	  return;
	}
      }
    }
    // If it's an output port, create a new output signal.
    if (!strcmp(newPort->direction,"OUT")) {
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal->setName(ref);
      newSignal->setType(port->dataType());

      topSignalList.put(*newSignal);
      firingSignalList.put(*newSignal);

      newPort->connect(newSignal);

      firingPortMapList.put(newPort->name, "", "", newSignal->name);
    }

    firingPortList.put(*newPort);
  }

  // Only provide a toggled clock for firing if data is output.
  if (port->isItOutput()) {
    toggleClock(clockName);
  }

  // For wormhole inputs, create a system port.
  // But for delayed values of wormhole inputs, do not create another port.
  if ((port->atBoundary()) && (tokenNum >= 0)) {
    // Signal an error: we won't support wormholes for now:
    Error::error(*port, "is at a wormhole boundary: Not currently supported");
    // Port at wormhole boundary, so create a system port instead of a signal.
    //    systemPortList.put(ref, port->direction(), port->dataType());
  }
  // For local inputs, create a signal.
  else {
    // The following is commented out: we only create and register
    // a new signal when the port that generates the data is created.
    //    firingSignalList.put(sinkName, port->dataType());
  }
}

// Return the assignment operator for States.
const char* StructTarget :: stateAssign() {
  // FIXME: Make these perm. strings (hash or save)
  const char* assign = ":=";
  return assign;
}

// Return the assignment operator for PortHoles.
const char* StructTarget :: portAssign() {
  // FIXME: Make these perm. strings (hash or save)
  const char* assign = "<=";
  return assign;
}

ISA_FUNC(StructTarget,SimVSSTarget);

// Method called by comm stars to place important code into structure.
void StructTarget :: registerComm(int direction, int pairid, int numxfer, const char* dtype) {
  // direction == 0 --> C2V ; direction == 1 --> V2C.
  // Create a string with the right VHDL data type
  StringList vtype = "";
  StringList name = "";
  if (strcmp(dtype, "INT") == 0) {
    vtype = "INTEGER";
    if (direction) {
      name = "V2Cinteger";
    }
    else {
      name = "C2Vinteger";
    }
  }
  else if (strcmp(dtype, "FLOAT") == 0) {
    vtype = "REAL";
    if (direction) {
      name = "V2Creal";
    }
    else {
      name = "C2Vreal";
    }
  }
  else {
    Error::abortRun(*this, dtype, ": type not supported");
    return;
  }
  
  // Construct unique label and signal names and put comp map in main list
  StringList label;
  StringList startName, goName, dataName, doneName, endName;
  StringList rootName = name;
  rootName << pairid;

  label << rootName;
  startName << rootName << "_start";
  goName << rootName << "_go";
  dataName << rootName << "_data";
  doneName << rootName << "_done";
  endName << rootName << "_end";
  
  VHDLGenericList* genMapList = new VHDLGenericList;
  VHDLPortList* portMapList = new VHDLPortList;
  genMapList->initialize();
  portMapList->initialize();
  
  genMapList->put("pairid", "", pairid);
  genMapList->put("numxfer", "", numxfer);
  portMapList->put("go", "", "", goName);
  portMapList->put("data", "", "", dataName);
  portMapList->put("done", "", "", doneName);
  mainCompMapList.put(label, portMapList, genMapList, name, portMapList, genMapList);

  ctlerPortList.put(startName, "OUT", "STD_LOGIC");
  firingPortList.put(startName, "IN", "STD_LOGIC");
  firingPortList.put(goName, "OUT", "STD_LOGIC");
  if (direction) {
    firingPortList.put(dataName, "OUT", vtype);
  }
  else {
    firingPortList.put(dataName, "IN", vtype);
  }
  firingPortList.put(doneName, "IN", "STD_LOGIC");
  firingPortList.put(endName, "OUT", "STD_LOGIC");
  ctlerPortList.put(endName, "IN", "STD_LOGIC");

  ctlerPortMapList.put(startName, "", "", startName);
  firingPortMapList.put(startName, "", "", startName);
  firingPortMapList.put(goName, "", "", goName);
  firingPortMapList.put(dataName, "", "", dataName);
  firingPortMapList.put(doneName, "", "", doneName);
  firingPortMapList.put(endName, "", "", endName);
  ctlerPortMapList.put(endName, "", "", endName);

  ctlerSignalList.put(startName, "STD_LOGIC");
  firingSignalList.put(startName, "STD_LOGIC");
  firingSignalList.put(goName, "STD_LOGIC");
  firingSignalList.put(dataName, vtype);
  firingSignalList.put(doneName, "STD_LOGIC");
  firingSignalList.put(endName, "STD_LOGIC");
  ctlerSignalList.put(endName, "STD_LOGIC");

  ctlerAction << startName << " <= '0';\n";
  preSynch << "wait on " << startName << "'transaction;\n";
  postSynch << endName << " <= '0';\n";
  ctlerAction << "wait on " << endName << "'transaction;\n";
}

// Add in sensitivity list of input ports.
// Do this explicitly for sake of synthesis.
StringList StructTarget :: addSensitivities(VHDLCluster* cl, int level) {
  StringList all;
  if ((*(cl->firingList)).head()) {
    StringList opener, body, closer;

    level++;
    opener << indent(level) << "(\n";

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int sensCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      // Special exception to avoid sensitivity list for controller
      // which will contain wait statements.  Synopsys complains that
      // it can't synthesize blocks with wait statements if they also
      // have sensitivity lists.
      StringList ctlName = hashstring(filePrefix);
      ctlName << "controller";
      const char* ctlString = (const char*) ctlName;
      if (!strcmp(nfiring->name,ctlString)) continue;
      // Another exception for those firings that wish to disable
      // sensitivity lists, usually due to needing wait statements
      // within the firing, such as for send/receive synchronization.
      if (nfiring->noSensitivities) continue;
      // Otherwise, proceed.
      if ((*(nfiring->portList)).head()) {
	VHDLPortListIter nextPort(*(nfiring->portList));
	VHDLPort* nport;
	while ((nport = nextPort++) != 0) {
	  if (!strcmp(nport->direction,"IN")) {
	    level++;
	    if (sensCount) {
	      body << ",\n";
	    }
	    body << indent(level) << nport->name;
	    sensCount++;
	    level--;
	  }
	}
      }
    }
    
    closer << "\n";
    closer << indent(level) << ")\n";
    level--;

    if (sensCount) {
      all << opener << body << closer;
    }
  }
  return all;
}

// Add in wait statement with list of input ports.
// Do this explicitly for sake of simulation.
StringList StructTarget :: addWaitStatement(VHDLCluster* cl, int level) {
  StringList all;
  if ((*(cl->firingList)).head()) {
    StringList opener, body, closer;

    level++;
    opener << indent(level) << "wait on\n";

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int sensCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      // Special exception to avoid sensitivity list for controller
      // which will contain wait statements.  Synopsys complains that
      // it can't synthesize blocks with wait statements if they also
      // have sensitivity lists.  Also don't want more than one wait
      // statement.
      StringList ctlName = hashstring(filePrefix);
      ctlName << "controller";
      const char* ctlString = (const char*) ctlName;
      if (!strcmp(nfiring->name,ctlString)) continue;
      if ((*(nfiring->portList)).head()) {
	VHDLPortListIter nextPort(*(nfiring->portList));
	VHDLPort* nport;
	while ((nport = nextPort++) != 0) {
	  if (!strcmp(nport->direction,"IN")) {
	    level++;
	    if (sensCount) {
	      body << ",\n";
	    }
	    body << indent(level) << nport->name;
	    sensCount++;
	    level--;
	  }
	}
      }
    }
    
    closer << "\n";
    closer << indent(level) << ";\n";
    level--;

    if (sensCount) {
      all << opener << body << closer;
    }
  }
  return all;
}

// Add in variable refs here from variableList.
StringList StructTarget :: addVariableRefs(VHDLCluster* cl, int level) {
  StringList all;
  if ((*(cl->firingList)).head()) {
    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    while ((nfiring = nextFiring++) != 0) {
      all << addVariableDecls(nfiring->variableList,level);
    }
  }
  return all;
}

// Add in port to variable transfers here from portVarList.
StringList StructTarget :: addPortVarTransfers(VHDLCluster* cl,
					       int /*level*/) {
  StringList all;
  if ((*(cl->firingList)).head()) {
    StringList body;

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int portVarCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      VHDLPortVarListIter nextPortVar(*(nfiring->portVarList));
      VHDLPortVar* nPortVar;
      while ((nPortVar = nextPortVar++) != 0) {
	body << nPortVar->variable << " := " << nPortVar->name << ";\n";
	portVarCount++;
      }
    }
    
    if (portVarCount) {
      all << body;
    }
  }
  return all;
}

// Add in firing declarations here.
StringList StructTarget :: addDeclarations(VHDLCluster* cl, int /*level*/) {
  StringList all;
  if ((*(cl->firingList)).head()) {
    StringList body;

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int declsCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      body << nfiring->decls;
      declsCount++;
    }
    
    if (declsCount) {
      all << body;
    }
  }
  return all;
}

// Add in firing actions here.
StringList StructTarget :: addActions(VHDLCluster* cl, int /*level*/) {
  StringList all;
  if ((*(cl->firingList)).head()) {
    StringList body;

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int actionCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      if (!actionCount) {
	body << "-- if " << cl->name << "_SELECT" << " = " << nfiring->name
	     << " then\n";
      }
      else {
	body << "-- elsif " << cl->name << "_SELECT" << " = " << nfiring->name
	     << " then\n";
      }
      body << nfiring->action;
      actionCount++;
    }
    if (actionCount) {
      body << "-- endif\n";
    }

    if (actionCount) {
      all << body;
    }
  }
  return all;
}

// Add in variable to port transfers here from varPortList.
StringList StructTarget :: addVarPortTransfers(VHDLCluster* cl,
					       int /*level*/) {
  StringList all;
  if ((*(cl->firingList)).head()) {
    StringList body;

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int varPortCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      VHDLPortVarListIter nextVarPort(*(nfiring->varPortList));
      VHDLPortVar* nVarPort;
      while ((nVarPort = nextVarPort++) != 0) {
	body << nVarPort->name << " <= " << nVarPort->variable << ";\n";
	varPortCount++;
      }
    }
    
    if (varPortCount) {
      all << body;
    }
  }
  return all;
}

// Register compDecls and compMaps and merge signals.
void StructTarget :: registerAndMerge(VHDLCluster* cl) {
  StringList clLabel = cl->name;
  clLabel << "_proc";
  StringList clName = cl->name;
  
  // iterate through firings
  // tack all onto master lists
  VHDLPortList* masterPortList = new VHDLPortList;
  VHDLGenericList* masterGenericList = new VHDLGenericList;
  VHDLSignalList* masterSignalList = new VHDLSignalList;
  VHDLPortList* masterPortMapList = new VHDLPortList;
  VHDLGenericList* masterGenericMapList = new VHDLGenericList;

  VHDLFiringListIter nextFiring(*(cl->firingList));
  VHDLFiring* fi;
  while ((fi = nextFiring++) != 0) {
    VHDLPortListIter nextPort(*(fi->portList));
    VHDLPort* po;
    while ((po = nextPort++) != 0) {
      VHDLPort* newPort = new VHDLPort;
      newPort = po->newCopy();
      masterPortList->put(*newPort);
    }
    VHDLGenericListIter nextGeneric(*(fi->genericList));
    VHDLGeneric* ge;
    while ((ge = nextGeneric++) != 0) {
      VHDLGeneric* newGeneric = new VHDLGeneric;
      newGeneric = ge->newCopy();
      masterGenericList->put(*newGeneric);
    }
    VHDLSignalListIter nextSignal(*(fi->signalList));
    VHDLSignal* si;
    while ((si = nextSignal++) != 0) {
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal = si->newCopy();
      masterSignalList->put(*newSignal);
    }
    VHDLPortListIter nextPortMap(*(fi->portMapList));
    VHDLPort* pm;
    while ((pm = nextPortMap++) != 0) {
      VHDLPort* newPortMap = new VHDLPort;
      newPortMap = pm->newCopy();
      masterPortMapList->put(*newPortMap);
    }
    VHDLGenericListIter nextGenericMap(*(fi->genericMapList));
    VHDLGeneric* gm;
    while ((gm = nextGenericMap++) != 0) {
      VHDLGeneric* newGenericMap = new VHDLGeneric;
      newGenericMap = gm->newCopy();
      masterGenericMapList->put(*newGenericMap);
    }
  }

  mainCompDeclList.put(clName, masterPortList, masterGenericList, "", masterPortList, masterGenericList);
  mergeSignalList(masterSignalList);
  mainCompMapList.put(clLabel,  masterPortMapList, masterGenericMapList,
		      clName, masterPortMapList, masterGenericMapList);
}

// Generate the entity_declaration.
void StructTarget :: buildEntityDeclaration(int level) {
  entity_declaration << "-- firing use clauses\n";
  entity_declaration << "library SYNOPSYS,IEEE,PTVHDLSIM;\n";
  entity_declaration << "use SYNOPSYS.ATTRIBUTES.all;\n";
  entity_declaration << "use IEEE.STD_LOGIC_1164.all;\n";
  entity_declaration << "use std.textio.all;\n";

  entity_declaration << "entity " << (const char*) filePrefix << " is\n";
  if (systemPortList.head()) {
    level++;
    entity_declaration << indent(level) << "port(\n";
    VHDLPortListIter nextPort(systemPortList);
    VHDLPort* nport;
    int portCount = 0;
    while ((nport = nextPort++) != 0) {
      level ++;
      if (portCount) {
	entity_declaration << ";\n";
      }
      entity_declaration << indent(level) << nport->name << ": "
			 << nport->direction << " " << nport->type;
      portCount++;
      level--;
    }
    entity_declaration << "\n";
    entity_declaration << indent(level) << ");\n";
    level--;
  }
  entity_declaration << "end " << (const char*) filePrefix << ";\n";
}

// Generate the architecture_body_opener.
void StructTarget :: buildArchitectureBodyOpener(int /*level*/) {
  architecture_body_opener << "architecture " << "structure" << " of "
			   << (const char*) filePrefix << " is\n";
}

// Add in component declarations here from mainCompDeclList.
void StructTarget :: buildComponentDeclarations(int level) {
  component_declarations << cli_comps;

  // HashTable to keep track of which components already declared.
  HashTable myTable;
  myTable.clear();

  VHDLCompDeclListIter nextCompDecl(mainCompDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    if (!(myTable.hasKey(compDecl->name))) {
      myTable.insert(compDecl->name, compDecl);

      level++;
      component_declarations << indent(level) << "component " << compDecl->name
			     << "\n";

      // Add in generic refs here from genList.
      if (compDecl->genList->head()) {
	level++;
	component_declarations << indent(level) << "generic(\n";
	VHDLGenericListIter nextGen(*(compDecl->genList));
	VHDLGeneric* ngen;
	int genCount = 0;
	while ((ngen = nextGen++) != 0) {
	  level++;
	  if (genCount) {
	    component_declarations << ";\n";
	  }
	  component_declarations << indent(level) << ngen->name << ": "
				 << ngen->type;
	  if (ngen->defaultVal.length() > 0) {
	    component_declarations << " := " << ngen->defaultVal;
	  }
	  genCount++;
	  level--;
	}
	component_declarations << "\n";
	component_declarations << indent(level) << ");\n";
	level--;
      }
    
      // Add in port refs here from portList.
      if (compDecl->portList->head()) {
	level++;
	component_declarations << indent(level) << "port(\n";
	VHDLPortListIter nextPort(*(compDecl->portList));
	VHDLPort* nport;
	int portCount = 0;
	while ((nport = nextPort++) != 0) {
	  level++;
	  if (portCount) {
	    component_declarations << ";\n";
	  }
	  component_declarations << indent(level) << nport->name << ": "
				 << nport->direction << " " << nport->type;
	  portCount++;
	  level--;
	}
	component_declarations << "\n";
	component_declarations << indent(level) << ");\n";
	level--;
      }
    
      component_declarations << indent(level) << "end component;\n";
      level--;
    }
  }
}

// Generate the architecture_body_closer.
void StructTarget :: buildArchitectureBodyCloser(int /*level*/) {
  architecture_body_closer << "end structure;\n";
}

// Add in configuration declaration here from mainCompDeclList.
void StructTarget :: buildConfigurationDeclaration(int level) {
  configuration_declaration << "configuration " << (const char*) filePrefix
			    << "_parts" << " of " << (const char*) filePrefix
			    << " is\n";
  configuration_declaration << "for " << "structure" << "\n";

  configuration_declaration << cli_configs;

  VHDLCompDeclListIter nextCompDecl(mainCompDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    level++;
    configuration_declaration << indent(level) << "for all:" << compDecl->name
			      << " use entity " << "work." << compDecl->name
			      << "(behavior); end for;\n";
    level--;
  }

  configuration_declaration << "end " << "for" << ";\n";
  configuration_declaration << "end " << (const char*) filePrefix << "_parts"
			    << ";\n";
}

// Generate the clock generator entity and architecture.
StringList StructTarget :: clockGenCode() {
  // This comes from CGTarget::mainLoopCode() since it isn't exported
  // out of there, but beginIteration is called from there.
  // We may need to change the in-wormhole behavior sometime.
  int repetitions = inWormHole()? -1 : (int)scheduler()->getStopTime();

  StringList codeList;
  codeList << "\n";
  codeList << "     -- ClockGen : clock generator\n";
  codeList << "entity ClockGen is\n";
  codeList << "     port (system_clock: out boolean;\n";
  codeList << "           iter_clock: in boolean;\n";
  codeList << "           iter_count: inout integer);\n";
  codeList << "end ClockGen;\n";
  codeList << "\n";
  codeList << "architecture behavior of ClockGen is\n";
  codeList << "begin\n";
  codeList << "     main: process\n";
  codeList << "     	variable internal: boolean := FALSE;\n";
  codeList << "     begin\n";
  codeList << "		if iter_count < " << repetitions << " then\n";
  codeList << "         	wait for 1 ps;\n";
  codeList << "         	internal := not internal;\n";
  codeList << "         	system_clock <= internal;\n";
  codeList << "     	else\n";
  codeList << "     		wait;\n";
  codeList << "     	end if;\n";
  codeList << "     end process main;\n";
  codeList << "\n";
  codeList << "     count: process\n";
  codeList << "     	variable internal: integer := 0;\n";
  codeList << "     begin\n";
  codeList << "		wait until iter_clock'event and iter_clock = TRUE;\n";
  codeList << "		internal := internal + 1;\n";
  codeList << "         iter_count <= internal;\n";
  codeList << "     end process count;\n";
  codeList << "end behavior;\n";
  codeList << "\n";
  return codeList;
}

// Generate the register entity and architecture.
StringList StructTarget :: regCode(StringList type) {
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Reg_" << type << " : register of type " << type << "\n";
  codeList << "entity Reg_" << type << " is\n";
  codeList << "     port (C: in boolean; D: in " << type << "; Q: out " << type << ");\n";
  codeList << "end Reg_" << type << ";\n";
  codeList << "\n";
  codeList << "architecture behavior of Reg_" << type << " is\n";
  codeList << "begin\n";
  codeList << "     main: process\n";
  codeList << "     begin\n";
  codeList << "          wait until C'event and C = TRUE;\n";
  codeList << "          Q <= D;\n";
  codeList << "     end process main;\n";
  codeList << "end behavior;\n";
  codeList << "\n";
  return codeList;
}

// Generate the multiplexor entity and architecture.
StringList StructTarget :: muxCode(StringList type) {
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Mux_" << type << " : initial value multiplexor of type " << type << "\n";
  codeList << "entity Mux_" << type << " is\n";
  codeList << "	port(\n";
  codeList << "		control: IN BOOLEAN;\n";
  codeList << "		init_val: IN " << type << ";\n";
  codeList << "		input: IN " << type << ";\n";
  codeList << "		output: OUT " << type << "\n";
  codeList << "	);\n";
  codeList << "end Mux_" << type << ";\n";
  codeList << "\n";
  codeList << "architecture behavior of Mux_" << type << " is\n";
  codeList << "	begin\n";
  codeList << "		process (control, init_val, input)\n";
  codeList << "		begin\n";
  codeList << "			if control then\n";
  codeList << "				output <= input;\n";
  codeList << "			else\n";
  codeList << "				output <= init_val;\n";
  codeList << "			end if;\n";
  codeList << "		end process;\n";
  codeList << "end behavior;\n";
  codeList << "\n";
  return codeList;
}

// Generate the source entity and architecture.
StringList StructTarget :: sourceCode(StringList type) {
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Source_" << type << " : constant generator\n";
  codeList << "entity Source_" << type << " is\n";
  codeList << "	generic(\n";
  codeList << "		value: " << type << "\n";
  codeList << "	);\n";
  codeList << "	port(\n";
  codeList << "		output: OUT " << type << "\n";
  codeList << "	);\n";
  codeList << "end Source_" << type << ";\n";
  codeList << "\n";
  codeList << "architecture behavior of Source_" << type << " is\n";
  codeList << "	begin\n";
  codeList << "		output <= value;\n";
  codeList << "end behavior;\n";
  codeList << "\n";
  return codeList;
}

// Generate a comment.
StringList StructTarget :: comment(const char* text, const char* b,
				 const char* e, const char* c) {
    const char* begin = "-- ";
    const char* end = "";
    const char* cont = "-- ";
    if (b != NULL) begin = b;
    if (e != NULL) end = e;
    if (c != NULL) cont = c;
    return HLLTarget::comment(text, begin, end, cont);
}

// Generate code to begin an iterative procedure
void StructTarget :: beginIteration(int /*repetitions*/, int /*depth*/) {
}

// Generate code to end an iterative procedure
void StructTarget :: endIteration(int /*reps*/, int /*depth*/) {
  toggleClock("iter_clock");
}

// Add the clock to the clock list and generate code to toggle it.
void StructTarget :: toggleClock(const char* clock) {
  // Pulse it HI-LO if it isn't already at the end of the list.
  if (!(clockList.tail()) || (strcmp(clockList.tail(),clock))) {
    clockList << clock;
    ctlerAction << "     -- Toggle " << clock << "\n";
    ctlerAction << "wait until "
		<< "system_clock'event and system_clock = TRUE;\n";
    ctlerAction << clock << " <= TRUE;\n";
    ctlerAction << "wait until "
		<< "system_clock'event and system_clock = TRUE;\n";
    ctlerAction << clock << " <= FALSE;\n";
    ctlerPortList.put(clock, "OUT", "boolean");
    ctlerPortMapList.put(clock, "", "", clock);
    ctlerSignalList.put(clock, "boolean");
  }
}

// Add the clock to the clock list and generate code to assert it.
void StructTarget :: assertClock(const char* clock) {
  // Assert it HI if it isn't already at the end of the list.
  if (!(clockList.tail()) || (strcmp(clockList.tail(),clock))) {
    clockList << clock;
    ctlerAction << "     -- Assert " << clock << "\n";
    ctlerAction << "wait until "
		<< "system_clock'event and system_clock = TRUE;\n";
    ctlerAction << clock << " <= TRUE;\n";
    ctlerPortList.put(clock, "OUT", "boolean");
    ctlerPortMapList.put(clock, "", "", clock);
    ctlerSignalList.put(clock, "boolean");
  }
}

int StructTarget :: codeGenInit() {
  // Set the names of all geodesics.
  setGeoNames(*galaxy());
  // Call initCode for each star.
  GalStarIter nextStar(*galaxy());
  VHDLStar* s;
  while ((s = (VHDLStar*) nextStar++) != 0) {
    // Nothing needs to be done for forks.
    if (s->isItFork()) continue;
    
    // Generate PortHole initialization code before the Star is run
    // because running the Star modifies the PortHole indices.
    s->initCode();
  }
  return TRUE;
}

// Method to write out com file for VSS if needed.
void StructTarget :: writeComFile() {
  // Make sure to do the com file uniquely too!!!
  StringList comCode = "";
  comCode << "cd " << (const char*) filePrefix << "\n";
  comCode << "assign 0.0 *'vhdl\n";
  comCode << "run\n";
  comCode << simWrapup;
  comCode << "quit\n";
  writeFile(comCode, ".com", 0);
}

// Add additional codeStreams.
void StructTarget :: addCodeStreams() {
  addStream("entity_declaration", &entity_declaration);
  addStream("architecture_body_opener", &architecture_body_opener);
  addStream("component_declarations", &component_declarations);
  addStream("signal_declarations", &signal_declarations);
  addStream("component_mappings", &component_mappings);
  addStream("architecture_body_closer", &architecture_body_closer);
  addStream("configuration_declaration", &configuration_declaration);
  addStream("preSynch", &preSynch);
  addStream("firingAction", &firingAction);
  addStream("postSynch", &postSynch);
  addStream("ctlerAction", &ctlerAction);

  SimVSSTarget::addCodeStreams();
}

// Initialize codeStreams.
void StructTarget :: initCodeStreams() {
  entity_declaration.initialize();
  architecture_body_opener.initialize();
  component_declarations.initialize();
  signal_declarations.initialize();
  component_mappings.initialize();
  architecture_body_closer.initialize();
  configuration_declaration.initialize();
  preSynch.initialize();
  firingAction.initialize();
  postSynch.initialize();
  ctlerAction.initialize();

  SimVSSTarget::initCodeStreams();
}

// Initialize VHDLObjLists.
void StructTarget :: initVHDLObjLists() {
  systemPortList.initialize();
  ctlerPortList.initialize();
  mainCompDeclList.initialize();
  mainSignalList.initialize();
  mainCompMapList.initialize();
  stateList.initialize();
  clusterList.initialize();

  topSignalList.initialize();

  ctlerGenericList.initialize();
  ctlerPortList.initialize();
  ctlerGenericMapList.initialize();
  ctlerPortMapList.initialize();
  ctlerSignalList.initialize();
  ctlerVariableList.initialize();
  ctlerPortVarList.initialize();
  ctlerVarPortList.initialize();

  SimVSSTarget::initVHDLObjLists();
}

// Initialize firing lists.
void StructTarget :: initFiringLists() {
  firingGenericList.initialize();
  firingPortList.initialize();
  firingGenericMapList.initialize();
  firingPortMapList.initialize();
  firingSignalList.initialize();
  firingVariableList.initialize();
  firingPortVarList.initialize();
  firingVarPortList.initialize();
}
