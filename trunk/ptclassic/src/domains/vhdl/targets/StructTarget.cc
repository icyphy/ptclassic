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

// constructor
StructTarget :: StructTarget(const char* name,const char* starclass,
			 const char* desc) :
VHDLTarget(name,starclass,desc) {
  // Add additional codeStreams.
  addCodeStreams();

  // Initialize codeStreams.
  initCodeStreams();
}

// clone
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
//  addStream("firingVarDecls", &firingVarDecls);
//  addStream("firingPreAction", &firingPreAction);
  addStream("firingAction", &firingAction);
//  addStream("firingPostAction", &firingPostAction);
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
//  firingVarDecls.initialize();
//  firingPreAction.initialize();
  firingAction.initialize();
//  firingPostAction.initialize();
}

// Setup the target.
void StructTarget :: setup() {
	if (galaxy()) setStarIndices(*galaxy()); 
	HLLTarget::setup();
}

// Main routine.
int StructTarget :: runIt(VHDLStar* s) {
  int level = 0;
  // Change the default stream temporarily.
  defaultStream = &firingAction;
  // Initialize lists for new firing.
  firingPortList.initialize();
  firingGenericList.initialize();
  firingPortMapList.initialize();
  firingGenericMapList.initialize();
  firingSignalList.initialize();
  firingVariableList.initialize();
  firingPortVarList.initialize();
  firingVarPortList.initialize();
  // process action; running star modifies myCode.
  int status = ((CGStar*) s)->CGStar::run();
  // Change the default stream back.
  defaultStream = &myCode;

  myCode << "\n\t-- firing ";
  StringList tempName = s->fullName();
  StringList sanTempName = sanitize(tempName);
  myCode << targetNestedSymbol.push(sanTempName);
  myCode << " (class " << s->className() << ") \n";
  myCode << "entity ";
  myCode << targetNestedSymbol.get();
  myCode << " is\n";

  if (firingPortList.head()) {
    level++;
    myCode << indent(level);
    myCode << "port(\n";
    // Add in port refs here from firingPortList.
    VHDLPortListIter nextPort(firingPortList);
    VHDLPort* nport;
    int portCount = 0;
    while ((nport = nextPort++) != 0) {
      if (portCount) {
	myCode << ";";
      	myCode << "\n";
      }
      level++;
      myCode << indent(level);
      myCode << nport->name;
      myCode << ": ";
      myCode << nport->direction;
      myCode << " ";
      myCode << nport->type;
      portCount++;
      level--;
    }
    myCode << "\n";
    myCode << indent(level);
    myCode << ");\n";
    level--;
  }
  
  if (firingGenericList.head()) {
    level++;
    myCode << indent(level);
    myCode << "generic(\n";
    // Add in generic refs here from firingGenericList.
    VHDLGenericListIter nextGeneric(firingGenericList);
    VHDLGeneric* ngen;
    int genCount = 0;
    while ((ngen = nextGeneric++) != 0) {
      if (genCount) {
	myCode << ";";
	myCode << "\n";
      }
      level++;
      myCode << indent(level);
      myCode << ngen->name;
      myCode << ": ";
      myCode << ngen->type;
      if ((ngen->defaultVal).numPieces() > 0) {
	myCode << " := ";
	myCode << ngen->defaultVal;
      }
      genCount++;
      level--;
    }
    myCode << "\n";
    myCode << indent(level);
    myCode << ");\n";
    level--;
  }
  
  myCode << indent(level);
  myCode << "end ";
  myCode << targetNestedSymbol.get();
  myCode << ";\n";
  myCode << "\n";

  myCode << "architecture ";
  myCode << "behavior";
  myCode << " of ";
  myCode << targetNestedSymbol.get();
  myCode << " is\n";

  myCode << "begin\n";
  myCode << "process\n";

  // Add in variable refs here from firingVariableList.
  VHDLVariableListIter nextVar(firingVariableList);
  VHDLVariable* nvar;
  while ((nvar = nextVar++) != 0) {
    level++;
    myCode << indent(level);
    myCode << "variable ";
    myCode << nvar->name;
    myCode << ": ";
    myCode << nvar->type;
    if ((nvar->initVal).numPieces() > 0) {
      myCode << " := ";
      myCode << nvar->initVal;
    }
    myCode << ";\n";
    level--;
  }

  myCode << "begin\n";

  // process pre-action
//  myCode << firingPreAction;
//  firingPreAction.initialize();
  // Add in port to variable transfers here from firingPortVarList.
  VHDLPortVarListIter nextPortVar(firingPortVarList);
  VHDLPortVar* nPortVar;
  while ((nPortVar = nextPortVar++) != 0) {
    myCode << nPortVar->variable;
    myCode << " := ";
    myCode << nPortVar->name;
    myCode << ";\n";
  }

  // process action
  myCode << firingAction;
  firingAction.initialize();

  // process post-action
//  myCode << firingPostAction;
//  firingPostAction.initialize();
  // Add in variable to port transfers here from firingVarPortList.
  VHDLPortVarListIter nextVarPort(firingVarPortList);
  VHDLPortVar* nVarPort;
  while ((nVarPort = nextVarPort++) != 0) {
    myCode << nVarPort->name;
    myCode << " <= ";
    myCode << nVarPort->variable;
    myCode << ";\n";
  }

  myCode << "end process;\n";
  myCode << "end ";
  myCode << "behavior";
  myCode << ";\n";

  StringList label = targetNestedSymbol.get();
  label << "_proc";
  StringList name = targetNestedSymbol.get();

  VHDLPortList* portList = firingPortList.newCopy();
  VHDLGenericList* genList = firingGenericList.newCopy();
  VHDLPortMapList* portMapList = firingPortMapList.newCopy();
  VHDLGenericMapList* genMapList = firingGenericMapList.newCopy();
  VHDLSignalList* signalList = firingSignalList.newCopy();

  registerCompDecl(name, portList, genList);
  registerSignalList(signalList);
  registerCompMap(label, name, portMapList, genMapList);

  if (!status) {
    return status;
  }
  if (s->isItFork()) {
    return status;
  }
  return status;
}

