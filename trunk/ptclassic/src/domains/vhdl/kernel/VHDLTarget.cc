static const char file_id[] = "VHDLTarget.cc";
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

#include "VHDLTarget.h"
#include "VHDLStar.h"
//#include <ostream.h>

// constructor
VHDLTarget :: VHDLTarget(const char* name,const char* starclass,
			 const char* desc) :
HLLTarget(name,starclass,desc) {
  // Add additional codeStreams.
  addCodeStreams();

  // Initialize codeStreams.
  initCodeStreams();

  // Initialize lists.
  firingVariableList.initialize();
  variableList.initialize();
}

// clone
Block* VHDLTarget :: makeNew() const {
  LOG_NEW; return new VHDLTarget(name(), starType(), descriptor());
}

// Keep this at the top of this file to please the compiler.
// Add code to the beginning of a CodeStream instead of the end.
void VHDLTarget :: prepend(StringList code, CodeStream& stream) {
  StringList temp;
  temp << code;
  temp << stream;
  stream.initialize();
  stream << temp;
}

// Add additional codeStreams.
void VHDLTarget :: addCodeStreams() {
  addStream("entity_declaration", &entity_declaration);
  addStream("architecture_body_opener", &architecture_body_opener);
//  addStream("mainDecls", &mainDecls);
  addStream("variable_declarations", &variable_declarations);
//  addStream("mainInit", &mainInit);
  addStream("architecture_body_closer", &architecture_body_closer);
}

// Initialize codeStreams.
void VHDLTarget :: initCodeStreams() {
  entity_declaration.initialize();
  architecture_body_opener.initialize();
//  mainDecls.initialize();
  variable_declarations.initialize();
//  mainInit.initialize();
  architecture_body_closer.initialize();
}

// Setup the target.
void VHDLTarget :: setup() {
	if (galaxy()) setStarIndices(*galaxy()); 
	HLLTarget::setup();
}

// Main routine.
int VHDLTarget :: runIt(VHDLStar* s) {
  StringList code = "\n\t-- star ";
  code << s->fullName() << " (class " << s->className() << ") \n";
  myCode << code;
  // Initialize lists for new firing.
  firingVariableList.initialize();
  // process action; running star modifies myCode.
  int status = ((CGStar*) s)->CGStar::run();

//  VHDLVariableList* varList = s->firingVariableList.newCopy();
  VHDLVariableList* varList = firingVariableList.newCopy();
  registerVariableList(varList);
  
  if (!status) return status;

  if (s->isItFork()) {
    return status;
  }

  return status;
}

// Initial stage of code generation.
void VHDLTarget :: headerCode() {
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
  architecture_body_opener << "behavior";
  architecture_body_opener << " of ";
  architecture_body_opener << galaxy()->name();
  architecture_body_opener << " is\n";
  architecture_body_opener << "begin\n";
}

// Trailer code.
void VHDLTarget :: trailerCode() {
  // Declare this target's associated galaxy.
//  declareGalaxy(*galaxy());

  // Go through registered variables and give them
  // declarations with initialization.
  VHDLVariableListIter nextVariable(variableList);
  VHDLVariable* variable;
  while ((variable = nextVariable++) != 0) {
    variable_declarations << "variable ";
    variable_declarations << variable->name;
    variable_declarations << ": ";
    variable_declarations << variable->type;
    if ((variable->initVal).numPieces() > 0) {
      variable_declarations << " := ";
      variable_declarations << variable->initVal;
    }
    variable_declarations << ";\n";
  }

  // Generate the architecture_body_closer.
  architecture_body_closer << "-- architecture_body_closer\n";
  architecture_body_closer << "wait;\n";
  architecture_body_closer << "end process;\n";
  architecture_body_closer << "end ";
  architecture_body_closer << "behavior";
  architecture_body_closer << ";\n";
}

// Combine all sections of code.
void VHDLTarget :: frameCode() {
  StringList code = headerComment();
  code << "\n" << entity_declaration;
  code << "\n" << architecture_body_opener;
  code << "\n" << "process";
//  code << "\n" << mainDecls;
  code << "\n" << variable_declarations;
  code << "\n" << "begin";
//  code << "\n" << mainInit;
  
  // Prepend the header, declarations, and initialization.
  prepend(code, myCode);

  myCode << "\n" << architecture_body_closer;

  // after generating code, initialize codeStreams again.
  initCodeStreams();

  // Initialize lists.
  firingVariableList.initialize();
  variableList.initialize();
}

