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
#include "VHDLCluster.h"
#include "FloatArrayState.h"
#include "IntArrayState.h"
#include "ComplexArrayState.h"
#include "KnownTarget.h"
#include <ostream.h>

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

  // Create a new VHDLCluster to load up.
  VHDLCluster* cl = new VHDLCluster;

  // Begin constructing the firing's code in myCode.
  StringList tempName = s->fullName();
  StringList sanTempName = sanitize(tempName);
  cl->name = targetNestedSymbol.push(sanTempName);
  cl->starClassName = s->className();

  cl->genericList = firingGenericList.newCopy();
  cl->portList = firingPortList.newCopy();
  cl->variableList = firingVariableList.newCopy();
  cl->portVarList = firingPortVarList.newCopy();
  cl->action = firingAction;
  firingAction.initialize();
  cl->varPortList = firingVarPortList.newCopy();

  VHDLGenericList* genList = firingGenericList.newCopy();
  VHDLPortList* portList = firingPortList.newCopy();
  VHDLGenericMapList* genMapList = firingGenericMapList.newCopy();
  VHDLPortMapList* portMapList = firingPortMapList.newCopy();
  VHDLSignalList* sigList = firingSignalList.newCopy();

  StringList label = cl->name;
  label << "_proc";
  StringList name = cl->name;

  registerCompDecl(name, portList, genList);
  mergeSignalList(sigList);
  registerCompMap(label, name, portMapList, genMapList);

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

  // Begin constructing the cluster's code in myCode.
  myCode << "\n\t-- firing " << cl->name;
  myCode << " (class " << cl->starClassName << ") \n";
  myCode << "entity " << cl->name << " is\n";

  // Add in generic refs here from genericList.
  if ((*(cl->genericList)).head()) {
    level++;
    myCode << indent(level) << "generic(\n";
    VHDLGenericListIter nextGeneric(*(cl->genericList));
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
  
  // Add in port refs here from portList.
  if ((*(cl->portList)).head()) {
    level++;
    myCode << indent(level) << "port(\n";
    VHDLPortListIter nextPort(*(cl->portList));
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
  
  myCode << indent(level) << "end " << cl->name << ";\n";
  myCode << "\n";
  myCode << "architecture " << "behavior" << " of "
	 << cl->name << " is\n";
  myCode << "begin\n";
  myCode << "process\n";

  // Add in sensitivity list of input ports.
  // Do this explicitly for sake of synthesis.
  if ((*(cl->portList)).head()) {
    level++;
    myCode << indent(level) << "(\n";
    VHDLPortListIter nextPort(*(cl->portList));
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
  
  // Add in variable refs here from variableList.
  VHDLVariableListIter nextVar(*(cl->variableList));
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

  // Add in port to variable transfers here from portVarList.
  VHDLPortVarListIter nextPortVar(*(cl->portVarList));
  VHDLPortVar* nPortVar;
  while ((nPortVar = nextPortVar++) != 0) {
    myCode << nPortVar->variable << " := " << nPortVar->name << ";\n";
  }

  // process action
  myCode << cl->action;

  // Add in variable to port transfers here from varPortList.
  VHDLPortVarListIter nextVarPort(*(cl->varPortList));
  VHDLPortVar* nVarPort;
  while ((nVarPort = nextVarPort++) != 0) {
    myCode << nVarPort->name << " <= " << nVarPort->variable << ";\n";
  }

  myCode << "end process;\n";
  myCode << "end behavior;\n";

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

  // Iterate through the state list and connect registers and
  // initial value selectors for each referenced state.
  VHDLStateListIter nextState(stateList);
  VHDLState* state;
  while ((state = nextState++) != 0) {
    // Need to connect a reg and a mux between the lastRef
    // to the state and the firstRef to the state.
//    connectRegister(state->lastRef, state->firstRef, state->type);
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
      connectRegister(sourceName, destName, "INTEGER");
      cout << "Connecting " << sourceName << " and " << destName << "\n";

      // Must also create signals for those lines which are neither read nor
      // written by a $ref() - eg if more delays than tokens read.
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
// ** CONSTRUCTION SECTION  - END


  int level = 0;

// Begin Cluster Elaboration  
// iterate for all firings/clusters:


// End Cluster Elaboration
  
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

  // Generate the architecture_body_closer.
  architecture_body_closer << "end structure;\n";

  // Add in configuration declaration here from compDeclList.
  configuration_declaration << "configuration " << "parts" << " of "
			    << galaxy()->name() << " is\n";
  configuration_declaration << "for " << "structure" << "\n";

  nextCompDecl.reset();
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
    myCode << "architecture behavior of Reg_INT is\n";
    myCode << "begin\n";
    myCode << "     main: process\n";
    myCode << "     begin\n";
    myCode << "          wait until C'event and C = TRUE;\n";
    myCode << "          Q <= D;\n";
    myCode << "     end process main;\n";
    myCode << "end behavior;\n";
    myCode << "\n";
  }
  
  if (selectors()) {
    myCode << "\n";
    myCode << "     -- Sel_INT : initial value selector of type INT\n";
    myCode << "entity Sel_INT is\n";
    myCode << "	port(\n";
    myCode << "		control: IN BOOLEAN;\n";
    myCode << "		init_val: IN INTEGER;\n";
    myCode << "		input: IN INTEGER;\n";
    myCode << "		output: OUT INTEGER\n";
    myCode << "	);\n";
    myCode << "end Sel_INT;\n";
    myCode << "\n";
    myCode << "architecture behavior of Sel_INT is\n";
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
  
  if (sources()) {
    myCode << "\n";
    myCode << "     -- Source_INT : constant generator\n";
    myCode << "entity Source_INT is\n";
    myCode << "	generic(\n";
    myCode << "		value: INTEGER\n";
    myCode << "	);\n";
    myCode << "	port(\n";
    myCode << "		output: OUT INTEGER\n";
    myCode << "	);\n";
    myCode << "end Source_INT;\n";
    myCode << "\n";
    myCode << "architecture behavior of Source_INT is\n";
    myCode << "	begin\n";
    myCode << "		output <= value;\n";
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
  newCompMap->genMapList = genMapList;
  newCompMap->portMapList = portMapList;
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
    initVal << state->initValue();
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

    // If it's the first firing to refer to this state,
    if (isFirstStateRef) {
      firingSignalList.put(refIn, stType, "", refIn);
      firingPortMapList.put(refIn, refIn);
      firingPortMapList.put(refOut, refOut);
    }

    // If this isn't the first firing to refer to this state,
    if (!isFirstStateRef) {
      // Contstruct the name of the last ref to the state.
      StringList stateSignal = root;
      stateSignal << "_" << listState->lastFiring << "_Out";
      firingSignalList.put(stateSignal, stType, stateSignal, refIn);
      firingPortMapList.put(refIn, stateSignal);
      firingPortMapList.put(refOut, refOut);
    }
      
    firingSignalList.put(refOut, stType, refOut, "");
    firingVariableList.put(ref, stType, "");
    firingPortVarList.put(refIn, ref);
    firingVarPortList.put(refOut, ref);
    firingPortList.put(refIn, "IN", stType);
    firingPortList.put(refOut, "OUT", stType);
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
void StructTarget :: registerSource(StringList type) {
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
				     StringList initVal, StringList type) {
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
void StructTarget :: registerSelector(StringList type) {
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
				     StringList type) {
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
void StructTarget :: registerRegister(StringList type) {
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
