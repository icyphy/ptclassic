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
//#include <ostream.h>

// Constructor.
StructTarget :: StructTarget(const char* name,const char* starclass,
			 const char* desc) :
VHDLTarget(name,starclass,desc) {
  regsUsed = 0;
  selsUsed = 0;
  sorsUsed = 0;
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
  HLLTarget::setup();
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
  StringList sanTempName = sanitize(tempName);
  fi->name = targetNestedSymbol.push(sanTempName);
  fi->starClassName = s->className();
  fi->genericList = firingGenericList.newCopy();
  fi->portList = firingPortList.newCopy();
  fi->genericMapList = firingGenericMapList.newCopy();
  fi->portMapList = firingPortMapList.newCopy();
  fi->signalList = firingSignalList.newCopy();
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
  cl->name = fi->name;
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
  masterC->name = "MasterCluster";
  masterC->firingList = masterFL;

  // Set main cluster list to this one cluster.
  clusterList.initialize();
  clusterList.put(*masterC);
}

// Trailer code (done last).
void StructTarget :: trailerCode() {

  // Special Action: merge all firings into one cluster
//  allFiringsOneCluster();

  int level = 0;
  // Iterate through the cluster list.
  // Generate entity, architecture for each firing.
  VHDLClusterListIter nextCluster(clusterList);
  VHDLCluster* cl;
//  int clCount = 0;
  while ((cl = nextCluster++) != 0) {
//    clCount++;
//    cout << "Cluster " << clCount <<":  " << cl->name << "\n";

    // Begin constructing the cluster's code in myCode.

    myCode << "\n\t-- Cluster " << cl->name << "\n";
    myCode << "entity " << cl->name << " is\n";
/*
    myCode << "\n\t-- Cluster " << cl->name;
    myCode << " (class " << cl->starClassName << ") \n";
    myCode << "entity " << cl->name << " is\n";
    */

    addGenericRefs(cl, level);
    addPortRefs(cl, level);

    myCode << indent(level) << "end " << cl->name << ";\n";
    myCode << "\n";
    myCode << "architecture " << "behavior" << " of "
	   << cl->name << " is\n";
    myCode << "begin\n";
    myCode << "process\n";

    addSensitivities(cl, level);
    addVariableRefs(cl, level);

    myCode << "begin\n";

    addPortVarTransfers(cl, level);
    addActions(cl, level);
    addVarPortTransfers(cl, level);

    myCode << "end process;\n";
    myCode << "end behavior;\n";
    
/*
    StringList clLabel = cl->name;
    clLabel << "_proc";
    StringList clName = cl->name;
    */
    
    registerAndMerge(cl);

/*
    registerCompDecl(clName, cl->portList, cl->genericList);
    mergeSignalList(cl->signalList);
    registerCompMap(clLabel, clName, cl->portMapList, cl->genericMapList);
    */
  }
  
  // Iterate through the state list and connect registers and
  // initial value selectors for each referenced state.
  VHDLStateListIter nextState(stateList);
  VHDLState* state;
  while ((state = nextState++) != 0) {
    // If its a constant source, need to do things a bit differently:
    // only have a source with a signal, and no mux or reg.
    if (state->constant) {
      signalList.put(state->name, state->type, "", "");
      connectSource(state->initVal, state->name);
    }
    // If firings change state, need to connect a reg and a mux
    // between the lastRef to the state and the firstRef to the state.
    if (!(state->constant)) {
      StringList tempName = state->name;
      tempName << "_Temp";
      signalList.put(tempName, state->type, "", "");
      StringList initName = state->name;
      initName << "_Init";

      signalList.put(initName, state->type, "", "");
      connectRegister(state->lastRef, tempName, state->type);
      connectSelector(tempName, state->firstRef, initName, state->type);
      connectSource(state->initVal, initName);
    }
  }

  // Iterate through the arc list.
  // Track the read/write references made on each arc.
//  cout << "#############################\n";
  VHDLArcListIter nextArc(arcList);
  VHDLArc* arc;
  while ((arc = nextArc++) != 0) {
//    cout << "\n";
//    cout << "Name:  " << arc->name << "\n";
//    cout << "    lowWrite:   " << arc->lowWrite << "\n";
//    cout << "    highWrite:  " << arc->highWrite << "\n";
//    cout << "    lowRead:    " << arc->lowRead << "\n";
//    cout << "    highRead:   " << arc->highRead << "\n";

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
      
      // sourceName is input to register, destName is output of register.
      connectRegister(sourceName, destName, "INTEGER");
//      cout << "Connecting " << sourceName << " and " << destName << "\n";

      // Must also create signals for those lines which are neither read nor
      // written by a $ref() - e.g. if more delays than tokens read.
      // However, do not create a signal if it's a wormhole input.
      // Will have created a system port input instead.

      // If no system port by the given name, go ahead and make the signal.
      if (sx < arc->lowWrite) {
	signalList.put(sourceName, "INTEGER", "", "");
      }
      if (ix < arc->lowWrite) {
	signalList.put(destName, "INTEGER", "", "");
      }
    }
  }

  buildEntityDeclaration(level);
  buildArchitectureBodyOpener(level);
  buildComponentDeclarations(level);
  buildSignalDeclarations(level);
  buildComponentMappings(level);
  buildArchitectureBodyCloser(level);
  buildConfigurationDeclaration(level);
}

