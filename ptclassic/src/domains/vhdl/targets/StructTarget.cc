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
  // Create a new VHDLFiring to load up.
  VHDLFiring* fi = new VHDLFiring;
  // Set the current firing to point to this firing.
  currentFiring = fi;

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

  fi->setName(fiName);

  // Name is set -- now add the firing to the list of firings
  firingList.put(*fi);

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
    // Add to the signal list.
    nfi->signalList->addList(firingSignalList);
    // Add to the declarations.
    nfi->decls << mainDecls;
    // Add to the local variable declarations.
    nfi->decls << addVariableDecls(&localVariableList);
    // Add to the variable list.
    nfi->variableList->addList(firingVariableList);
    // Add to the action list.
    StringList naction = "";
    naction << preSynch;
    naction << firingAction;
    naction << postSynch;
    nfi->action << naction;
    preSynch.initialize();
    firingAction.initialize();
    postSynch.initialize();
  }

  if (!foundFiring) {
    fi->starClassName = s->className();
    fi->genericList = firingGenericList.newCopy();
    fi->portList = firingPortList.newCopy();
    fi->signalList = firingSignalList.newCopy();
    fi->decls = mainDecls;
    mainDecls.initialize();

    // If there are local/temporary variables, put them into
    // mainDecls here.  Then clear the localVariableList.
    fi->decls << addVariableDecls(&localVariableList);
    localVariableList.initialize();

    fi->variableList = firingVariableList.newCopy();

    StringList action = "";
    action << preSynch;
    action << firingAction;
    action << postSynch;
  
    fi->action = action;

    preSynch.initialize();
    firingAction.initialize();
    postSynch.initialize();

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
      VHDLSignal* stateSignal = topSignalList.vhdlSignalWithName(state->name);
      if (stateSignal) {
	connectSource(state->initVal, stateSignal);
      }
      else {
	Error::abortRun(state->name,
			": no such signal with this name in topSignalList");
	return;
      }
    }
    // If firings change state, need to connect a reg and a mux
    // between the lastRef to the state and the firstRef to the state.
    if (!(state->constant)) {
      StringList initName = state->name;
      initName << "_Init";

      VHDLSignal* initSignal = new VHDLSignal;
      initSignal->setName(initName);
      initSignal->setType(state->type);

      mainSignalList.put(*initSignal);

      VHDLSignal* lastRefSignal =
	topSignalList.vhdlSignalWithName(state->lastRef);
      VHDLSignal* firstRefSignal =
	topSignalList.vhdlSignalWithName(state->firstRef);
      if (!lastRefSignal) {
	Error::abortRun(state->lastRef, ": no such signal in topSignalList");
	return;
      }
      else if (!firstRefSignal) {
	Error::abortRun(state->firstRef, ": no such signal in topSignalList");
	return;
      }
      else {
	connectMultiplexor(lastRefSignal, firstRefSignal, initSignal);
	connectSource(state->initVal, initSignal);
      }
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

      VHDLSignal* sourceSignal = new VHDLSignal;
      sourceSignal->setName(sourceName);
      sourceSignal->setType(arc->getType());
      VHDLSignal* destSignal = new VHDLSignal;
      destSignal->setName(destName);
      destSignal->setType(arc->getType());
      VHDLSignal* clockSignal = new VHDLSignal;
      clockSignal->setName("feedback_clock");
      clockSignal->setType("boolean");

      // sourceName is input to register, destName is output of register.
      connectRegister(sourceSignal, destSignal, clockSignal);
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
	  poMap = fi->portList->vhdlPortWithName(poName);
	  if (!poMap) {
	    Error::abortRun(poName, ": port not in firing's portList.");
	    return;
	  }
	  oldMapping = poMap->mapping;
	  newMapping = oldMapping;
	  newMapping << "_new";
	  poMap->mapping = newMapping;
	  fi->signalList->put(newMapping, po->getType());

	  VHDLSignal* oldSignal = topSignalList.vhdlSignalWithName(oldMapping);
	  VHDLSignal* newSignal = new VHDLSignal;
	  newSignal->setName(newMapping);
	  newSignal->setType(po->getType());
	  VHDLSignal* clkSignal = topSignalList.vhdlSignalWithName(clkName);
	  if (!clkSignal) {
	    clkSignal = new VHDLSignal;
	    clkSignal->setName(clkName);
	    clkSignal->setType("boolean");
	    topSignalList.put(*clkSignal);
	  }

	  oldSignal->disconnect();
	  connectRegister(newSignal, oldSignal, clkSignal);
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
  ctlerFi->signalList = ctlerSignalList.newCopy();
  ctlerFi->variableList = ctlerVariableList.newCopy();
  ctlerFi->action = ctlerAction;
  ctlerAction.initialize();
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
  component_declarations << addComponentDeclarations(&mainCompDeclList, level);
  signal_declarations << addSignalDeclarations(&mainSignalList, level);
  component_mappings << addComponentMappings(&mainCompDeclList, level);
  buildArchitectureBodyCloser(level);
  buildConfigurationDeclaration(level);
}

