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

  // Set the default to display the code.
  analyze.setInitValue("NO");
  // Set the default to not use loop scheduling.
  loopingLevel.setInitValue("0");

  addCodeStreams();
  initCodeStreams();
}

// Clone the Target.
Block* StructTarget :: makeNew() const {
  LOG_NEW; return new StructTarget(name(), starType(), descriptor());
}

static StructTarget proto("struct-VHDL", "CGStar",
			 "VHDL code generation target with structural style");
static KnownTarget entry(proto,"struct-VHDL");

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

  // Don't init code streams, because you lose stuff from init,
  // like the sysWrapup for graphing an xgraph.
  //  initCodeStreams();
  initVHDLObjLists();
  initFiringLists();
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

  // Create a new VHDLFiring to load up.
  VHDLFiring* fi = new VHDLFiring;

  // Begin constructing the components of the firing's code.
  StringList tempName = s->fullName();
  tempName << "_F" << s->firingNum();
  //  StringList sanTempName = sanitize(tempName);
  //  StringList fiName = targetNestedSymbol.push(sanTempName);
  StringList fiName = sanitize(tempName);
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
  printf("Name: %s\n", (const char*) fiName);
  printf("LocalVars?: %d\n", localVariableList.size());
  fi->decls << addVariableDecls(&localVariableList);
  localVariableList.initialize();

  fi->variableList = firingVariableList.newCopy();
  fi->portVarList = firingPortVarList.newCopy();
  fi->action = firingAction;
  firingAction.initialize();
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

  // Vestigial code - see original for reasoning behind this, then change it.
  if (!status) {
    return status;
  }
  if (s->isItFork()) {
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
      mainSignalList.put(state->name, state->type, "", "");
      connectSource(state->initVal, state->name, state->type);
    }
    // If firings change state, need to connect a reg and a mux
    // between the lastRef to the state and the firstRef to the state.
    if (!(state->constant)) {
      StringList tempName = state->name;
      tempName << "_Temp";
      mainSignalList.put(tempName, state->type, "", "");
      StringList initName = state->name;
      initName << "_Init";

      mainSignalList.put(initName, state->type, "", "");
      //      connectRegister(state->lastRef, tempName, "feedback_clock", state->type);
      //      connectMultiplexor(tempName, state->firstRef, initName, state->type);
      connectMultiplexor(state->lastRef, state->firstRef, initName, state->type);
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

      // NEW: Even though these are "source" and "dest", the naming
      // convention is that any latched output of an exu is being
      // called "sink" (for now). 4/12/96.
      sourceName << "_sink";
      destName << "_sink";

      // sourceName is input to register, destName is output of register.
      //      connectRegister(sourceName, destName, "feedback_clock", "INTEGER");
      connectRegister(sourceName, destName, "feedback_clock", arc->type);

      // Must also create signals for those lines which are neither read nor
      // written by a $ref() - e.g. if more delays than tokens read.
      // However, do not create a signal if it's a wormhole input.
      // Will have created a system port input instead.

      // If no system port by the given name, go ahead and make the signal.
      if (sx < arc->lowWrite) {
	//	mainSignalList.put(sourceName, "INTEGER", "", "");
	mainSignalList.put(sourceName, arc->type, "", "");
      }
      if (ix < arc->lowWrite) {
	//	mainSignalList.put(destName, "INTEGER", "", "");
	mainSignalList.put(destName, arc->type, "", "");
      }
    }
  }

  // Add in the entity, architecture, entity declaration, and
  // component mapping for the controller.
  VHDLFiring* ctlerFi = new VHDLFiring;
  ctlerFi->setName("controller");
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
    myCode << "\n\t-- Cluster " << cl->name << "\n";
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

  // after generating code, initialize codeStreams again.
  //  initCodeStreams();
}