// Combine all sections of code.
void StructTarget :: frameCode() {
  StringList code = headerComment();

  if (registers()) {
    myCode << regCode();
  }
  if (selectors()) {
    myCode << selCode();
  }
  if (sources()) {
    myCode << sourceCode();
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
  initCodeStreams();
}

// Write the code to a file.
void StructTarget :: writeCode() {
  writeFile(myCode,".vhdl",displayFlag);
}

// Compile the code.
int StructTarget :: compileCode() {
  // Return TRUE indicating success.
  return TRUE;
}

// Run the code.
int StructTarget :: runCode() {
  // Return TRUE indicating success.
  return TRUE;
}

// Register component declaration.
void StructTarget :: registerCompDecl(StringList name, VHDLPortList* portList,
				      VHDLGenericList* genList) {
  if (compDeclList.inList(name)) return;
  
  // Allocate memory for a new VHDLCompDecl and put it in the list.
  VHDLCompDecl* newCompDecl = new VHDLCompDecl;
  newCompDecl->name = name;
  newCompDecl->genList = genList;
  newCompDecl->portList = portList;
  compDeclList.put(*newCompDecl);
}

// Merge the Star's signal list with the Target's signal list.
void StructTarget :: mergeSignalList(VHDLSignalList* starSignalList) {
  VHDLSignalListIter starSignalNext(*starSignalList);
  VHDLSignal* nStarSignal;
  // Scan through the list of signals from the star firing.
  while ((nStarSignal = starSignalNext++) != 0) {
    // Search for a match from the existing list.
    if (!(signalList.inList(nStarSignal))) {
      // Allocate memory for a new VHDLSignal and put it in the list.
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal = nStarSignal->newCopy();
      signalList.put(*newSignal);
    }
  }
}

// Register component mapping.
void StructTarget :: registerCompMap(StringList name, StringList type,
				     VHDLPortMapList* portMapList,
				     VHDLGenericMapList* genMapList) {
  // Allocate memory for a new VHDLCompMap and put it in the list.
  VHDLCompMap* newCompMap = new VHDLCompMap;
  newCompMap->name = name;
  newCompMap->type = type;
  newCompMap->genMapList = genMapList;
  newCompMap->portMapList = portMapList;
  compMapList.put(*newCompMap);
}

// Register the State reference.
void StructTarget :: registerState(State* state, const char* varName,
				   int thisFiring/*=-1*/, int pos/*=-1*/) {
//  StringList temp = sanitizedFullName(*state);
//  StringList ref = sanitize(temp);
  StringList ref = varName;
  StringList stType = stateType(state);
  StringList root;
  StringList initVal;
  
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

//  ref << "_" << thisFiring;

  StringList refIn = sanitize(ref);
  refIn << "_" << thisFiring << "_In";
  StringList refOut = sanitize(ref);
  refOut << "_" << thisFiring << "_Out";
  StringList reg = sanitize(root);
  reg << "_Reg";

  // look for state in stateList
  // if don't find it, enter it with -1 as state->lastFiring
  if (!(stateList.inList(root))) {
    // Allocate memory for a new VHDLState and put it in the list.
    VHDLState* newState = new VHDLState;
    newState->name = root;
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
//      cout << "State " << ref << " is constant.\n";
      constState = 1;
      listState->constant = 1;
    }
    else {
      listState->constant = 0;
    }
    
    // If it's the first firing to refer to this state,
    if (isFirstStateRef) {
      if (constState) {
	firingSignalList.put(root, stType, "", root);
	firingPortMapList.put(root, root);
      }
      if (!(constState)) {
	firingSignalList.put(refIn, stType, "", refIn);
	firingPortMapList.put(refIn, refIn);
	firingPortMapList.put(refOut, refOut);
      }
    }
    
    // If this isn't the first firing to refer to this state,
    if (!isFirstStateRef) {
      // Contstruct the name of the last ref to the state.
      StringList stateSignal = root;
      stateSignal << "_" << listState->lastFiring << "_Out";
      if (constState) {
	firingSignalList.put(root, stType, root, root);
	firingPortMapList.put(root, root);
      }
      if (!(constState)) {
	firingSignalList.put(stateSignal, stType, stateSignal, refIn);
	firingPortMapList.put(refIn, stateSignal);
	firingPortMapList.put(refOut, refOut);
      }
    }

    if (constState) {
      firingVariableList.put(ref, stType, "");
      firingPortVarList.put(root, ref);
      firingPortList.put(root, "IN", stType);
    }
    if (!(constState)) {
      firingSignalList.put(refOut, stType, refOut, "");
      firingVariableList.put(ref, stType, "");
      firingPortVarList.put(refIn, ref);
      firingVarPortList.put(refOut, ref);
      firingPortList.put(refIn, "IN", stType);
      firingPortList.put(refOut, "OUT", stType);
    }
  }

  // Reset state's lastRef name.
  listState->lastRef = refOut;
  // Reset state's lastFiring tag just before exiting.
  listState->lastFiring = thisFiring;
}

