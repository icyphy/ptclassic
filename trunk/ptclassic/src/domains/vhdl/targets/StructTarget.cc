static const char file_id[] = "StructTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer: Michael C. Williamson

 Base target for VHDL code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "StructTarget.h"
#include "VHDLStar.h"
#include "VHDLState.h"
#include "KnownTarget.h"
#include <ostream.h>

// Constructor.
StructTarget :: StructTarget(const char* name,const char* starclass,
			 const char* desc) :
VHDLTarget(name,starclass,desc) {
  regsUsed = 0;
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

// Add additional codeStreams.
void StructTarget :: addCodeStreams() {
  addStream("entity_declaration", &entity_declaration);
  addStream("architecture_body_opener", &architecture_body_opener);
  addStream("component_declarations", &component_declarations);
  addStream("signal_declarations", &signal_declarations);
  addStream("component_mappings", &component_mappings);
  addStream("architecture_body_closer", &architecture_body_closer);
  addStream("starDecls", &starDecls);
  addStream("starInit", &starInit);
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
  starDecls.initialize();
  starInit.initialize();
  firingAction.initialize();
}

// Initialize firing lists.
void StructTarget :: initFiringLists() {
  firingPortList.initialize();
  firingGenericList.initialize();
  firingPortMapList.initialize();
  firingGenericMapList.initialize();
  firingSignalList.initialize();
  firingVariableList.initialize();
  firingPortVarList.initialize();
  firingVarPortList.initialize();
}

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
  int level = 0;
  // Change the default stream temporarily.
  defaultStream = &firingAction;
  // Initialize lists for new firing.
  initFiringLists();
  // Process action; running star modifies myCode.
  int status = ((CGStar*) s)->CGStar::run();
  // Change the default stream back.
  defaultStream = &myCode;

  // Begin constructing the firing's code in myCode.
  StringList tempName = s->fullName();
  StringList sanTempName = sanitize(tempName);
  myCode << "\n\t-- firing " << targetNestedSymbol.push(sanTempName);
  myCode << " (class " << s->className() << ") \n";
  myCode << "entity " << targetNestedSymbol.get() << " is\n";

  // Add in port refs here from firingPortList.
  if (firingPortList.head()) {
    level++;
    myCode << indent(level) << "port(\n";
    VHDLPortListIter nextPort(firingPortList);
    VHDLPort* nport;
    int portCount = 0;
    while ((nport = nextPort++) != 0) {
      level++;
      if (portCount) myCode << ";\n";
      myCode << indent(level) << nport->name << ": " << nport->direction
	     << " " << nport->type;
      portCount++;
      level--;
    }
    myCode << "\n";
    myCode << indent(level) << ");\n";
    level--;
  }
  
  // Add in generic refs here from firingGenericList.
  if (firingGenericList.head()) {
    level++;
    myCode << indent(level) << "generic(\n";
    VHDLGenericListIter nextGeneric(firingGenericList);
    VHDLGeneric* ngen;
    int genCount = 0;
    while ((ngen = nextGeneric++) != 0) {
      level++;
      if (genCount) {
	myCode << ";\n";
      }
      myCode << indent(level) << ngen->name << ": " << ngen->type;
      if ((ngen->defaultVal).numPieces() > 0) {
	myCode << " := " << ngen->defaultVal;
      }
      genCount++;
      level--;
    }
    myCode << "\n";
    myCode << indent(level) << ");\n";
    level--;
  }
  
  myCode << indent(level) << "end " << targetNestedSymbol.get() << ";\n";
  myCode << "\n";
  myCode << "architecture " << "behavior" << " of "
	 << targetNestedSymbol.get() << " is\n";
  myCode << "begin\n";
  myCode << "process\n";

  // Add in sensitivity list of input ports.
  // Do this explicitly for sake of synthesis.
  if (firingPortList.head()) {
    level++;
    myCode << indent(level) << "(\n";
    VHDLPortListIter nextPort(firingPortList);
    VHDLPort* nport;
    int portCount = 0;
    while ((nport = nextPort++) != 0) {
      if (!strcmp(nport->direction,"IN")) {
	level++;
	if (portCount) {
	  myCode << ",\n";
	}
	myCode << indent(level) << nport->name;
	portCount++;
	level--;
      }
    }
    myCode << "\n";
    myCode << indent(level) << ")\n";
    level--;
  }
  
  // Add in variable refs here from firingVariableList.
  VHDLVariableListIter nextVar(firingVariableList);
  VHDLVariable* nvar;
  while ((nvar = nextVar++) != 0) {
    level++;
    myCode << indent(level) << "variable " << nvar->name << ": " << nvar->type;
    if ((nvar->initVal).length() > 0) {
      myCode << " := " << nvar->initVal;
    }
    myCode << ";\n";
    level--;
  }

  myCode << "begin\n";

  // Add in port to variable transfers here from firingPortVarList.
  VHDLPortVarListIter nextPortVar(firingPortVarList);
  VHDLPortVar* nPortVar;
  while ((nPortVar = nextPortVar++) != 0) {
    myCode << nPortVar->variable << " := " << nPortVar->name << ";\n";
  }

  // process action
  myCode << firingAction;
  firingAction.initialize();

  // Add in variable to port transfers here from firingVarPortList.
  VHDLPortVarListIter nextVarPort(firingVarPortList);
  VHDLPortVar* nVarPort;
  while ((nVarPort = nextVarPort++) != 0) {
    myCode << nVarPort->name << " <= " << nVarPort->variable << ";\n";
  }

  myCode << "end process;\n";
  myCode << "end behavior;\n";

  StringList label = targetNestedSymbol.get();
  label << "_proc";
  StringList name = targetNestedSymbol.get();

  VHDLPortList* portList = firingPortList.newCopy();
  VHDLGenericList* genList = firingGenericList.newCopy();
  VHDLPortMapList* portMapList = firingPortMapList.newCopy();
  VHDLGenericMapList* genMapList = firingGenericMapList.newCopy();
  VHDLSignalList* sigList = firingSignalList.newCopy();

  registerCompDecl(name, portList, genList);
  mergeSignalList(sigList);
  registerCompMap(label, name, portMapList, genMapList);

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

// Trailer code (done last).
void StructTarget :: trailerCode() {

  
// ** CONSTRUCTION SECTION - START
  cout << "#############################\n";
  VHDLArcListIter nextArc(arcList);
  VHDLArc* arc;
  while ((arc = nextArc++) != 0) {
    cout << "\n";
    cout << "Name:  " << arc->name << "\n";
    cout << "    lowWrite:   " << arc->lowWrite << "\n";
    cout << "    highWrite:  " << arc->highWrite << "\n";
    cout << "    lowRead:    " << arc->lowRead << "\n";
    cout << "    highRead:   " << arc->highRead << "\n";

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
      connectReg(sourceName, destName, "INTEGER");
      cout << "Connecting " << sourceName << " and " << destName << "\n";

      // Must also create signals for those lines which are neither read nor
      // written by a $ref() - eg if more delays than tokens read.
      // However, do not create a signal if it's a wormhole input.
      // Will have created a system port input instead.

      // If no system port by the given name, go ahead and make the signal.
      if (sx < arc->lowWrite) {
	signalListPut(&signalList, sourceName, "INTEGER", "", "");
      }
      if (ix < arc->lowWrite) {
	signalListPut(&signalList, destName, "INTEGER", "", "");
      }
    }
  }
// ** CONSTRUCTION SECTION  - END


  int level = 0;
  // Generate the entity_declaration.
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

  // Generate the architecture_body_opener.
  architecture_body_opener << "architecture " << "structure" << " of "
			   << galaxy()->name() << " is\n";

  // Add in component declarations here from compDeclList.
  VHDLCompDeclListIter nextCompDecl(compDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    level++;
    component_declarations << indent(level) << "component " << compDecl->name
			   << "\n";

    // Add in port refs here from portList.
    if (!(compDecl->portList->head())) {
      Error::error("Empty compDecl port list!");
    }
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
	if ((ngen->defaultVal).numPieces() > 0) {
	  component_declarations << " := " << ngen->defaultVal;
	}
	genCount++;
	level--;
      }
      component_declarations << "\n";
      component_declarations << indent(level) << ");\n";
      level--;
    }
    
    component_declarations << indent(level) << "end component;\n";
    level--;
  }

  // Add in signal declarations here from signalList.
  VHDLSignalListIter nextSignal(signalList);
  VHDLSignal* signal;
  while ((signal = nextSignal++) != 0) {
    level++;
    signal_declarations << indent(level) << "signal " << signal->name << ": "
			<< signal->type << ";\n";
    level--;
  }

  // Add in component mappings here from compMapList.
  VHDLCompMapListIter nextCompMap(compMapList);
  VHDLCompMap* compMap;
  while ((compMap = nextCompMap++) != 0) {
    level++;
    component_mappings << indent(level) << compMap->label << ": "
		       << compMap->name << "\n";

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

    component_mappings << indent(level) << ";\n";
    level--;
  }

  // Generate the architecture_body_closer.
  architecture_body_closer << "end structure;\n";
}