// Write the code to a file.
void StructTarget :: writeCode() {
  /*
  ////////////////////////////////////////////
  // Print out the list of clock names.
  printf("CLOCK FIRINGS IN ORDER:\n\n");
  StringListIter nextClock(clockList);
  const char* clockName;
  while ((clockName = nextClock++) != 0) {
    printf("ASSERT:  %s\n", clockName);
  }

  StringList ctlerCode = "";
  ctlerCode << "entity CONTROLLER is\n";
  ctlerCode << "  port\n";
  ctlerCode << "  (\n";
  int portCount = 0;
  nextClock.reset();
  while ((clockName = nextClock++) != 0) {
    if (portCount) {
      ctlerCode << ";\n";
    }
    ctlerCode << "    " << clockName << ": OUT BOOLEAN";
    portCount++;
  }
  ctlerCode << "\n";
  ctlerCode << "  );\n";
  ctlerCode << "end CONTROLLER;\n";
  ctlerCode << "\n";
  ctlerCode << "architecture SEQUENCE of CONTROLLER is\n";
  ctlerCode << "begin\n";
  ctlerCode << "process\n";
  ctlerCode << "begin\n";
  nextClock.reset();
  while ((clockName = nextClock++) != 0) {
    ctlerCode << "  " << clockName << " <= '1';\n";
  }
  ctlerCode << "end process;\n";
  ctlerCode << "end SEQUENCE;\n";

  printf("Sequencer Code:\n");
  printf("%s", (const char*) ctlerCode);
  ////////////////////////////////////////////
  */

  SimVSSTarget::writeCode();
  /*
  writeFile(myCode,".vhdl",displayFlag);
  if (writeCom) {
    writeComFile();
  }
  singleUnderscore();
  */
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
  VHDLStateListIter nextState(stateList);
  VHDLState* nState;
  VHDLState* listState = new VHDLState;
  while ((nState = nextState++) != 0) {
    StringList tempRoot = root;
    StringList tempName = nState->name;
    if (!strcmp(tempRoot,tempName)) {
      stateLastFiring = nState->lastFiring;
      listState = nState;
    }
  }
  
  int isFirstStateRef = (stateLastFiring == -1);
  
  // If this is the first reference to this state in this firing
  // need to put it in the proper lists and make connections.
  if (listState->lastFiring != thisFiring) {

    int constState = 0;
    
    // If state is constant, according to its attributes, do things
    // a bit differently: have all references refer only to the single
    // constant source, rather than passing through values from one firing
    // to the next, which is useless and restrictive.

    if (state->attributes() & AB_CONST) {
      // if constant, the ref should be to the main constant value,
      // not to the last exu which reffed the value, but can't have
      // changed it.
      // Also, there should be no output on which to pass on the value,
      // since it isn't changed by this exu.
      constState = 1;
      listState->constant = 1;
    }
    else {
      listState->constant = 0;
    }
    
    // Contstruct the name of the last ref to the state.
    StringList stateSignal = root;
    stateSignal << "_F" << listState->lastFiring << "_Out";
    StringList temp_state = stateSignal;
    temp_state << "_sink";
    StringList temp_in = refIn;
    temp_in << "_sink";
    StringList temp_out = refOut;
    temp_out << "_source";
    StringList temp_out_reg = refOut;
    temp_out_reg << "_sink";

    // If it's the first firing to refer to this state,
    if (isFirstStateRef) {
      if (constState) {
	firingSignalList.put(root, stType, "", root);
	firingPortMapList.put(refIn, root);
      }
      if (!(constState)) {
	firingSignalList.put(refIn, stType, "", refIn);
	firingPortMapList.put(refIn, refIn);
	firingPortMapList.put(refOut, temp_out);
      }
    }
    
    // If this isn't the first firing to refer to this state,
    if (!isFirstStateRef) {
      if (constState) {
	firingSignalList.put(root, stType, root, root);
	firingPortMapList.put(refIn, root);
      }
      if (!(constState)) {
	//	firingSignalList.put(stateSignal, stType, stateSignal, refIn);
	//	firingSignalList.put(stateSignal, stType, stateSignal, refIn);
	//	firingPortMapList.put(refIn, stateSignal);
	//	firingPortMapList.put(refOut, refOut);
	firingSignalList.put(temp_state, stType, temp_state, refIn);
	firingPortMapList.put(refIn, temp_state);
	firingPortMapList.put(refOut, temp_out);
      }
    }

    if (constState) {
      firingVariableList.put(ref, stType, "");
      firingPortVarList.put(refIn, ref);
      firingPortList.put(refIn, "IN", stType);
    }
    if (!(constState)) {
      //      firingSignalList.put(refOut, stType, refOut, "");
      //      firingVariableList.put(ref, stType, "");
      //      firingPortVarList.put(refIn, ref);
      //      firingVarPortList.put(refOut, ref);
      //      firingPortList.put(refIn, "IN", stType);
      //      firingPortList.put(refOut, "OUT", stType);
      firingSignalList.put(temp_out, stType, temp_out, "");
      firingVariableList.put(ref, stType, "");
      firingPortVarList.put(refIn, ref);
      firingVarPortList.put(refOut, ref);
      firingPortList.put(refIn, "IN", stType);
      firingPortList.put(refOut, "OUT", stType);

      // Data clock name.  Needs to be the name of the firing, not the star.
      StringList clockName = sanitize(state->parent()->fullName());
      clockName << "_F" << thisFiring;
      clockName << "_clk";
      // Create a register for data value.
      connectRegister(temp_out, temp_out_reg, clockName, stType);
      firingSignalList.put(temp_out_reg, stType, temp_out_reg, "");
 
    }

    // Reset state's lastRef name.
    //  listState->lastRef = refOut;
    //    listState->lastRef = temp_out;
    listState->lastRef = temp_out_reg;

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

      VHDLGenericMapList* genMapList = new VHDLGenericMapList;
      VHDLPortMapList* portMapList = new VHDLPortMapList;
      genMapList->initialize();
      portMapList->initialize();
      
      genMapList->put("value", initVal);
      portMapList->put("output", initName);
      mainCompMapList.put(label, name, portMapList, genMapList);
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

  mainCompDeclList.put(name, portList, genList);
}

// Connect a multiplexor between the given input and output signals.
void StructTarget :: connectMultiplexor(StringList inName, StringList outName,
					StringList initVal, StringList type) {
  // Add the clock to the list of clocks to be triggered.
  const char* clock = "control";
  if (clockList.tail()) {
    if (strcmp(clockList.tail(),clock)) {
      clockList << clock;
      ctlerAction << "     -- Assert " << clock << "\n";
      ctlerAction << "wait until "
		  << "system_clock'event and system_clock = TRUE;\n";
      ctlerAction << clock << " <= TRUE;\n";
    }
  }
  else {
    clockList << clock;
    ctlerAction << "     -- Assert " << clock << "\n";
    ctlerAction << "wait until "
		<< "system_clock'event and system_clock = TRUE;\n";
    ctlerAction << clock << " <= TRUE;\n";
  }

  registerMultiplexor(type);
  StringList label = outName;
  label << "_MUX";
  StringList name = "Mux";
  name << "_" << type;

  VHDLGenericMapList* genMapList = new VHDLGenericMapList;
  VHDLPortMapList* portMapList = new VHDLPortMapList;
  genMapList->initialize();
  portMapList->initialize();
      
  portMapList->put("input", inName);
  portMapList->put("output", outName);
  portMapList->put("init_val", initVal);
  portMapList->put("control", "control");
  //  systemPortList.put("control", "IN", "boolean");
  ctlerPortList.put("control", "OUT", "boolean");
  ctlerPortMapList.put("control", "control");
  ctlerSignalList.put("control", "boolean", "", "");
  ctlerPortList.put("system_clock", "IN", "boolean");
  ctlerPortMapList.put("system_clock", "system_clock");
  // If using a system clock generator, then need a signal.
  if (systemClock()) {
    mainSignalList.put("system_clock", "boolean", "", "");
    connectClockGen("system_clock");
  }
  else {
    systemPortList.put("system_clock", "IN", "boolean");
  }
  mainCompMapList.put(label, name, portMapList, genMapList);
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

  mainCompDeclList.put(name, portList, genList);
}

// Connect a register between the given input and output signals.
void StructTarget :: connectRegister(StringList inName, StringList outName,
				     StringList clkName, StringList type) {
  // Add the clock to the list of clocks to be triggered.
  addClock(clkName);

  registerRegister(type);
  StringList label = outName;
  label << "_REG";
  StringList name = "Reg";
  name << "_" << type;

  VHDLGenericMapList* genMapList = new VHDLGenericMapList;
  VHDLPortMapList* portMapList = new VHDLPortMapList;
  genMapList->initialize();
  portMapList->initialize();
      
  portMapList->put("D", inName);
  portMapList->put("Q", outName);
  portMapList->put("C", clkName);
  //  systemPortList.put(clkName, "IN", "boolean");
  ctlerPortList.put(clkName, "OUT", "boolean");
  ctlerPortMapList.put(clkName, clkName);
  ctlerSignalList.put(clkName, "boolean", "", "");
  ctlerPortList.put("system_clock", "IN", "boolean");
  ctlerPortMapList.put("system_clock", "system_clock");
  // If using a system clock generator, then need a signal.
  if (systemClock()) {
    mainSignalList.put("system_clock", "boolean", "", "");
    connectClockGen("system_clock");
  }
  else {
    systemPortList.put("system_clock", "IN", "boolean");
  }
  mainCompMapList.put(label, name, portMapList, genMapList);
}

// Connect a clock generator driving the given signal.
void StructTarget :: connectClockGen(StringList clkName) {
      registerClockGen();
      StringList label = clkName;
      label << "_Clock";
      StringList name = "ClockGen";

      VHDLGenericMapList* genMapList = new VHDLGenericMapList;
      VHDLPortMapList* portMapList = new VHDLPortMapList;
      genMapList->initialize();
      portMapList->initialize();
      
      portMapList->put("system_clock", clkName);
      portMapList->put("iter_clock", "iter_clock");
      mainCompMapList.put(label, name, portMapList, genMapList);
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
  mainCompDeclList.put(name, portList, genList);
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
  mainCompDeclList.put(name, portList, genList);
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

  // NEW:  If the port reference is to be latched, then outputs will
  // refer to signals that are inputs to registers, and inputs will
  // refer to signals that are outputs of registers.  Additional names
  // are required for this.  An additional register is needed.  The
  // register needs its own clock signal also, keyed off of when the
  // exu that feeds it completes its execution (outputs are stable).

  // The registry keeps track of all refed arcs, and their min/max R/W offsets.
  registerArcRef(port, tokenNum);

  StringList ref = dataName;
  ref << part;

  // Data source signal name.
  StringList sourceName = ref;
  sourceName << "_source";
  // Data sink signal name.
  StringList sinkName = ref;
  sinkName << "_sink";
  // Data clock name.  Needs to be the name of the firing, not the star.
  StringList clockName = sanitize(port->parent()->fullName());
  clockName << "_F" << firing;
  clockName << "_clk";

  // Create a port and a port mapping for this firing entity.
  firingPortList.put(ref, port->direction(), port->dataType());
  if (port->isItInput()) {
    firingPortMapList.put(ref, sinkName);
  }
  else {
    // Create a register for data value.
    connectRegister(sourceName, sinkName, clockName, port->dataType());
    firingPortMapList.put(ref, sourceName);
  }

  // For wormhole inputs, create a system port.
  // But for delayed values of wormhole inputs, do not create another port.
  // For local inputs, create a signal.
  if ((port->atBoundary()) && (tokenNum >= 0)) {
    // NEW: Signal an error: we won't support wormholes for now:
    Error::error(*port, "is at a wormhole boundary: Not currently supported");
    // Port at wormhole boundary, so create a system port instead of a signal.
    //    systemPortList.put(ref, port->direction(), port->dataType());
  }
  else {
    //    firingSignalList.put(ref, port->dataType(), ref, ref);
    firingSignalList.put(sourceName, port->dataType(), ref, ref);
    firingSignalList.put(sinkName, port->dataType(), ref, ref);
    //    firingSignalList.put(clockName, "boolean", ref, ref);
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
      if (!strcmp(nfiring->name,"controller")) continue;
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
      if (!strcmp(nfiring->name,"controller")) continue;
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
    //    StringList body;

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    //    int varCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      all << addVariableDecls(nfiring->variableList);

      /*
      VHDLVariableListIter nextVar(*(nfiring->variableList));
      VHDLVariable* nvar;
      while ((nvar = nextVar++) != 0) {
	level++;
	body << indent(level) << "variable " << nvar->name << ": "
	     << nvar->type;
	if ((nvar->initVal).length() > 0) {
	  body << " := " << nvar->initVal;
	}
	body << ";\n";
	varCount++;
	level--;
      }
      */
    }
    
    /*Don't use varcount anymore
    if (varCount) {
      all << body;
    }
    */
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
      body << nfiring->action;
      actionCount++;
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
  VHDLPortMapList* masterPortMapList = new VHDLPortMapList;
  VHDLGenericMapList* masterGenericMapList = new VHDLGenericMapList;

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
    VHDLPortMapListIter nextPortMap(*(fi->portMapList));
    VHDLPortMap* pm;
    while ((pm = nextPortMap++) != 0) {
      VHDLPortMap* newPortMap = new VHDLPortMap;
      newPortMap = pm->newCopy();
      masterPortMapList->put(*newPortMap);
    }
    VHDLGenericMapListIter nextGenericMap(*(fi->genericMapList));
    VHDLGenericMap* gm;
    while ((gm = nextGenericMap++) != 0) {
      VHDLGenericMap* newGenericMap = new VHDLGenericMap;
      newGenericMap = gm->newCopy();
      masterGenericMapList->put(*newGenericMap);
    }
  }

  mainCompDeclList.put(clName, masterPortList, masterGenericList);
  mergeSignalList(masterSignalList);
  mainCompMapList.put(clLabel, clName, masterPortMapList, masterGenericMapList);
}