// Connect a source of the given value to the given signal.
void StructTarget :: connectSource(StringList initVal, StringList initName) {
      registerSource("INTEGER");
      StringList label = initName;
      label << "_SOURCE";
      StringList name = "Source";
//      name << "_" << type;
      name << "_" << "INT";

      VHDLGenericMapList* genMapList = new VHDLGenericMapList;
      VHDLPortMapList* portMapList = new VHDLPortMapList;
      genMapList->initialize();
      portMapList->initialize();
      
      genMapList->put("value", initVal);
      portMapList->put("output", initName);
      registerCompMap(label, name, portMapList, genMapList);
}

// Add a source component declaration.
void StructTarget :: registerSource(StringList /*type*/) {
  // Set the flag indicating sources are needed.
  setSources();

  StringList name = "Source";
//  name << "_" << type;
  name << "_" << "INT";

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;
  genList->initialize();
  portList->initialize();
  
//  genList->put("value", type);
//  portList->put("output", "OUT", type);
  genList->put("value", "INTEGER");
  portList->put("output", "OUT", "INTEGER");

  registerCompDecl(name, portList, genList);
}

// Connect a selector between the given input and output signals.
void StructTarget :: connectSelector(StringList inName, StringList outName,
				     StringList initVal, StringList /*type*/) {
      registerSelector("INTEGER");
      StringList label = outName;
      label << "_SEL";
      StringList name = "Sel";
//      name << "_" << type;
      name << "_" << "INT";

      VHDLGenericMapList* genMapList = new VHDLGenericMapList;
      VHDLPortMapList* portMapList = new VHDLPortMapList;
      genMapList->initialize();
      portMapList->initialize();
      
      portMapList->put("input", inName);
      portMapList->put("output", outName);
      portMapList->put("init_val", initVal);
      portMapList->put("control", "control");
      systemPortList.put("control", "IN", "boolean");
      registerCompMap(label, name, portMapList, genMapList);
}