// Combine all sections of code.
void StructTarget :: frameCode() {
  StringList code = headerComment();

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
    newPort->setFire(currentFiring);

    // If it's an input port, find the signal generating the data.
    if (!strcmp(newPort->direction,"IN")) {
      VHDLSignal* mySignal = topSignalList.vhdlSignalWithName(ref);
      // Check for signal already existing.
      if (mySignal) {
	// mySignal exists with ref as name, so connect to it.
	newPort->connect(mySignal);

	// Register a new VHDLDependency.
	VHDLDependency* dep = new VHDLDependency;
	// Port is IN, so source is mySignal's port's firing.
	VHDLPort* sourcePort = mySignal->getSource();
	if(!sourcePort) {
	  Error::abortRun(mySignal->name, "has NULL sourcePort");
	}
	VHDLFiring* sourceFiring = sourcePort->getFire();
	if(!sourceFiring) {
	  Error::abortRun(sourcePort->name, "has NULL sourceFiring");
	}
	dep->source = sourceFiring;

	// Port is IN, so sink is currentFiring.
	dep->sink = currentFiring;

	// Need dep to have unique name before putting into list.
	StringList depName = "";
	depName << sourceFiring->name << "," << currentFiring->name;
	dep->setName(depName);
	dependencyList.put(*dep);

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
	}
	// tokenNum not negative: signal should already exist, but doesn't.
	else {
	  Error::abortRun(ref,
			  ": Need to connect to signal for data which hasn't",
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

    VHDLVariable* inVar = new VHDLVariable;
    inVar->setName(ref);
    inVar->setType(stType);

    VHDLPort* inPort = new VHDLPort;
    inPort->setName(refIn);
    inPort->setType(stType);
    inPort->setDirec("IN");
    inPort->setVar(inVar);

    firingPortList.put(*inPort);

    firingVariableList.put(*inVar);

    if (constState) {
      if (isFirstStateRef) {
	// Create a new signal.
	VHDLSignal* inSignal = new VHDLSignal;
	inSignal->setName(root);
	inSignal->setType(stType);

	topSignalList.put(*inSignal);
	firingSignalList.put(*inSignal);

	inPort->connect(inSignal);
      }
      if (!isFirstStateRef) {
	// Need to find the previous signal to connect to.
	// That signal should have been created during the previous firing's
	// first reference to the state.
	VHDLSignal* inSignal = topSignalList.vhdlSignalWithName(root);
	if (!inSignal) {
	  Error::abortRun(root,
			  ": Not first state ref, ",
			  "but can't find any signal created for it");
	  return;
	}
	inPort->connect(inSignal);
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

      if (isFirstStateRef) {
	// Create a new signal.
	VHDLSignal* inSignal = new VHDLSignal;
	inSignal->setName(refIn);
	inSignal->setType(stType);

	topSignalList.put(*inSignal);
	firingSignalList.put(*inSignal);

	inPort->connect(inSignal);
      }
      if (!isFirstStateRef) {
	// Need to find the previous signal to connect to
	// FIXME: Still don't like this, depends on knowing the name already!!
	VHDLSignal* inSignal = topSignalList.vhdlSignalWithName(state_in);
	if (!inSignal) {
	  Error::abortRun(state_in,
			  ": Not first state ref, ",
			  " but can't find any signal created for it");
	  return;
	}

	inPort->connect(inSignal);
      }

      topSignalList.put(*outSignal);
      firingSignalList.put(*outSignal);

      VHDLVariable* outVar = new VHDLVariable;
      outVar->setName(ref);
      outVar->setType(stType);

      outPort->setVar(outVar);

      firingPortList.put(*outPort);

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
void StructTarget :: connectSource(StringList initVal, VHDLSignal* initSignal)
{
  if (!initSignal) {
    Error::abortRun(*this, "connectSource passed a null initSignal");
    return;
  }

  StringList type = initSignal->getType();

  // Set the flag indicating sources are needed.
  if (!strcmp(type,"INTEGER")) {
    setSourceInts();
  }
  if (!strcmp(type,"REAL")) {
    setSourceReals();
  }

  StringList label = initSignal->name;
  label << "_SOURCE";
  StringList name = "Source";
  name << "_" << type;

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;

  VHDLGeneric* valueGen = new VHDLGeneric("value", type, "", initVal);
  genList->put(*valueGen);

  VHDLPort* outPort = new VHDLPort("output", type, "OUT", "", NULL);
  outPort->connect(initSignal);
  portList->put(*outPort);

  mainCompDeclList.put(label, portList, genList,
		       name, portList, genList);
}

// Connect a multiplexor between the given input and output signals.
void StructTarget :: connectMultiplexor(VHDLSignal* inSignal,
					VHDLSignal* outSignal,
					VHDLSignal* initSignal) {
  if (!inSignal) {
    Error::abortRun(*this, "connectMultiplexor passed a null inSignal");
    return;
  }
  if (!outSignal) {
    Error::abortRun(*this, "connectMultiplexor passed a null outSignal");
    return;
  }
  if (!initSignal) {
    Error::abortRun(*this, "connectMultiplexor passed a null initSignal");
    return;
  }

  if ((strcmp(outSignal->getType(),inSignal->getType())) ||
      (strcmp(outSignal->getType(),initSignal->getType()))) {
    Error::error(outSignal->name,
		 ": connectMultiplexor: Types of signals do not match");
    return;
  }
  StringList type = outSignal->getType();

  assertClock("control");

  // Set the flag indicating multiplexors and sources are needed.
  if (!strcmp(type,"INTEGER")) {
    setMultiplexorInts();
  }
  if (!strcmp(type,"REAL")) {
    setMultiplexorReals();
  }

  StringList label = outSignal->name;
  label << "_MUX";
  StringList name = "Mux";
  name << "_" << type;

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;

  VHDLPort* inPort = new VHDLPort("input", type, "IN", "", NULL);
  VHDLPort* outPort = new VHDLPort("output", type, "OUT", "", NULL);
  VHDLPort* initPort = new VHDLPort("init_val", type, "IN", "", NULL);
  VHDLPort* controlPort = new VHDLPort("control", "boolean", "IN", "", NULL);

  VHDLSignal* controlSignal = ctlerSignalList.vhdlSignalWithName("control");
  if (!controlSignal) {
    controlSignal = new VHDLSignal("control", "boolean", NULL);
    ctlerSignalList.put(*controlSignal);
  }

  inPort->connect(inSignal);
  outPort->connect(outSignal);
  initPort->connect(initSignal);
  controlPort->connect(controlSignal);

  portList->put(*inPort);
  portList->put(*outPort);
  portList->put(*initPort);
  portList->put(*controlPort);

  ctlerPortList.put("control", "boolean", "OUT", "control", NULL);
  ctlerPortList.put("system_clock", "boolean", "IN", "system_clock", NULL);

  // If using a system clock generator, then need a signal.
  if (systemClock()) {
    mainSignalList.put("system_clock", "boolean");
    connectClockGen("system_clock");
  }
  else {
    systemPortList.put("system_clock", "boolean", "IN");
  }
  mainCompDeclList.put(label, portList, genList,
		       name, portList, genList);
}

// Connect a register between the given input and output signals.
void StructTarget :: connectRegister(VHDLSignal* inSignal,
				     VHDLSignal* outSignal,
				     VHDLSignal* clkSignal) {
  if (!inSignal) {
    Error::abortRun(*this, "connectRegister passed a null inSignal");
    return;
  }
  if (!outSignal) {
    Error::abortRun(*this, "connectRegister passed a null outSignal");
    return;
  }
  if (!clkSignal) {
    Error::abortRun(*this, "connectRegister passed a null clkSignal");
    return;
  }

  if (strcmp(outSignal->getType(),inSignal->getType())) {
    Error::error(outSignal->name,
		 ": connectRegister: Types of signals do not match");
    return;
  }
  StringList type = outSignal->getType();

  // Set the flag indicating registers are needed.
  if (!strcmp(type,"INTEGER")) {
    setRegisterInts();
  }
  if (!strcmp(type,"REAL")) {
    setRegisterReals();
  }

  StringList label = outSignal->name;
  label << "_REG";
  StringList name = "Reg";
  name << "_" << type;

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;

  VHDLPort* inPort = new VHDLPort("D", type, "IN", "", NULL);
  VHDLPort* outPort = new VHDLPort("Q", type, "OUT", "", NULL);
  VHDLPort* clkPort = new VHDLPort("C", "boolean", "IN", "", NULL);

  inPort->connect(inSignal);
  outPort->connect(outSignal);
  clkPort->connect(clkSignal);

  portList->put(*inPort);
  portList->put(*outPort);
  portList->put(*clkPort);


  ctlerPortList.put(clkSignal->name, "boolean", "OUT", clkSignal->name, NULL);
  ctlerPortList.put("system_clock", "boolean", "IN", "system_clock", NULL);

  // If using a system clock generator, then need a signal.
  if (systemClock()) {
    mainSignalList.put("system_clock", "boolean");
    connectClockGen("system_clock");
  }
  else {
    systemPortList.put("system_clock", "boolean", "IN");
  }
  mainCompDeclList.put(label, portList, genList,
		       name, portList, genList);
}

// Connect a clock generator driving the given signal.
void StructTarget :: connectClockGen(StringList clkName) {
  // Set the flag indicating clock generator used.
  setSystemClockUsed();

  StringList label = clkName;
  label << "_Clock";
  StringList name = "ClockGen";

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;

  portList->put("system_clock", "boolean", "OUT", clkName, NULL);
  portList->put("iter_clock", "boolean", "IN", "iter_clock", NULL);

  mainCompDeclList.put(label, portList, genList, name, portList, genList);
}

ISA_FUNC(StructTarget,SimVSSTarget);

// Method called by comm stars to place important code into structure.
void StructTarget :: registerComm(int direction, int pairid, int numxfer,
				  const char* dtype) {
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
  
  // Construct unique label and signal names and put comp in main list
  StringList label;
  StringList goName, dataName, doneName;
  StringList rootName = name;
  rootName << pairid;

  label << rootName;
  goName << rootName << "_go";
  dataName << rootName << "_data";
  doneName << rootName << "_done";
  
  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;
  
  genList->put("pairid", "INTEGER", "", pairid);
  genList->put("numxfer", "INTEGER", "", numxfer);

  portList->put("go", "STD_LOGIC", "IN", goName, NULL);
  if (direction) {
    firingPortList.put(dataName, vtype, "OUT", dataName, NULL);
    firingSignalList.put(dataName, vtype, NULL);
    portList->put("data", vtype, "IN", dataName, NULL);
  }
  else {
    portList->put("data", vtype, "OUT", dataName, NULL);
    firingSignalList.put(dataName, vtype, NULL);
    firingPortList.put(dataName, vtype, "IN", dataName, NULL);
  }
  portList->put("done", "STD_LOGIC", "OUT", doneName, NULL);

  mainCompDeclList.put(label, portList, genList, name, portList, genList);

  StringList startName;
  StringList endName;
  startName << rootName << "_start";
  endName << rootName << "_end";

  // Also add to port list of controller.
  ctlerPortList.put(startName, "STD_LOGIC", "OUT", startName, NULL);
  ctlerSignalList.put(startName, "STD_LOGIC", NULL);
  firingSignalList.put(startName, "STD_LOGIC", NULL);
  firingPortList.put(startName, "STD_LOGIC", "IN", startName, NULL);
  firingPortList.put(goName, "STD_LOGIC", "OUT", goName, NULL);
  firingSignalList.put(goName, "STD_LOGIC", NULL);
  firingSignalList.put(doneName, "STD_LOGIC", NULL);
  firingPortList.put(doneName, "STD_LOGIC", "IN", doneName, NULL);
  firingPortList.put(endName, "STD_LOGIC", "OUT", endName, NULL);
  firingSignalList.put(endName, "STD_LOGIC", NULL);
  ctlerSignalList.put(endName, "STD_LOGIC", NULL);
  ctlerPortList.put(endName, "STD_LOGIC", "IN", endName, NULL);

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
    int portCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      VHDLPortListIter nextPort(*(nfiring->portList));
      VHDLPort* nPort;
      while ((nPort = nextPort++) != 0) {
	if (!strcmp(nPort->direction,"IN") && nPort->variable) {
	  body << nPort->variable->name << " := " << nPort->name << ";\n";
	  portCount++;
	}
      }
    }
    
    if (portCount) {
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
    int portCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      VHDLPortListIter nextPort(*(nfiring->portList));
      VHDLPort* nPort;
      while ((nPort = nextPort++) != 0) {
	if (!strcmp(nPort->direction,"OUT") && nPort->variable) {
	  body << nPort->name << " <= " << nPort->variable->name << ";\n";
	  portCount++;
	}
      }
    }
    
    if (portCount) {
      all << body;
    }
  }
  return all;
}

// Register compDecls and merge signals.
void StructTarget :: registerAndMerge(VHDLCluster* cl) {
  StringList clLabel = cl->name;
  clLabel << "_proc";
  StringList clName = cl->name;
  
  // iterate through firings
  // tack all onto master lists
  VHDLPortList* masterPortList = new VHDLPortList;
  VHDLGenericList* masterGenericList = new VHDLGenericList;
  VHDLSignalList* masterSignalList = new VHDLSignalList;

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
  }

  mainCompDeclList.put(clLabel, masterPortList, masterGenericList,
		       clName, masterPortList, masterGenericList);
  mergeSignalList(masterSignalList);
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
  Error::warn("buildComponentDeclarations", ": Don't call me anymore!");

  // HashTable to keep track of which components already declared.
  HashTable myTable;
  myTable.clear();

  VHDLCompDeclListIter nextCompDecl(mainCompDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    if (!(myTable.hasKey(compDecl->type))) {
      myTable.insert(compDecl->type, compDecl);

      level++;
      component_declarations << indent(level) << "component " << compDecl->type
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
  // HashTable to keep track of which components already configured.
  HashTable myTable;
  myTable.clear();

  configuration_declaration << "configuration " << (const char*) filePrefix
			    << "_parts" << " of " << (const char*) filePrefix
			    << " is\n";
  configuration_declaration << "for " << "structure" << "\n";

  configuration_declaration << cli_configs;

  VHDLCompDeclListIter nextCompDecl(mainCompDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    if (!(myTable.hasKey(compDecl->type))) {
      myTable.insert(compDecl->type, compDecl);

      // Filter out the CLI components C2V,V2C integer, real.
      if (!strcmp(compDecl->type,"C2Vreal")) continue;
      if (!strcmp(compDecl->type,"V2Creal")) continue;
      if (!strcmp(compDecl->type,"C2Vinteger")) continue;
      if (!strcmp(compDecl->type,"V2Cinteger")) continue;

      level++;
      configuration_declaration << indent(level) << "for all:"
				<< compDecl->type
				<< " use entity " << "work." << compDecl->type
				<< "(behavior); end for;\n";
      level--;
    }
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
    ctlerPortList.put(clock, "boolean", "OUT", clock, NULL);
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
    ctlerPortList.put(clock, "boolean", "OUT", clock, NULL);
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
  mainCompDeclList.initialize();
  mainSignalList.initialize();
  stateList.initialize();
  clusterList.initialize();

  topSignalList.initialize();

  ctlerGenericList.initialize();
  ctlerPortList.initialize();
  ctlerSignalList.initialize();
  ctlerVariableList.initialize();

  SimVSSTarget::initVHDLObjLists();
}

// Initialize firing lists.
void StructTarget :: initFiringLists() {
  firingGenericList.initialize();
  firingPortList.initialize();
  firingSignalList.initialize();
  firingVariableList.initialize();
}