// Combine all sections of code.
void StructTarget :: frameCode() {
  StringList code = headerComment();

  if (registers()) {
    myCode << "\n";
    myCode << "     -- Reg_INT : register of type INT\n";
    myCode << "entity Reg_INT is\n";
    myCode << "     port (C: in boolean; D: in integer; Q: out integer);\n";
    myCode << "end Reg_INT;\n";
    myCode << "\n";
    myCode << "architecture Reg_INT_behavior of Reg_INT is\n";
    myCode << "begin\n";
    myCode << "     main: process\n";
    myCode << "     begin\n";
    myCode << "          wait until C'event and C = TRUE;\n";
    myCode << "          Q <= D;\n";
    myCode << "     end process main;\n";
    myCode << "end Reg_INT_behavior;\n";
    myCode << "\n";
  }
  
  if (initials()) {
    myCode << "\n";
    myCode << "     -- initial : initial value selector\n";
    myCode << "entity initial is\n";
    myCode << "	port(\n";
    myCode << "		control: IN BOOLEAN;\n";
    myCode << "		init_val: IN INTEGER;\n";
    myCode << "		input: IN INTEGER;\n";
    myCode << "		output: OUT INTEGER\n";
    myCode << "	);\n";
    myCode << "end initial;\n";
    myCode << "\n";
    myCode << "architecture behavior of initial is\n";
    myCode << "	begin\n";
    myCode << "		process (control, init_val, input)\n";
    myCode << "		begin\n";
    myCode << "			if control then\n";
    myCode << "				output <= input;\n";
    myCode << "			else\n";
    myCode << "				output <= init_val;\n";
    myCode << "			end if;\n";
    myCode << "		end process;\n";
    myCode << "end behavior;\n";
    myCode << "\n";
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
void StructTarget :: beginIteration(int repetitions, int depth) {
}

// Generate code to end an iterative procedure
void StructTarget :: endIteration(int /*reps*/, int depth) {
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

// Register component declaration.
void StructTarget :: registerCompDecl(StringList name, VHDLPortList* portList,
				      VHDLGenericList* genList) {
  if (compDeclList.inList(name)) return;
  
  // Allocate memory for a new VHDLCompDecl and put it in the list.
  VHDLCompDecl* newCompDecl = new VHDLCompDecl;
  newCompDecl->name = name;
  newCompDecl->portList = portList;
  newCompDecl->genList = genList;
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
void StructTarget :: registerCompMap(StringList label, StringList name,
				     VHDLPortMapList* portMapList,
				     VHDLGenericMapList* genMapList) {
//  Problem: inList uses name, not label, for generic VHDL objects.
//  Will just comment this out for now.  If get repeated registrations
//  of a comp map, will deal with that.
//  if (compMapList.inList(label)) return;
  
  // Allocate memory for a new VHDLCompMap and put it in the list.
  VHDLCompMap* newCompMap = new VHDLCompMap;
  newCompMap->label = label;
  newCompMap->name = name;
  newCompMap->portMapList = portMapList;
  newCompMap->genMapList = genMapList;
  compMapList.put(*newCompMap);
}

// Register the State reference.
void StructTarget :: registerState(State* state, int thisFiring/*=-1*/,
				   int pos/*=-1*/) {
  StringList temp = sanitizedFullName(*state);
  StringList ref = sanitize(temp);
  StringList stType = stateType(state);
  StringList root;
  StringList initVal;
  
  if (pos >= 0) {
    ref << "_P" << pos;
    // WARNING: The following is dangerous unless you know
    // that state has a big enough array and that pos is valid!
//    initVal << state[pos].initValue();
  }
  else {
//    initVal << state->initValue();
  }
  
  // Root is ref, without marking for any particular firing.
  root = ref;

  ref << "_" << thisFiring;

  StringList refIn = sanitize(ref);
  refIn << "_In";
  StringList refOut = sanitize(ref);
  refOut << "_Out";
  StringList reg = sanitize(root);
  reg << "_Reg";

  // look for state in stateList
  // if don't find it, enter it with -1 as state->lastFiring
  if (!(stateList.inList(root))) {
    // Allocate memory for a new VHDLState and put it in the list.
    VHDLState* newState = new VHDLState;
    newState->name = root;
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

    // If it's the first firing to refer to this state,
    if (isFirstStateRef) {
      // make regi, map it to IN and OUT
      connectReg(refOut, refIn, stType);
//	StringList TEMP = refIn;
//	TEMP << "_Temp";
//      connectReg(refOut, TEMP, stType);
//      connectInit(TEMP, refIn, initVal, stType);
//      signalListPut(&signalList, TEMP, stType, "", "");

      signalListPut(&firingSignalList, refIn, stType, "", refIn);
      portMapListPut(&firingPortMapList, refIn, refIn);
      portMapListPut(&firingPortMapList, refOut, refOut);
    }

    // If this isn't the first firing to refer to this state,
    if (!isFirstStateRef) {
      // re-map input to register with output of this firing's state

      // Contstruct the name of the last ref to the state.
      StringList stateSignal = root;
      stateSignal << "_" << listState->lastFiring << "_Out";

      signalListPut(&firingSignalList, stateSignal, stType, stateSignal, refIn);
      portMapListPut(&firingPortMapList, refIn, stateSignal);
      portMapListPut(&firingPortMapList, refOut, refOut);
      
      // Get the register port map from the port map list and re-map it.
      VHDLCompMapListIter compMapNext(compMapList);
      VHDLCompMap* nCompMap;
      while ((nCompMap = compMapNext++) != 0) {
	// Check for a match.
	StringList nameTemp = nCompMap->label;
	StringList stateTemp = reg;
	if (!strcmp(nameTemp,stateTemp)) {
	  // Found a match.
	  VHDLPortMapListIter portMapNext(*(nCompMap->portMapList));
	  VHDLPortMap* nPortMap;
	  while ((nPortMap = portMapNext++) != 0) {
	    // Check for a match.
	    StringList portTemp = nPortMap->name;
	    StringList DTemp = "D";
	    if (!strcmp(portTemp,DTemp)) {
	      // Found a match.
	      nPortMap->mapping = refOut;
	    }
	  }
	}
      }
    }

    signalListPut(&firingSignalList, refOut, stType, refOut, "");
    variableListPut(&firingVariableList, ref, stType, "");
    portVarListPut(&firingPortVarList, refIn, ref);
    portVarListPut(&firingVarPortList, refOut, ref);
    portListPut(&firingPortList, refIn, "IN", stType);
    portListPut(&firingPortList, refOut, "OUT", stType);
  }

  // Reset state's lastFiring tag just before exiting.
  listState->lastFiring = thisFiring;
}

// Connect a register between the given input and output signals.
void StructTarget :: connectReg(StringList inName, StringList outName,
				StringList type) {
      registerRegister("INTEGER");
      
      StringList label = outName;
      label << "_REG";
      
      StringList name = "Reg";
//      name << "_" << type;
      name << "_" << "INT";

      VHDLPortMapList* portMapList = new VHDLPortMapList;
      VHDLGenericMapList* genMapList = new VHDLGenericMapList;
      portMapList->initialize();
      genMapList->initialize();
      
      portMapListPut(portMapList, "D", inName);
      portMapListPut(portMapList, "Q", outName);
      portMapListPut(portMapList, "C", "clock");
      
      portListPut(&systemPortList, "clock", "IN", "boolean");
      
      registerCompMap(label, name, portMapList, genMapList);
}

// Add a register component declaration.
void StructTarget :: registerRegister(StringList regType) {
  // Set the flag indicating registers are needed.
  setRegisters();

  StringList name = "Reg";
//  name << "_" << regType;
  name << "_" << "INT";

  VHDLPortList* portList = new VHDLPortList;
  VHDLGenericList* genList = new VHDLGenericList;
  portList->initialize();
  genList->initialize();
  
//  portListPut(portList, "D", "IN", regType);
//  portListPut(portList, "Q", "OUT", regType);
  portListPut(portList, "D", "IN", "INTEGER");
  portListPut(portList, "Q", "OUT", "INTEGER");
  portListPut(portList, "C", "IN", "boolean");

  registerCompDecl(name, portList, genList);
}

// Register PortHole reference.
void StructTarget :: registerPortHole(VHDLPortHole* port,
				      int tokenNum/*=-1*/) {
  // The registry keeps track of all refed arcs, and their min/max R/W offsets.
  registerArcRef(port, tokenNum);

// I want to pass along the offset info as well as whether it's
// a read or a write so I can keep tabs on the production window and
// the read window, and then do nice things at the end based on that.
// Also, must do special things if it's a wormhole input.

// Continue to do normal signal naming and portmapping.

  // Create a port and a port mapping for this firing entity.
  StringList ref = port->getGeoName();
  if (tokenNum >= 0) {
    ref << "_" << tokenNum;
  }
  else { /* (tokenNum < 0) */
    ref << "_N" << (-tokenNum);
  }
  
  portListPut(&firingPortList, ref, port->direction(), port->dataType());
  portMapListPut(&firingPortMapList, ref, ref);

  // For wormhole inputs, create a system port.
  // But for delayed values of wormhole inputs, do not create another port.
  // For local inputs, create a signal.
  if ((port->atBoundary()) && (tokenNum >= 0)) {
    // Port at wormhole boundary, so create a system port instead of a signal.
    portListPut(&systemPortList, ref, port->direction(), port->dataType());
  }
  else {
    signalListPut(&firingSignalList, ref, port->dataType(), ref, ref);
  }
}

ISA_FUNC(StructTarget,VHDLTarget);

// Register a read or write to an arc and the offset.
void StructTarget :: registerArcRef(VHDLPortHole* port, int tokenNum) {
  StringList direction = port->direction();
  StringList name = port->getGeoName();
  int noSuchArc = 1;
  
  // Search through the arc list for an arc with the given name.
  // If one is found, update it's low/high write/read markers.
  VHDLArcListIter nextArc(arcList);
  VHDLArc* arc;
  while ((arc = nextArc++) != 0) {
    if (!strcmp(arc->name, name)) {
      noSuchArc = 0;
      if (!strcmp(port->direction(),"OUT")) {
	if (tokenNum < arc->lowWrite) arc->lowWrite = tokenNum;
	if (tokenNum > arc->highWrite) arc->highWrite = tokenNum;
      }
      else if (!strcmp(port->direction(),"IN")) {
	if (tokenNum < arc->lowRead) arc->lowRead = tokenNum;
	if (tokenNum > arc->highRead) arc->highRead = tokenNum;
      }
      else {
	Error::error(*port, " is neither IN nor OUT");
      }
    }
  } 

  // If no arc with the given name is in the list, then create one.
  if (noSuchArc) {
    VHDLArc* newArc = new VHDLArc;
    newArc->name = name;
    if (!strcmp(port->direction(),"OUT")) {
      newArc->lowWrite = tokenNum;
      newArc->highWrite = tokenNum;
      newArc->lowRead = 0;
      newArc->highRead = 0;
    }
    else if (!strcmp(port->direction(),"IN")) {
      newArc->lowWrite = port->geo().numInit();
      newArc->highWrite = port->geo().numInit();
      newArc->lowRead = tokenNum;
      newArc->highRead = tokenNum;
    }
    else {
      Error::error(*port, " is neither IN nor OUT");
    }
    arcList.put(*newArc);
  }
}

// Allocate memory for a new VHDLSignal and put it in the list.
void StructTarget :: signalListPut(VHDLSignalList* nlist, StringList nname,
				   StringList ntype, StringList nfrom,
				   StringList nto) {
  if (nlist->inList(nname)) return;
  VHDLSignal* newSignal = new VHDLSignal;
  newSignal->name = nname;
  newSignal->type = ntype;
  newSignal->from = nfrom;
  newSignal->to = nto;
  nlist->put(*newSignal);
}

// Allocate memory for a new VHDLPort and put it in the list.
void StructTarget :: portListPut(VHDLPortList* list, StringList name,
				 StringList direction, StringList type) {
  if (list->inList(name)) return;
  VHDLPort* newPort = new VHDLPort;
  newPort->name = name;
  newPort->direction = direction;
  newPort->type = type;
  list->put(*newPort);
}

// Allocate memory for a new VHDLPortMap and put it in the list.
void StructTarget :: portMapListPut(VHDLPortMapList* list, StringList name,
				    StringList mapping) {
  if (list->inList(name)) return;
  VHDLPortMap* newPortMap = new VHDLPortMap;
  newPortMap->name = name;
  newPortMap->mapping = mapping;
  list->put(*newPortMap);
}


// Allocate memory for a new VHDLVariable and put it in the list.
void StructTarget :: variableListPut(VHDLVariableList* list, StringList name,
				     StringList type, StringList initVal) {
  if (list->inList(name)) return;
  VHDLVariable* newVar = new VHDLVariable;
  newVar->name = name;
  newVar->type = type;
  newVar->initVal = initVal;
  list->put(*newVar);
}

// Allocate memory for a new VHDLPortVar and put it in the list.
void StructTarget :: portVarListPut(VHDLPortVarList* list, StringList name,
				    StringList variable) {
  if (list->inList(name)) return;
  VHDLPortVar* portVar = new VHDLPortVar;
  portVar->name = name;
  portVar->variable = variable;
  list->put(*portVar);
}