// Add a selector component declaration.
void StructTarget :: registerSelector(StringList /*type*/) {
  // Set the flag indicating selectors and sources are needed.
  setSelectors();

  StringList name = "Sel";
//  name << "_" << type;
  name << "_" << "INT";

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;
  genList->initialize();
  portList->initialize();
  
//  portList->put("init_val", "IN", type);
//  portList->put("input", "IN", type);
//  portList->put("output", "OUT", type);
  portList->put("init_val", "IN", "INTEGER");
  portList->put("input", "IN", "INTEGER");
  portList->put("output", "OUT", "INTEGER");
  portList->put("control", "IN", "boolean");

  registerCompDecl(name, portList, genList);
}

// Connect a register between the given input and output signals.
void StructTarget :: connectRegister(StringList inName, StringList outName,
				     StringList /*type*/) {
      registerRegister("INTEGER");
      StringList label = outName;
      label << "_REG";
      StringList name = "Reg";
//      name << "_" << type;
      name << "_" << "INT";

      VHDLGenericMapList* genMapList = new VHDLGenericMapList;
      VHDLPortMapList* portMapList = new VHDLPortMapList;
      genMapList->initialize();
      portMapList->initialize();
      
      portMapList->put("D", inName);
      portMapList->put("Q", outName);
      portMapList->put("C", "clock");
      systemPortList.put("clock", "IN", "boolean");
      registerCompMap(label, name, portMapList, genMapList);
}

// Add a register component declaration.
void StructTarget :: registerRegister(StringList /*type*/) {
  // Set the flag indicating registers are needed.
  setRegisters();

  StringList name = "Reg";
//  name << "_" << type;
  name << "_" << "INT";

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;
  genList->initialize();
  portList->initialize();
  
//  portList->put("D", "IN", type);
//  portList->put("Q", "OUT", type);
  portList->put("D", "IN", "INTEGER");
  portList->put("Q", "OUT", "INTEGER");
  portList->put("C", "IN", "boolean");
  registerCompDecl(name, portList, genList);
}

// Register PortHole reference.
void StructTarget :: registerPortHole(VHDLPortHole* port, const char* varName,
				      int tokenNum/*=-1*/,
				      const char* part/*=""*/) {
  StringList ref = varName;
  
  // The registry keeps track of all refed arcs, and their min/max R/W offsets.
  registerArcRef(port, tokenNum);

// I want to pass along the offset info as well as whether it's
// a read or a write so I can keep tabs on the production window and
// the read window, and then do nice things at the end based on that.
// Also, must do special things if it's a wormhole input.

// Continue to do normal signal naming and portmapping.

  // Create a port and a port mapping for this firing entity.
/*
  StringList ref = port->getGeoName();
  if (tokenNum >= 0) {
    ref << "_" << tokenNum;
  }
  else { // (tokenNum < 0)
    ref << "_N" << (-tokenNum);
  }
  */
  ref << part;
  
  firingPortList.put(ref, port->direction(), port->dataType());
  firingPortMapList.put(ref, ref);

  // For wormhole inputs, create a system port.
  // But for delayed values of wormhole inputs, do not create another port.
  // For local inputs, create a signal.
  if ((port->atBoundary()) && (tokenNum >= 0)) {
    // Port at wormhole boundary, so create a system port instead of a signal.
    systemPortList.put(ref, port->direction(), port->dataType());
  }
  else {
    firingSignalList.put(ref, port->dataType(), ref, ref);
  }
}

// Return the assignment operator for States.
const char* StructTarget :: stateAssign() {
  const char* assign = ":=";
  return assign;
}

// Return the assignment operator for PortHoles.
const char* StructTarget :: portAssign() {
  const char* assign = "<=";
  return assign;
}

ISA_FUNC(StructTarget,VHDLTarget);

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

