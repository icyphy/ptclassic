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
#include "KnownTarget.h"

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
  // process action; running star modifies myCode.
  int status = ((CGStar*) s)->CGStar::run();
  // Change the default stream back.
  defaultStream = &myCode;

  myCode << "\n\t-- firing ";
  myCode << targetNestedSymbol.push(sanitize(s->fullName()));
  myCode << " (class " << s->className() << ") \n";
  myCode << "entity ";
  myCode << targetNestedSymbol.get();
  myCode << " is\n";

  if (s->firingPortList.head()) {
    level++;
    myCode << indent(level);
    myCode << "port(\n";
    // Add in port refs here from firingPortList.
    VHDLPortListIter nextPort(s->firingPortList);
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
  
  if (s->firingGenericList.head()) {
    level++;
    myCode << indent(level);
    myCode << "generic(\n";
    // Add in generic refs here from firingGenericList.
    VHDLGenericListIter nextGeneric(s->firingGenericList);
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
      myCode << " := ";
      myCode << ngen->defaultVal;
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
  VHDLVariableListIter nextVar(s->firingVariableList);
  VHDLVariable* nvar;
  while ((nvar = nextVar++) != 0) {
    level++;
    myCode << indent(level);
    myCode << "variable ";
    myCode << nvar->name;
    myCode << ": ";
    myCode << nvar->type;
    myCode << " := ";
    myCode << nvar->initVal;
    myCode << ";\n";
    level--;
  }

//	declareStar(s);
//	myCode << starDecls;
  myCode << "begin\n";

// process action
  myCode << firingAction;
  firingAction.initialize();
	
  myCode << "end process;\n";
  myCode << "end ";
  myCode << "behavior";
  myCode << ";\n";

// Need to make ports for changing states to communicate them
// between firings - a further complication.
// include portholes of star, but have multiples for inhomogeneity
// try to include states which get passed between firings
	
// At the end of running a star, build a component decl for it
// and build a component mapping for it, and register them.

  StringList label = targetNestedSymbol.get();
  label << "_proc";
  StringList name = targetNestedSymbol.get();

  VHDLPortList* portList = s->firingPortList.newCopy();
  VHDLGenericList* genList = s->firingGenericList.newCopy();
  VHDLPortMapList* portMapList = s->firingPortMapList.newCopy();
  VHDLGenericMapList* genMapList = s->firingGenericMapList.newCopy();
  VHDLSignalList* signalList = s->firingSignalList.newCopy();

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
  entity_declaration << "-- entity_declaration\n";
  entity_declaration << "entity ";
  entity_declaration << galaxy()->name();
  entity_declaration << " is\n";
  entity_declaration << "end ";
  entity_declaration << galaxy()->name();
  entity_declaration << ";\n";

  // Generate the architecture_body_opener.
  architecture_body_opener << "-- architecture_body_opener\n";
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
	component_declarations << " := ";
	component_declarations << ngen->defaultVal;
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

/*
  // Declare this target's associated galaxy.
  declareGalaxy(*galaxy());
  */
  
  // Generate the architecture_body_closer.
  architecture_body_closer << "-- architecture_body_closer\n";
  architecture_body_closer << "end ";
  architecture_body_closer << "structure";
  architecture_body_closer << ";\n";
}

// Combine all sections of code.
void StructTarget :: frameCode() {
  StringList code = headerComment();

  myCode << "\n" << entity_declaration;
  myCode << "\n" << architecture_body_opener;
  myCode << "\n" << component_declarations;
  myCode << "\n" << signal_declarations;
  myCode << "\n" << "begin";
  myCode << "\n" << component_mappings;
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
  // Do nothing.
}

// Run the code.
int StructTarget :: runCode() {
  // Do nothing.
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
    mainDecls << indent(depth)
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

// Star declarations.
void StructTarget :: declareStar(VHDLStar* star) {
  starDecls.initialize();
  
  // States must be declared after the Star has been run because running
    // the Star builds the list of referenced States.
    starDecls << star->declareStates();
    starDecls << star->declarePortHoles();

    // States must be initialized before they are used, so this code must
    // appear before other code generated by the Star.
    prepend(star->initCodeStates(), starInit);
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
    mainInit << s->initCodePortHoles();
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
  VHDLCompDeclListIter compDeclNext(compDeclList);
  VHDLCompDecl* nCompDecl;
  // Search for a match from the existing list.
  while ((nCompDecl = compDeclNext++) != 0) {
    // Create temporary StringLists so as to allow safe (const char*) casts.
    StringList newTemp = name;
    StringList nameTemp = nCompDecl->name;
    // If a match is found, no need to go any further.
    if (!strcmp(newTemp,nameTemp)) return;
  }
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
  VHDLSignalListIter signalNext(signalList);
  VHDLSignal* nStarSignal;
  VHDLSignal* nSignal;
  // Scan through the list of signals from the star firing.
  while ((nStarSignal = starSignalNext++) != 0) {
    // Search for a match from the existing list.
    int isNewSignal = 1;
    signalNext.reset();
    while ((nSignal = signalNext++) != 0) {
      // Create temporary StringLists so as to allow safe (const char*) casts.
      StringList newTemp = nStarSignal->name;
      StringList nameTemp = nSignal->name;
      // If a match is found, no need to go any further.
      if (!strcmp(newTemp,nameTemp)) isNewSignal = 0;
    }
    if (isNewSignal) {
      // Allocate memory for a new VHDLSignal and put it in the list.
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal->name = nStarSignal->name;
      newSignal->type = nStarSignal->type;
      newSignal->from = nStarSignal->from;
      newSignal->to = nStarSignal->to;
      signalList.put(*newSignal);
    }
  }
}

// Register component mapping.
void StructTarget :: registerCompMap(StringList label, StringList name,
				     VHDLPortMapList* portMapList,
				     VHDLGenericMapList* genMapList) {
  VHDLCompMapListIter compMapNext(compMapList);
  VHDLCompMap* nCompMap;
  // Search for a match from the existing list.
  while ((nCompMap = compMapNext++) != 0) {
    // Create temporary StringLists so as to allow safe (const char*) casts.
    StringList newTemp = label;
    StringList labelTemp = nCompMap->label;
    // If a match is found, no need to go any further.
    if (!strcmp(newTemp,labelTemp)) return;
  }
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

ISA_FUNC(StructTarget,VHDLTarget);