// Write the code to a file.
void VHDLTarget :: writeCode() {
  writeFile(myCode,".vhdl",displayFlag);
}

// Compile the code.
int VHDLTarget :: compileCode() {
  // Do nothing.
}

// Run the code.
int VHDLTarget :: runCode() {
  // Do nothing.
}

// Generate a comment.
StringList VHDLTarget :: comment(const char* text, const char* b,
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
void VHDLTarget :: beginIteration(int repetitions, int depth) {
  myCode << "\n";
  myCode << indent(depth);
  if (repetitions == -1)          // iterate infinitely
    myCode += "while TRUE loop\n";
  else {
//    mainDecls << indent(depth)
    variable_declarations << indent(depth)
	   << "variable " << targetNestedSymbol.push("i") << ": INTEGER;\n";
    myCode << indent(depth)
	   << "for " << targetNestedSymbol.pop() << " in 1 to "
	   << repetitions << " loop\n";
  }
  return;
}

// Generate code to end an iterative procedure
void VHDLTarget :: endIteration(int /*reps*/, int depth) {
    myCode << indent(depth)
	   << "end loop;     -- end repeat, depth " << depth << "\n";
}

/*
// Galaxy declarations.
void VHDLTarget :: declareGalaxy(Galaxy& galaxy) {
    GalStarIter starIter(galaxy);
    VHDLStar* star;
    while ((star = (VHDLStar*)starIter++) != NULL)
    {
	if (!star->isItFork()) declareStar(star);
    }
}

// Star declarations.
void VHDLTarget :: declareStar(VHDLStar* star) {
    // States must be declared after the Star has been run because running
    // the Star builds the list of referenced States.
    mainDecls << star->declareStates();
    mainDecls << star->declarePortHoles();

    // States must be initialized before they are used, so this code must
    // appear before other code generated by the Star.
    prepend(star->initCodeStates(), mainInit);
}
*/

/*
// Declare PortHole buffer.
StringList VHDLTarget :: declBuffer(const VHDLPortHole* port) {
  StringList dec;

  if (port->isItOutput()) {
    StringList name = port->getGeoName();

    if (port->bufSize() > 1) {	// declare using subscripts
      int i;
      for (i=0; i<port->bufSize(); i++) {
	dec << "variable ";
	dec << name;
	dec << "_";
	dec << i;
	dec << ": ";

	dec << port->dataType();

//  if (type == INT) dec << "INTEGER";
//  else if (type == COMPLEX) dec << "complex";
//  else dec << "REAL";
	
	dec << ";\n";
      }
    }
    else {
      dec << "variable ";
      dec << name;
      dec << ": ";
	  
      dec << port->dataType();

//  if (type == INT) dec << "INTEGER";
//  else if (type == COMPLEX) dec << "complex";
//  else dec << "REAL";

      dec << ";\n";
    }
  }

  return dec;
}
*/

/*
// Declare State variable.
StringList VHDLTarget :: declState(const State* state, const char*
				      name) {
  StringList dec;

      if (state->isArray()) { // declare using subscripts
      int i;
      for (i=0; i<state->size(); i++) {
	dec << "variable ";
	dec << name;
	dec << "_";
	dec << i;
	dec << ": ";

	dec << stateType(state);

	//  if (state->isA("IntState") || state->isA("IntArrayState"))
	//  dec << "INTEGER";
	//  else if (state->isA("ComplexState") || state->isA("ComplexArrayState"))
	//  dec << "complex";
	//  else if (state->isA("StringState") || state->isA("StringArrayState"))
	//  dec << "char*";
	//  else dec << "REAL";
	
	dec << ";\n";
      }
    }
    else {
      dec << "variable ";
      dec << name;
      dec << ": ";

      if (state->isA("IntState") || state->isA("IntArrayState"))
	dec << "INTEGER";
      else if (state->isA("ComplexState") || state->isA("ComplexArrayState"))
	dec << "complex";
      else if (state->isA("StringState") || state->isA("StringArrayState"))
	dec << "char*";
      else dec << "REAL";

      dec << ";\n";
    }

  return dec;
}
*/

int VHDLTarget :: codeGenInit() {
  // Set the names of all geodesics.
  setGeoNames(*galaxy());
  
  // Call initCode for each star.
  GalStarIter nextStar(*galaxy());
  VHDLStar* s;
  while ((s = (VHDLStar*) nextStar++) != 0) {
    // Nothing needs to be done for forks.
    if (s->isItFork()) continue;
    
/*
  // Generate PortHole initialization code before the Star is run
    // because running the Star modifies the PortHole indices.
    mainInit << s->initCodePortHoles();
    */
    s->initCode();
  }

  return TRUE;
}

// Assign names for each geodesic according to port connections.
void VHDLTarget :: setGeoNames(Galaxy& galaxy) {
  GalStarIter nextStar(galaxy);
  Star* s;
  while ((s = nextStar++) != NULL) {
    BlockPortIter nextPort(*s);
    VHDLPortHole* p;
    while ((p = (VHDLPortHole*) nextPort++) != NULL) {
      if (p->isItInput()) {
	// Create temporary StringLists so as to allow safe (const char*) casts.
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
	// Create temporary StringLists so as to allow safe (const char*) casts.
	StringList sl = sanitizedFullName(*p);
	p->setGeoName(savestring(sl));
      }
    }
  }
}
  
// The only reason for redefining this from HLLTarget
// is to change the separator from "." to "_".
StringList VHDLTarget :: sanitizedFullName (const NamedObj& obj) const {
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

// Return the VHDL type corresponding to the State type.
StringList VHDLTarget :: stateType(const State* st) {
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
void VHDLTarget :: registerTemp(const char* temp, const char* type) {
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
void VHDLTarget :: registerState(State* state, int offset/*=-1*/) {
//  cout << "Call to VHDLTarget::registerState();\n";
//  StringList ref = state->name();
  StringList temp = sanitizedFullName(*state);
  StringList ref = sanitize(temp);

/*
  VHDLVariableListIter varNext(firingVariableList);
  VHDLVariable* nvar;
  // Search for a match from the existing list.
  while ((nvar = varNext++) != 0) {
    // Create temporary StringLists so as to allow safe (const char*) casts.
    StringList refTemp = ref;
    StringList nameTemp = nvar->name;
    // If a match is found, no need to go any further.
    if (!strcmp(refTemp,nameTemp)) return;
  }
  */

  if (firingVariableList.inList(ref)) return;
  
  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->name = ref;
  newvar->type = stateType(state);
  newvar->initVal = state->initValue();
  firingVariableList.put(*newvar);
}

// Register PortHole reference.
void VHDLTarget :: registerPortHole(VHDLPortHole* port, int offset/*=-1*/) {
  StringList ref = port->getGeoName();

/*
  VHDLVariableListIter varNext(firingVariableList);
  VHDLVariable* nvar;
  // Search for a match from the existing list.
  while ((nvar = varNext++) != 0) {
    // Create temporary StringLists so as to allow safe (const char*) casts.
    StringList refTemp = ref;
    StringList nameTemp = nvar->name;
    // If a match is found, no need to go any further.
    if (!strcmp(refTemp,nameTemp)) return;
  }
  */

  if (firingVariableList.inList(ref)) return;
  
  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->name = ref;
  newvar->type = port->dataType();
//  newvar->initVal = "";
  (newvar->initVal).initialize();
  firingVariableList.put(*newvar);
}

// Register each variable in the star variable list.
void VHDLTarget :: registerVariableList(VHDLVariableList* starVariableList) {
  VHDLVariableListIter starVariableNext(*starVariableList);
//  VHDLVariableListIter variableNext(variableList);
  VHDLVariable* nStarVariable;
//  VHDLVariable* nVariable;
  // Scan through the list of variables from the star firing.
  while ((nStarVariable = starVariableNext++) != 0) {
    // Search for a match from the existing list.
    int isNewVariable = 1;

/*    
    variableNext.reset();
    while ((nVariable = variableNext++) != 0) {
      // Create temporary StringLists so as to allow safe (const char*) casts.
      StringList newTemp = nStarVariable->name;
      StringList nameTemp = nVariable->name;
      // If a match is found, no need to go any further.
      if (!strcmp(newTemp,nameTemp)) isNewVariable = 0;
    }
    */

    if (variableList.inList(nStarVariable)) isNewVariable = 0;
    
    if (isNewVariable) {
      // Allocate memory for a new VHDLVariable and put it in the list.
      VHDLVariable* newVariable = new VHDLVariable;
      newVariable = nStarVariable->newCopy();
//      newVariable->name = nStarVariable->name;
//      newVariable->type = nStarVariable->type;
//      newVariable->initVal = nStarVariable->initVal;
      variableList.put(*newVariable);
    }
  }
}

ISA_FUNC(VHDLTarget,HLLTarget);