// Add in generic refs here from genericList.
void StructTarget :: addGenericRefs(VHDLCluster* cl, int level) {
  if ((*(cl->firingList)).head()) {
    StringList opener, body, closer;

    level++;
    opener << indent(level) << "generic(\n";

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int genCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      if ((*(nfiring->genericList)).head()) {
	VHDLGenericListIter nextGeneric(*(nfiring->genericList));
	VHDLGeneric* ngen;
	while ((ngen = nextGeneric++) != 0) {
	  level++;
	  if (genCount) {
	    body << ";\n";
	  }
	  body << indent(level) << ngen->name << ": " << ngen->type;
//	  if ((ngen->defaultVal).numPieces() > 0) {
	  if (strlen(ngen->defaultVal) > 0) {
	    body << " := " << ngen->defaultVal;
	  }
	  genCount++;
	  level--;
	}
      }
    }
    
    closer << "\n";
    closer << indent(level) << ");\n";
    level--;

    if (genCount) {
      myCode << opener << body << closer;
    }
  }
}

// Add in port refs here from portList.
void StructTarget :: addPortRefs(VHDLCluster* cl, int level) {
  if ((*(cl->firingList)).head()) {
    StringList opener, body, closer;

    level++;
    opener << indent(level) << "port(\n";

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int portCount = 0;
    while ((nfiring = nextFiring++) != 0) {
      if ((*(nfiring->portList)).head()) {
	VHDLPortListIter nextPort(*(nfiring->portList));
	VHDLPort* nport;
	while ((nport = nextPort++) != 0) {
	  level++;
	  if (portCount) {
	    body << ";\n";
	  }
	  body << indent(level) << nport->name << ": " << nport->direction
	       << " " << nport->type;
	  portCount++;
	  level--;
	}
      }
    }
    
    closer << "\n";
    closer << indent(level) << ");\n";
    level--;

    if (portCount) {
      myCode << opener << body << closer;
    }
  }
}

// Add in sensitivity list of input ports.
// Do this explicitly for sake of synthesis.
void StructTarget :: addSensitivities(VHDLCluster* cl, int level) {
  if ((*(cl->firingList)).head()) {
    StringList opener, body, closer;

    level++;
    opener << indent(level) << "(\n";

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int sensCount = 0;
    while ((nfiring = nextFiring++) != 0) {
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
      myCode << opener << body << closer;
    }
  }
}

// Add in variable refs here from variableList.
void StructTarget :: addVariableRefs(VHDLCluster* cl, int level) {
  if ((*(cl->firingList)).head()) {
    StringList body;

    VHDLFiringListIter nextFiring(*(cl->firingList));
    VHDLFiring* nfiring;
    int varCount = 0;
    while ((nfiring = nextFiring++) != 0) {
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
    }
    
    if (varCount) {
      myCode << body;
    }
  }
}

// Add in port to variable transfers here from portVarList.
void StructTarget :: addPortVarTransfers(VHDLCluster* cl, int /*level*/) {
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
      myCode << body;
    }
  }
}

// Add in firing actions here.
void StructTarget :: addActions(VHDLCluster* cl, int /*level*/) {
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
      myCode << body;
    }
  }
}

// Add in variable to port transfers here from varPortList.
void StructTarget :: addVarPortTransfers(VHDLCluster* cl, int /*level*/) {
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
      myCode << body;
    }
  }
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

  registerCompDecl(clName, masterPortList, masterGenericList);
  mergeSignalList(masterSignalList);
  registerCompMap(clLabel, clName, masterPortMapList, masterGenericMapList);
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