// Initial stage of code generation.
void StructTarget :: headerCode() {
  // Generate the entity_declaration.
  entity_declaration << "entity ";
  entity_declaration << galaxy()->name();
  entity_declaration << " is\n";
  entity_declaration << "end ";
  entity_declaration << galaxy()->name();
  entity_declaration << ";\n";

  // Generate the architecture_body_opener.
  architecture_body_opener << "architecture ";
  architecture_body_opener << "structure";
  architecture_body_opener << " of ";
  architecture_body_opener << galaxy()->name();
  architecture_body_opener << " is\n";
}

// Trailer code.
void StructTarget :: trailerCode() {
  int level = 0;
  // Add in component declarations here from compDeclList.
  VHDLCompDeclListIter nextCompDecl(compDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    level++;
    component_declarations << indent(level);
    component_declarations << "component ";
    component_declarations << compDecl->name;
    component_declarations << "\n";

    if (!(compDecl->portList->head())) {
      Error::error("Empty compDecl port list!");
    }
    if (compDecl->portList->head()) {
      level++;
      component_declarations << indent(level);
      component_declarations << "port(\n";
      // Add in port refs here from portList.
      VHDLPortListIter nextPort(*(compDecl->portList));
      VHDLPort* nport;
      int portCount = 0;
      while ((nport = nextPort++) != 0) {
	if (portCount) {
	  component_declarations << ";";
	  component_declarations << "\n";
	}
	level++;
	component_declarations << indent(level);
	component_declarations << nport->name;
	component_declarations << ": ";
	component_declarations << nport->direction;
	component_declarations << " ";
	component_declarations << nport->type;
	portCount++;
	level--;
      }
      component_declarations << "\n";
      component_declarations << indent(level);
      component_declarations << ");\n";
      level--;
    }
    
    if (compDecl->genList->head()) {
      level++;
      component_declarations << indent(level);
      component_declarations << "generic(\n";
      // Add in generic refs here from genList.
      VHDLGenericListIter nextGen(*(compDecl->genList));
      VHDLGeneric* ngen;
      int genCount = 0;
      while ((ngen = nextGen++) != 0) {
	if (genCount) {
	  component_declarations << ";";
	  component_declarations << "\n";
	}
	level++;
	component_declarations << indent(level);
	component_declarations << ngen->name;
	component_declarations << ": ";
	component_declarations << ngen->type;
	if ((ngen->defaultVal).numPieces() > 0) {
	  component_declarations << " := ";
	  component_declarations << ngen->defaultVal;
	}
	genCount++;
	level--;
      }
      component_declarations << "\n";
      component_declarations << indent(level);
      component_declarations << ");\n";
      level--;
    }
    
    component_declarations << indent(level);
    component_declarations << "end component;\n";
    level--;
  }

  // Add in signal declarations here from signalList.
  VHDLSignalListIter nextSignal(signalList);
  VHDLSignal* signal;
  while ((signal = nextSignal++) != 0) {
    level++;
    signal_declarations << indent(level);
    signal_declarations << "signal ";
    signal_declarations << signal->name;
    signal_declarations << ": ";
    signal_declarations << signal->type;
    signal_declarations << ";\n";
    level--;
  }

  // Add in component mappings here from compMapList.
  VHDLCompMapListIter nextCompMap(compMapList);
  VHDLCompMap* compMap;
  while ((compMap = nextCompMap++) != 0) {
    level++;
    component_mappings << indent(level);
    component_mappings << compMap->label;
    component_mappings << ": ";
    component_mappings << compMap->name;
    component_mappings << "\n";

    if (compMap->portMapList->head()) {
      level++;
      component_mappings << indent(level);
      component_mappings << "port map(\n";
      // Add in port maps here from portMapList.
      VHDLPortMapListIter nextPortMap(*(compMap->portMapList));
      VHDLPortMap* nportmap;
      int portCount = 0;
      while ((nportmap = nextPortMap++) != 0) {
	if (portCount) {
	  component_mappings << ",";
	  component_mappings << "\n";
	}
	level++;
	component_mappings << indent(level);
	component_mappings << nportmap->name;
	component_mappings << " => ";
	component_mappings << nportmap->mapping;
	portCount++;
	level--;
      }
      component_mappings << "\n";
      component_mappings << indent(level);
      component_mappings << ")\n";
      level--;
    }
    
    if (compMap->genMapList->head()) {
      level++;
      component_mappings << indent(level);
      component_mappings << "generic map(\n";
      // Add in generic maps here from genMapList.
      VHDLGenericMapListIter nextGenMap(*(compMap->genMapList));
      VHDLGenericMap* ngenmap;
      int genCount = 0;
      while ((ngenmap = nextGenMap++) != 0) {
	if (genCount) {
	  component_mappings << ",";
	  component_mappings << "\n";
	}
	level++;
	component_mappings << indent(level);
	component_mappings << ngenmap->name;
	component_mappings << " => ";
	component_mappings << ngenmap->mapping;
	level--;
      }
      component_mappings << "\n";
      component_mappings << indent(level);
      component_mappings << ")\n";
      level--;
    }

    component_mappings << indent(level);
    component_mappings << ";\n";
    level--;
  }

  // Generate the architecture_body_closer.
  architecture_body_closer << "end ";
  architecture_body_closer << "structure";
  architecture_body_closer << ";\n";
}

