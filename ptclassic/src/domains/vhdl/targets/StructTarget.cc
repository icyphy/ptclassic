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
  // Change the default stream temporarily.
  defaultStream = &firingAction;
  // process action; running star modifies myCode.
  int status = ((CGStar*) s)->CGStar::run();
  // Change the default stream back.
  defaultStream = &myCode;

  myCode << "\n\t-- firing ";
  myCode << targetNestedSymbol.push(s->fullName());
  myCode << " (class " << s->className() << ") \n";
  myCode << "entity ";
  myCode << targetNestedSymbol.get();
  myCode << " is\n";

  myCode << "port(\n";
  // Add in port refs here from firingPortList.
  VHDLPortListIter nextPort(s->firingPortList);
  VHDLPort* nport;
  while ((nport = nextPort++) != 0) {
    myCode << nport->name;
    myCode << ": ";
    myCode << nport->direction;
    myCode << " ";
    myCode << nport->type;
    myCode << ";\n";
  }
  myCode << ");\n";

  myCode << "generic(\n";
  // Add in generic refs here from firingGenericList.
  VHDLGenericListIter nextGeneric(s->firingGenericList);
  VHDLGeneric* ngen;
  while ((ngen = nextGeneric++) != 0) {
    myCode << ngen->name;
    myCode << ": ";
    myCode << ngen->type;
    myCode << " := ";
    myCode << ngen->defaultVal;
    myCode << ";\n";
  }
  myCode << ");\n";

  myCode << "end ";
  myCode << targetNestedSymbol.get();
  myCode << "\n";
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
    myCode << nvar->name;
    myCode << ": ";
    myCode << nvar->type;
    myCode << " := ";
    myCode << nvar->initVal;
    myCode << ";\n";
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

// Major re-working is required.
// 1. A new entity for each firing
// 2. A new architecture for each entity
// 3. Proper signals declared to stitch firings together
// 4. Proper signals/variables declared in each architecture
// Add to component declarations list
// Add to signal declarations list
// Add to component mappings list
// Need to declare ports & generics here
// Need to make ports for changing states to communicate them
// between firings - a further complication.
// declaring star should generate starDecls for port spec
// it should also declare signals needed
// and also the component spec for the component list
// and also the port/generic mapping
// include portholes of star, but have multiples for inhomogeneity
// try to include states which get passed between firings
// a: in integer;\n b: in integer;\n c: out integer\n
// perhaps "firingDecls" is more appropriate here than "starDecls"
// doing all star decls is redundant; it gets re-done for each firing.
	
  // Calls to register a new component, register component map
  // register needed signals - use these built lists later to
  // elaborate these things in main vhdl code.

// At the end of running a star, build a component decl for it
// and build a component mapping for it, and register them.

//########################################################################
//
//		Problem! How to save lists from being destroyed?
//
//########################################################################
  // The following are pointer assignments - not actual copying
  // Will need to do actual copy if original reference destructs it.
  StringList label = targetNestedSymbol.get();
  StringList name = targetNestedSymbol.get();
/*
  VHDLPortList* portList = &(s->firingPortList);
  VHDLGenericList* genList = &(s->firingGenericList);
  VHDLPortMapList* portMapList = &(s->firingPortMapList);
  VHDLGenericMapList* genMapList = &(s->firingGenericMapList);
  */

  VHDLPortList* portList = s->firingPortList.newCopy();
  VHDLGenericList* genList = s->firingGenericList.newCopy();
  VHDLPortMapList* portMapList = s->firingPortMapList.newCopy();
  VHDLGenericMapList* genMapList = s->firingGenericMapList.newCopy();

  registerCompDecl(name, portList, genList);
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
  // Add in component declarations here from compDeclList.
  VHDLCompDeclListIter nextCompDecl(compDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    component_declarations << "component ";
    component_declarations << compDecl->name;
    component_declarations << "\n";

    component_declarations << "port(\n";
    // Add in port refs here from portList.
    VHDLPortListIter nextPort(*(compDecl->portList));
    VHDLPort* nport;
    while ((nport = nextPort++) != 0) {
      component_declarations << nport->name;
      component_declarations << ": ";
      component_declarations << nport->direction;
      component_declarations << " ";
      component_declarations << nport->type;
      component_declarations << ";\n";
    }
    component_declarations << ");\n";

    component_declarations << "generic(\n";
    // Add in generic refs here from genList.
    VHDLGenericListIter nextGen(*(compDecl->genList));
    VHDLGeneric* ngen;
    while ((ngen = nextGen++) != 0) {
      component_declarations << ngen->name;
      component_declarations << ": ";
      component_declarations << ngen->type;
      component_declarations << " := ";
      component_declarations << ngen->defaultVal;
      component_declarations << ";\n";
    }
    component_declarations << ");\n";
  
    component_declarations << "end component;\n";
  }

  // Add in component mappings here from compMapList.
  VHDLCompMapListIter nextCompMap(compMapList);
  VHDLCompMap* compMap;
  while ((compMap = nextCompMap++) != 0) {
    component_mappings << compMap->label;
    component_mappings << ": ";
    component_mappings << compMap->name;
    component_mappings << "\n";

    component_mappings << "port map(\n";
    // Add in port maps here from portMapList.
    VHDLPortMapListIter nextPortMap(*(compMap->portMapList));
    VHDLPortMap* nportmap;
    while ((nportmap = nextPortMap++) != 0) {
      component_mappings << nportmap->name;
      component_mappings << " => ";
      component_mappings << nportmap->mapping;
      component_mappings << ",\n";
    }
    component_mappings << ");\n";

    component_mappings << "generic map(\n";
    // Add in generic maps here from genMapList.
    VHDLGenericMapListIter nextGenMap(*(compMap->genMapList));
    VHDLGenericMap* ngenmap;
    while ((ngenmap = nextGenMap++) != 0) {
      component_mappings << ngenmap->name;
      component_mappings << " => ";
      component_mappings << ngenmap->mapping;
      component_mappings << ",\n";
    }
    component_mappings << ");\n";
  
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

ISA_FUNC(StructTarget,VHDLTarget);