// Add in component declarations here from compDeclList.
void StructTarget :: buildComponentDeclarations(int level) {
  VHDLCompDeclListIter nextCompDecl(compDeclList);
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
//	if ((ngen->defaultVal).numPieces() > 0) {
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

// Add in signal declarations here from signalList.
void StructTarget :: buildSignalDeclarations(int level) {
  VHDLSignalListIter nextSignal(signalList);
  VHDLSignal* signal;
  while ((signal = nextSignal++) != 0) {
    level++;
    signal_declarations << indent(level) << "signal " << signal->name << ": "
			<< signal->type << ";\n";
    level--;
  }
}

// Add in component mappings here from compMapList.
void StructTarget :: buildComponentMappings(int level) {
  VHDLCompMapListIter nextCompMap(compMapList);
  VHDLCompMap* compMap;
  while ((compMap = nextCompMap++) != 0) {
    level++;
    component_mappings << indent(level) << compMap->name << ": "
		       << compMap->type << "\n";

    // Add in generic maps here from genMapList.
    if (compMap->genMapList->head()) {
      level++;
      component_mappings << indent(level) << "generic map(\n";
      VHDLGenericMapListIter nextGenMap(*(compMap->genMapList));
      VHDLGenericMap* ngenmap;
      int genCount = 0;
      while ((ngenmap = nextGenMap++) != 0) {
	level++;
	if (genCount) {
	  component_mappings << ",\n";
	}
	component_mappings << indent(level) << ngenmap->name << " => "
			   << ngenmap->mapping;
	level--;
      }
      component_mappings << "\n";
      component_mappings << indent(level) << ")\n";
      level--;
    }

    // Add in port maps here from portMapList.
    if (compMap->portMapList->head()) {
      level++;
      component_mappings << indent(level) << "port map(\n";
      VHDLPortMapListIter nextPortMap(*(compMap->portMapList));
      VHDLPortMap* nportmap;
      int portCount = 0;
      while ((nportmap = nextPortMap++) != 0) {
	level++;
	if (portCount) {
	  component_mappings << ",\n";
	}
	component_mappings << indent(level) << nportmap->name << " => "
			   << nportmap->mapping;
	portCount++;
	level--;
      }
      component_mappings << "\n";
      component_mappings << indent(level) << ")\n";
      level--;
    }
    
    component_mappings << indent(level) << ";\n";
    level--;
  }
}

// Generate the architecture_body_closer.
void StructTarget :: buildArchitectureBodyCloser(int /*level*/) {
  architecture_body_closer << "end structure;\n";
}

// Add in configuration declaration here from compDeclList.
void StructTarget :: buildConfigurationDeclaration(int level) {
  configuration_declaration << "configuration " << "parts" << " of "
			    << galaxy()->name() << " is\n";
  configuration_declaration << "for " << "structure" << "\n";

  VHDLCompDeclListIter nextCompDecl(compDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    level++;
    configuration_declaration << indent(level) << "for all:" << compDecl->name
			      << " use entity " << "work." << compDecl->name
			      << "(behavior); end for;\n";
    level--;
  }

  configuration_declaration << "end " << "for" << ";\n";
  configuration_declaration << "end " << "parts" << ";\n";
}

// Generate the register entity and architecture.
StringList StructTarget :: regCode() {
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Reg_INT : register of type INT\n";
  codeList << "entity Reg_INT is\n";
  codeList << "     port (C: in boolean; D: in integer; Q: out integer);\n";
  codeList << "end Reg_INT;\n";
  codeList << "\n";
  codeList << "architecture behavior of Reg_INT is\n";
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

// Generate the selector entity and architecture.
StringList StructTarget :: selCode() {
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Sel_INT : initial value selector of type INT\n";
  codeList << "entity Sel_INT is\n";
  codeList << "	port(\n";
  codeList << "		control: IN BOOLEAN;\n";
  codeList << "		init_val: IN INTEGER;\n";
  codeList << "		input: IN INTEGER;\n";
  codeList << "		output: OUT INTEGER\n";
  codeList << "	);\n";
  codeList << "end Sel_INT;\n";
  codeList << "\n";
  codeList << "architecture behavior of Sel_INT is\n";
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
StringList StructTarget :: sourceCode() {
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Source_INT : constant generator\n";
  codeList << "entity Source_INT is\n";
  codeList << "	generic(\n";
  codeList << "		value: INTEGER\n";
  codeList << "	);\n";
  codeList << "	port(\n";
  codeList << "		output: OUT INTEGER\n";
  codeList << "	);\n";
  codeList << "end Source_INT;\n";
  codeList << "\n";
  codeList << "architecture behavior of Source_INT is\n";
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