// Combine all sections of code.
void StructTarget :: frameCode() {
  StringList code = headerComment();

  myCode << "-- entity_declaration\n";
  myCode << "\n" << entity_declaration;
  myCode << "-- architecture_body_opener\n";
  myCode << "\n" << architecture_body_opener;
  myCode << "-- component_declarations\n";
  myCode << "\n" << component_declarations;
  myCode << "-- signal_declarations\n";
  myCode << "\n" << signal_declarations;
  myCode << "\n" << "begin";
  myCode << "-- component_mappings\n";
  myCode << "\n" << component_mappings;
  myCode << "-- architecture_body_closer\n";
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
  // Do nothing, return nothing.
}

// Run the code.
int StructTarget :: runCode() {
  // Do nothing, return nothing.
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
  myCode << "\n";
  myCode << indent(depth);
  if (repetitions == -1)          // iterate infinitely
    myCode += "while TRUE loop\n";
  else {
    myCode << indent(depth)
	   << "variable " << targetNestedSymbol.push("i") << ": INTEGER;\n";
    myCode << indent(depth)
	   << "for " << targetNestedSymbol.pop() << " in 1 to "
	   << repetitions << " loop\n";
  }
  return;
}

// Generate code to end an iterative procedure
void StructTarget :: endIteration(int /*reps*/, int depth) {
    myCode << indent(depth)
	   << "end loop;     -- end repeat, depth " << depth << "\n";
}