// Generate the entity_declaration.
void StructTarget :: buildEntityDeclaration(int level) {
  entity_declaration << "entity " << galaxy()->name() << " is\n";
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
  entity_declaration << "end " << galaxy()->name() << ";\n";
}

// Generate the architecture_body_opener.
void StructTarget :: buildArchitectureBodyOpener(int /*level*/) {
  architecture_body_opener << "architecture " << "structure" << " of "
			   << galaxy()->name() << " is\n";
}

// Add in component declarations here from mainCompDeclList.
void StructTarget :: buildComponentDeclarations(int level) {
  VHDLCompDeclListIter nextCompDecl(mainCompDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
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
	if (strlen(ngen->defaultVal) > 0) {
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

// Generate the architecture_body_closer.
void StructTarget :: buildArchitectureBodyCloser(int /*level*/) {
  architecture_body_closer << "end structure;\n";
}

// Add in configuration declaration here from mainCompDeclList.
void StructTarget :: buildConfigurationDeclaration(int level) {
  configuration_declaration << "configuration " << galaxy()->name() << "_parts"
			    << " of " << galaxy()->name() << " is\n";
  configuration_declaration << "for " << "structure" << "\n";

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
  configuration_declaration << "end " << galaxy()->name() << "_parts" << ";\n";
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
  addClock("iter_clock");
  ctlerPortList.put("iter_clock", "OUT", "boolean");
  ctlerPortMapList.put("iter_clock", "iter_clock");
  ctlerSignalList.put("iter_clock", "boolean", "", "");
}

// Add the clock to the clock list and generate code to toggle it.
void StructTarget :: addClock(const char* clock) {
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
  }
  /*
  if (clockList.tail()) {
    if (strcmp(clockList.tail(),clock)) {
      clockList << clock;
      ctlerAction << "     -- Toggle " << clock << "\n";
      ctlerAction << "wait until "
		  << "system_clock'event and system_clock = TRUE;\n";
      ctlerAction << clock << " <= TRUE;\n";
      ctlerAction << "wait until "
		  << "system_clock'event and system_clock = TRUE;\n";
      ctlerAction << clock << " <= FALSE;\n";
    }
  }
  else {
    clockList << clock;
    ctlerAction << "     -- Toggle " << clock << "\n";
    ctlerAction << "wait until "
		<< "system_clock'event and system_clock = TRUE;\n";
    ctlerAction << clock << " <= TRUE;\n";
    ctlerAction << "wait until "
		<< "system_clock'event and system_clock = TRUE;\n";
    ctlerAction << clock << " <= FALSE;\n";
  }
  */
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

// Assign names for each geodesic according to port connections.
void StructTarget :: setGeoNames(Galaxy& galaxy) {
  GalStarIter nextStar(galaxy);
  Star* s;
  while ((s = nextStar++) != NULL) {
    BlockPortIter nextPort(*s);
    VHDLPortHole* p;
    while ((p = (VHDLPortHole*) nextPort++) != NULL) {
      if (p->isItInput()) {
	// Create temporary StringLists; allows safe (const char*) casts.
	StringList sl = sanitizedFullName(*p);
	p->setGeoName(savestring(sl));
      }
    }
  }
  nextStar.reset();
  while ((s = nextStar++) != NULL) {
    BlockPortIter nextPort(*s);
    VHDLPortHole* p;
    while ((p = (VHDLPortHole*) nextPort++) != NULL) {
      if (p->isItOutput()) {
	// Create temporary StringLists; allows safe (const char*) casts.
	StringList sl = sanitizedFullName(*p);
	p->setGeoName(savestring(sl));
      }
    }
  }
}
  
// The only reason for redefining this from HLLTarget
// is to change the separator from "." to "_".
StringList StructTarget :: sanitizedFullName (const NamedObj& obj) const {
  StringList out;
  if(obj.parent() != NULL) {
    Block* b = obj.parent();
    if (b->isItWormhole() == 0) {
      out = sanitizedFullName(*obj.parent());
      out += "_";
    }
    out += sanitizedName(obj);
  }
  else {
    out = sanitizedName(obj);
  }
  return out;
}

// Method to write out com file for VSS if needed.
void StructTarget :: writeComFile() {
  // Make sure to do the com file uniquely too!!!
  StringList comCode = "";
  comCode << "cd " << galaxy()->name() << "\n";
  comCode << "assign 0.0 *'vhdl\n";
  comCode << "run\n";
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
  addStream("firingAction", &firingAction);
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
  firingAction.initialize();
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