// Declare PortHole buffer.
StringList StructTarget :: declBuffer(const VHDLPortHole* /*port*/) {
  StringList dec;
  return dec;
}

// Declare State variable.
StringList StructTarget :: declState(const State* /*state*/, const
					char* /*name*/) {
  StringList dec;
  return dec;
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
        } else {
                out = sanitizedName(obj);
        }
        return out;
}

// Register component declaration.
void StructTarget :: registerCompDecl(StringList name,
				      VHDLPortList* portList,
				      VHDLGenericList* genList) {
  if (compDeclList.inList(name)) return;
  
  // Allocate memory for a new VHDLCompDecl and put it in the list.
  VHDLCompDecl* newCompDecl = new VHDLCompDecl;
  newCompDecl->name = name;
  newCompDecl->portList = portList;
  newCompDecl->genList = genList;
  compDeclList.put(*newCompDecl);
}

// Register each signal in the star signal list.
void StructTarget :: registerSignalList(VHDLSignalList* starSignalList) {
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
  if (compMapList.inList(name)) return;
  
  // Allocate memory for a new VHDLCompMap and put it in the list.
  VHDLCompMap* newCompMap = new VHDLCompMap;
  newCompMap->label = label;
  newCompMap->name = name;
  newCompMap->portMapList = portMapList;
  newCompMap->genMapList = genMapList;
  compMapList.put(*newCompMap);
}

// Return the VHDL type corresponding to the State type.
StringList StructTarget :: stateType(const State* st) {
  StringList type;
  
  if (st->isA("IntState") || st->isA("IntArrayState"))
    type = "INTEGER";
  else if (st->isA("ComplexState") || st->isA("ComplexArrayState"))
    type = "complex";
  else if (st->isA("StringState") || st->isA("StringArrayState"))
    type = "char*";
  else type = "REAL";

  return type;
}

// Register the temporary storage reference.
void StructTarget :: registerTemp(const char* temp, const char* type) {
  StringList ref = sanitize(temp);
  StringList dtyp;

  if (firingVariableList.inList(ref)) return;
  
  if (!strcmp(type,"INT") || !strcmp(type,"int")) dtyp << "INTEGER";
  else if (!strcmp(type,"COMPLEX") || !strcmp(type,"complex")) dtyp << "INTEGER";
  else dtyp << "REAL";

  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->name = ref;
  newvar->type = dtyp;
  newvar->initVal.initialize();
  firingVariableList.put(*newvar);
}

// Register the State reference.
void StructTarget :: registerState(State* state, int thisFiring/*=-1*/) {

  StringList temp = sanitizedFullName(*state);
  StringList ref = sanitize(temp);
  StringList root = ref;

  if (thisFiring >= 0) ref << "_" << thisFiring;

  StringList refIn = sanitize(temp);
  if (thisFiring >= 0) refIn << "_" << thisFiring;
  refIn << "_In";

  StringList refOut = sanitize(temp);
  if (thisFiring >= 0) refOut << "_" << thisFiring;
  refOut << "_Out";

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
  VHDLState* listState;
  while ((nState = nextState++) != 0) {
    StringList tempRoot = root;
    StringList tempName = nState->name;
    if (!strcmp(tempRoot,tempName)) {
      stateLastFiring = nState->lastFiring;
      listState = nState;
    }
  }
  
  int isFirstStateRef = (stateLastFiring == -1);
  
  // If it's the first firing to refer to this state,
  // make ports for it in & out, leave them open.
  if (isFirstStateRef) {
    if (!(firingVariableList.inList(ref))) {
      // Allocate memory for a new VHDLVariable and put it in the list.
      VHDLVariable* newVar = new VHDLVariable;
      newVar->name = ref;
      newVar->type = stateType(state);
      newVar->initVal.initialize();
      firingVariableList.put(*newVar);
    }

    if (!(firingPortVarList.inList(refIn))) {
      // Allocate memory for a new VHDLPortVar and put it in the list.
      VHDLPortVar* portVar = new VHDLPortVar;
      portVar->name = refIn;
      portVar->variable = ref;
      firingPortVarList.put(*portVar);
    }

    if (!(firingVarPortList.inList(refOut))) {
      // Allocate memory for a new VHDLPortVar and put it in the list.
      VHDLPortVar* portVar = new VHDLPortVar;
      portVar->name = refOut;
      portVar->variable = ref;
      firingVarPortList.put(*portVar);
    }

    if (!(firingPortList.inList(refIn))) {
      // Allocate memory for a new VHDLPort and put it in the list.
      VHDLPort* newportIn = new VHDLPort;
      newportIn->name = refIn;
      newportIn->direction = "IN";
      newportIn->type = stateType(state);
      firingPortList.put(*newportIn);
    }
    
    if (!(firingPortList.inList(refOut))) {
      // Allocate memory for a new VHDLPort and put it in the list.
      VHDLPort* newportOut = new VHDLPort;
      newportOut->name = refOut;
      newportOut->direction = "OUT";
      newportOut->type = stateType(state);
      firingPortList.put(*newportOut);
    }

    if (!(firingPortMapList.inList(refIn))) {
      // Allocate memory for a new VHDLPortMap and put it in the list.
      VHDLPortMap* newportMapIn = new VHDLPortMap;
      newportMapIn->name = refIn;
      newportMapIn->mapping.initialize();
      firingPortMapList.put(*newportMapIn);
    }

    if (!(firingPortMapList.inList(refOut))) {
      // Allocate memory for a new VHDLPortMap and put it in the list.
      VHDLPortMap* newportMapOut = new VHDLPortMap;
      newportMapOut->name = refOut;
      newportMapOut->mapping.initialize();
      firingPortMapList.put(*newportMapOut);
    }
  }

  // If this isn't the first firing to refer to this state,
  // and it's the first time for this firing to refer to the state,
  // make in & out ports, and connect state's in port to the state's
  // out port of the last firing to ref the state; leave this firing's
  // out port open.
  cout << "pointA\n";
  if (!isFirstStateRef) {
  cout << "pointB\n";
    // If lastFiring different from thisFiring, need to make connection.
    if (listState->lastFiring != thisFiring) {
  cout << "pointC\n";
      // Contstruct the name of the last ref to the state.
      StringList stateSignal = root;
      stateSignal << "_" << listState->lastFiring << "_Out";

      if (!(firingSignalList.inList(stateSignal))) {
  cout << "pointD\n";
	// Allocate memory for a new VHDLSignal and put it in the list.
	VHDLSignal* newSignal = new VHDLSignal;
	newSignal->name = stateSignal;
	newSignal->type = stateType(state);
	newSignal->from = stateSignal;
	newSignal->to = refIn;
	firingSignalList.put(*newSignal);
      }

  cout << "pointE\n";
      // Get the port map from the port map list and map it to this signal.
      VHDLCompMapListIter compMapNext(compMapList);
      VHDLCompMap* nCompMap;
      while ((nCompMap = compMapNext++) != 0) {
  cout << "pointF\n";
	VHDLPortMapListIter portMapNext(*(nCompMap->portMapList));
	VHDLPortMap* nPortMap;
	while ((nPortMap = portMapNext++) != 0) {
  cout << "pointG\n";
	  // Check for a match.
	  StringList nameTemp = nPortMap->name;
	  StringList stateTemp = stateSignal;
	  cout << "nameTemp  = " << nameTemp << "\n";
	  cout << "stateTemp = " << stateTemp << "\n";
	  cout << "strcmp(,) = " << (!strcmp(nameTemp,stateTemp)) << "\n";
	  if (!strcmp(nameTemp,stateTemp)) {
  cout << "pointH\n";
	    // Found a match.
	    if ((nPortMap->mapping).numPieces() > 0) {
	      Error::error("Port map ", nPortMap->name, " already mapped");
	    }
	    nPortMap->mapping = stateSignal;
	  }
	}
      }

  cout << "pointI\n";
    if (!(firingVariableList.inList(ref))) {
      // Allocate memory for a new VHDLVariable and put it in the list.
      VHDLVariable* newVar = new VHDLVariable;
      newVar->name = ref;
      newVar->type = stateType(state);
      newVar->initVal.initialize();
      firingVariableList.put(*newVar);
    }

    if (!(firingPortVarList.inList(refIn))) {
      // Allocate memory for a new VHDLPortVar and put it in the list.
      VHDLPortVar* portVar = new VHDLPortVar;
      portVar->name = refIn;
      portVar->variable = ref;
      firingPortVarList.put(*portVar);
    }

    if (!(firingVarPortList.inList(refOut))) {
      // Allocate memory for a new VHDLPortVar and put it in the list.
      VHDLPortVar* portVar = new VHDLPortVar;
      portVar->name = refOut;
      portVar->variable = ref;
      firingVarPortList.put(*portVar);
    }

    if (!(firingPortList.inList(refIn))) {
      // Allocate memory for a new VHDLPort and put it in the list.
      VHDLPort* newportIn = new VHDLPort;
      newportIn->name = refIn;
      newportIn->direction = "IN";
      newportIn->type = stateType(state);
      firingPortList.put(*newportIn);
    }
    
    if (!(firingPortList.inList(refOut))) {
      // Allocate memory for a new VHDLPort and put it in the list.
      VHDLPort* newportOut = new VHDLPort;
      newportOut->name = refOut;
      newportOut->direction = "OUT";
      newportOut->type = stateType(state);
      firingPortList.put(*newportOut);
    }

    if (!(firingPortMapList.inList(refIn))) {
      // Allocate memory for a new VHDLPortMap and put it in the list.
      VHDLPortMap* newportMapIn = new VHDLPortMap;
      newportMapIn->name = refIn;
      newportMapIn->mapping = stateSignal;
      firingPortMapList.put(*newportMapIn);
    }

    if (!(firingPortMapList.inList(refOut))) {
      // Allocate memory for a new VHDLPortMap and put it in the list.
      VHDLPortMap* newportMapOut = new VHDLPortMap;
      newportMapOut->name = refOut;
      newportMapOut->mapping.initialize();
      firingPortMapList.put(*newportMapOut);
    }
      
    }
  }
      
  // Reset state's lastFiring tag.
  listState->lastFiring = thisFiring;
}

// Register PortHole reference.
void StructTarget :: registerPortHole(VHDLPortHole* port, int offset=-1) {
  StringList ref = port->getGeoName();
  if (offset >= 0) ref << "_" << offset;

  if (!(firingPortList.inList(ref))) {
    // Allocate memory for a new VHDLPort and put it in the list.
    VHDLPort* newport = new VHDLPort;
    newport->name = ref;
    newport->direction = port->direction();
    newport->type = port->dataType();
    firingPortList.put(*newport);
  }
  
  if (!(firingPortMapList.inList(ref))) {
    // Allocate memory for a new VHDLPortMap and put it in the list.
    VHDLPortMap* newPortMap = new VHDLPortMap;
    newPortMap->name = ref;
    newPortMap->mapping = ref;
    firingPortMapList.put(*newPortMap);
  }
  
  if (!(firingSignalList.inList(ref))) {
    // Allocate memory for a new VHDLSignal and put it in the list.
    VHDLSignal* newSignal = new VHDLSignal;
    newSignal->name = ref;
    newSignal->type = port->dataType();
    newSignal->from = ref;
    newSignal->to = ref;
    firingSignalList.put(*newSignal);
  }
}

ISA_FUNC(StructTarget,VHDLTarget);
