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
#include <ostream.h>

// Constructor
VHDLTarget :: VHDLTarget(const char* name, const char* starclass,
			 const char* desc) :
HLLTarget(name, starclass, desc) {
  // Add additional codeStreams.
  addCodeStreams();

  // Initialize codeStreams.
  initCodeStreams();

  // Initialize lists.
  firingVariableList.initialize();
  variableList.initialize();
}

// Clone
Block* VHDLTarget :: makeNew() const {
  LOG_NEW; return new VHDLTarget(name(), starType(), descriptor());
}

// Keep this at the top of this file to please the compiler.
// Add code to the beginning of a CodeStream instead of the end.
void VHDLTarget :: prepend(StringList code, CodeStream& stream) {
  StringList temp;
  temp << code << stream;
  stream.initialize();
  stream << temp;
}

// Add additional codeStreams.
void VHDLTarget :: addCodeStreams() {
  addStream("entity_declaration", &entity_declaration);
  addStream("architecture_body_opener", &architecture_body_opener);
  addStream("variable_declarations", &variable_declarations);
  addStream("architecture_body_closer", &architecture_body_closer);
  addStream("mainDecls", &mainDecls);
  addStream("loopOpener", &loopOpener);
  addStream("loopCloser", &loopCloser);
  addStream("useLibs", &useLibs);
  addStream("sysWrapup", &sysWrapup);
}

// Initialize codeStreams.
void VHDLTarget :: initCodeStreams() {
  entity_declaration.initialize();
  architecture_body_opener.initialize();
  variable_declarations.initialize();
  architecture_body_closer.initialize();
  mainDecls.initialize();
  loopOpener.initialize();
  loopCloser.initialize();
  useLibs.initialize();
  sysWrapup.initialize();
}

// Register a read or write to an arc and the offset.
void VHDLTarget :: registerArcRef(VHDLPortHole* port, int tokenNum) {
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
      newArc->type = port->dataType();
      newArc->lowWrite = tokenNum;
      newArc->highWrite = tokenNum;
      newArc->lowRead = -(port->geo().numInit());
      newArc->highRead = -(port->geo().numInit());
    }
    else if (!strcmp(port->direction(),"IN")) {
      newArc->type = port->dataType();
      newArc->lowWrite = 0;
      newArc->highWrite = 0;
      newArc->lowRead = tokenNum;
      newArc->highRead = tokenNum;
    }
    else {
      Error::error(*port, " is neither IN nor OUT");
    }
    arcList.put(*newArc);
  }
}

// Setup the target.
void VHDLTarget :: setup() {
  if (galaxy()) setStarIndices(*galaxy()); 
  HLLTarget::setup();

  // Initialize codeStreams.
  initCodeStreams();
}

// Main routine.
int VHDLTarget :: runIt(VHDLStar* s) {
  StringList code = "\n\t-- Star ";
  code << s->fullName() << " (class " << s->className() << ") \n";
  myCode << code;
  // Initialize lists for new firing.
  firingVariableList.initialize();
  // process action; running star modifies myCode.
  int status = ((CGStar*) s)->CGStar::run();

  VHDLVariableList* varList = firingVariableList.newCopy();
  mergeVariableList(varList);
  
  if (!status) return status;
  if (s->isItFork()) {
    return status;
  }
  return status;
}

// Redefined from CGTarget to avoid cout messages.
int VHDLTarget :: sendWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	// data are discarded
	cp.forceGrabData();
	return TRUE;
}

// Redefined from CGTarget to avoid cout messages.
int VHDLTarget :: receiveWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	int size = cp.bufSize();
	// insert zero-valued particles onto cp's geodesic
	for (int i = 0; i < size; i++)
		cp%i << 0;
	cp.forceSendData();
	return TRUE;
}

// Initial stage of code generation.
void VHDLTarget :: headerCode() {
  StringList galName = galaxy()->name();

  // Generate the entity_declaration.
  entity_declaration << "-- entity_declaration\n";
  entity_declaration << "entity ";
  entity_declaration << galName;
  entity_declaration << " is\n";
  entity_declaration << "end ";
  entity_declaration << galName;
  entity_declaration << ";\n";

  // Generate the architecture_body_opener.
  architecture_body_opener << "-- architecture_body_opener\n";
  architecture_body_opener << "architecture ";
  architecture_body_opener << "behavior";
  architecture_body_opener << " of ";
  architecture_body_opener << galName;
  architecture_body_opener << " is\n";
  architecture_body_opener << "begin\n";
}

// Trailer code.
void VHDLTarget :: trailerCode() {
  // Iterate through the arc list.  Track read/write refs made on each arc.
  // Determine which variables need to be "wrap-around" assigned.
  VHDLArcListIter nextArc(arcList);
  VHDLArc* arc;
  while ((arc = nextArc++) != 0) {
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
//      connectRegister(sourceName, destName, "INTEGER");
      myCode << destName << " := " << sourceName << ";\n";
      cout << "Connecting " << sourceName << " and " << destName << "\n";
      
      // Must also create variables for those lines which are neither read nor
      // written by a $ref() - e.g. if more delays than tokens read.
      // However, do not create a variable if it's a wormhole input.
      // Will have created a system port input instead.

      // If no system port by the given name, go ahead and make the variable.
//      if (sx < arc->lowWrite) {
      if (!(variableList.inList(sourceName))) {
	// Allocate memory for a new VHDLVariable and put it in the list.
	VHDLVariable* newvar = new VHDLVariable;
	newvar->name = sourceName;
	newvar->type = arc->type;
	if (!strcmp(arc->type,"INTEGER")) {
	  newvar->initVal = "0";
	}
	else {
	  newvar->initVal = "0.0";
	}
	variableList.put(*newvar);
      }
//      if (ix < arc->lowWrite) {
      if (!(variableList.inList(destName))) {
	// Allocate memory for a new VHDLVariable and put it in the list.
	VHDLVariable* newvar = new VHDLVariable;
	newvar->name = destName;
	newvar->type = arc->type;
	if (!strcmp(arc->type,"INTEGER")) {
	  newvar->initVal = "0";
	}
	else {
	  newvar->initVal = "0.0";
	}
	variableList.put(*newvar);
      }
    }
  }
  
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
  CodeStream code;
  code << headerComment();
  code << "\n" << useLibs;
  code << "\n" << entity_declaration;
  code << "\n" << architecture_body_opener;
  code << "\n" << "process";
  // Special inserted line for initCode methods to use
  code << "\n" << mainDecls;
  code << "\n" << variable_declarations;
  code << "\n" << "begin";
  code << "\n" << loopOpener;
  
  // Prepend the header, declarations, and initialization.
//  prepend(code, myCode);
  code << myCode;
  
  code << "\n" << loopCloser;
  code << "\n" << architecture_body_closer;

  myCode = code;
  
  // Initialize lists.
  firingVariableList.initialize();
  variableList.initialize();
}

// Write the code to a file.
void VHDLTarget :: writeCode() {
  // Clean up the code by wrapping around long lines as separate lines.
  // !Warning! Not currently working properly!
//wrapAround(&myCode);
  writeFile(myCode,".vhdl",displayFlag);
}

// Compile the code.
int VHDLTarget :: compileCode() {
  // Return TRUE indicating success.
  return TRUE;
}

// Run the code.
int VHDLTarget :: runCode() {
  // Return TRUE indicating success.
  return TRUE;
}

// Generate code for reading from a wormhole input port.
void VHDLTarget :: wormInputCode(PortHole& p) {
  myCode << "\n";
  myCode << "-- READ from wormhole port " << p.fullName() << "\n";
}

// Generate code for writing to a wormhole output port.
void VHDLTarget :: wormOutputCode(PortHole& p) {
  myCode << "\n";
  myCode << "-- WRITE to wormhole port " << p.fullName() << "\n";
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
  loopOpener << "\n";
  loopOpener << indent(depth);
  // Check for infinite iteration.
  if (repetitions == -1)
    loopOpener += "while TRUE loop\n";
  else {
    variable_declarations << indent(depth) << "variable "
			  << targetNestedSymbol.push("i") << ": INTEGER;\n";
    loopOpener << indent(depth)
	       << "for " << targetNestedSymbol.pop() << " in 1 to "
	       << repetitions << " loop\n";
  }
  return;
}

// Generate code to end an iterative procedure
void VHDLTarget :: endIteration(int /*reps*/, int depth) {
  loopCloser << indent(depth) << "wait for 1 ns;" << "\n";
  loopCloser << indent(depth)
	     << "end loop;     -- end repeat, depth " << depth << "\n";
}

// code generation init routine; compute offsets, generate initCode
int VHDLTarget :: codeGenInit() {
  // Set the names of all geodesics.
  setGeoNames(*galaxy());
  
  // Call initCode for each star.
  GalStarIter nextStar(*galaxy());
  VHDLStar* s;
  while ((s = (VHDLStar*) nextStar++) != 0) {
    // Nothing needs to be done for forks.
    if (s->isItFork()) continue;
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
	// Create temporary StringLists so as to allow
	// safe (const char*) casts.
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
	// Create temporary StringLists so as to allow
	// safe (const char*) casts.
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
  }
  else {
    out = sanitizedName(obj);
  }
  return out;
}

// Merge the Star's variable list with the Target's variable list.
void VHDLTarget :: mergeVariableList(VHDLVariableList* starVariableList) {
  VHDLVariableListIter starVariableNext(*starVariableList);
  VHDLVariable* nStarVariable;
  // Scan through the list of variables from the star firing.
  while ((nStarVariable = starVariableNext++) != 0) {
    // Search for a match from the existing list.
    int isNewVariable = 1;

    if (variableList.inList(nStarVariable)) isNewVariable = 0;
    
    if (isNewVariable) {
      // Allocate memory for a new VHDLVariable and put it in the list.
      VHDLVariable* newVariable = new VHDLVariable;
      newVariable = nStarVariable->newCopy();
      variableList.put(*newVariable);
    }
  }
}

// Clean up the code by wrapping around long lines as separate lines.
void VHDLTarget :: wrapAround(StringList* codeList) {
  // Iterate through codeList, line by line.
  // Lines shall be defined by "\n" characters.
  // For each line, count the number of characters
  // when count exceeds 78, start it as a new line, and
  // keep counting through the current line.
  // When done, pass back the code list.
  StringListIter pieceNext(*codeList);
  const char* currentPiece;
  StringList newLine;
  StringList* newCode = new StringList;
  
  int oldCount = 0; // can go from 0 to whatever.
  int newCount = 0; // can go from 0 to 78, then start a new line.
  // note that \0 end of string chars should not be counted.


  *newCode << "WrapAround Filtered Code!\n";
  
//  while (not at end of codeList)
  while ((currentPiece = pieceNext++) != 0) {
    oldCount = 0;

//  while (not at end of currentPiece)
    while(currentPiece[oldCount]) {
      newLine << currentPiece[oldCount];

//    if (reach end of current line in codeList)
      if (currentPiece[oldCount] == '\n') {
	*newCode << newLine;
	newLine.initialize();
	newCount = 0;
      }

//    else if (number of chars in new line >= 78)
      else if (newCount >= 78) {
	newLine << "\n" << "  ";
	*newCode << newLine;
	newLine.initialize();
	newCount = 0;
      }

      oldCount++;
    }
  }
  
// drop out of codeList, tack final line onto newCode;
  *newCode << newLine;

// return new code listing by reference;
  *codeList = *newCode;
}

// Register the State reference.
void VHDLTarget :: registerState(State* state, int thisFiring/*=-1*/,
				 int pos/*=-1*/) {
  StringList temp = sanitizedFullName(*state);
  StringList ref = sanitize(temp);
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
//          initVal = state[pos].initValue();
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
    initVal = state->initValue();
  }
  
  ref << "_" << thisFiring;

  if (firingVariableList.inList(ref)) return;
  
  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->name = ref;
  newvar->type = stateType(state);
  newvar->initVal = initVal;
  firingVariableList.put(*newvar);
}

// Register PortHole reference.
void VHDLTarget :: registerPortHole(VHDLPortHole* port, int tokenNum/*=-1*/) {
  // The registry keeps track of all refed arcs, and their min/max R/W offsets.
  registerArcRef(port, tokenNum);

// I want to pass along the offset info as well as whether it's
// a read or a write so I can keep tabs on the production window and
// the read window, and then do nice things at the end based on that.
// Also, must do special things if it's a wormhole input.

// Continue to do normal signal naming and portmapping.

  StringList ref = port->getGeoName();
  if (tokenNum >= 0) {
    ref << "_" << tokenNum;
  }
  else { /* (tokenNum < 0) */
    ref << "_N" << (-tokenNum);
  }

  //FIXME: May want to do something different if it's a wormhole port.
  //Such as, what was done in StructTarget: make extra port to VHDL module.

  if (firingVariableList.inList(ref)) return;
  
  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->name = ref;
  newvar->type = port->dataType();
  newvar->initVal = "0.0";
  firingVariableList.put(*newvar);
}

// Register the temporary storage reference.
void VHDLTarget :: registerTemp(const char* temp, const char* type) {
  StringList ref = sanitize(temp);

  if (firingVariableList.inList(ref)) return;
  
  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->name = ref;
  newvar->type = sanitizeType(type);
  newvar->initVal = "0.0";
  firingVariableList.put(*newvar);
}

// Return the assignment operator for States.
const char* VHDLTarget :: stateAssign() {
  const char* assign = ":=";
  return assign;
}

// Return the assignment operator for PortHoles.
const char* VHDLTarget :: portAssign() {
  const char* assign = ":=";
  return assign;
}

// Return the VHDL type corresponding to the State type.
StringList VHDLTarget :: stateType(const State* st) {
  StringList type;
  
  if (st->isA("IntState") || st->isA("IntArrayState"))
    type = "INTEGER";
  else if (st->isA("ComplexState") || st->isA("ComplexArrayState"))
    type = "COMPLEX";
  else if (st->isA("StringState") || st->isA("StringArrayState"))
    type = "CHARACTER";
  else
    type = "REAL";

  return type;
}

// Return the VHDL type corresponding to the given const char*.
StringList VHDLTarget :: sanitizeType(const char* ctyp) {
  StringList type;

  if (!strcmp(ctyp,"INT") || !strcmp(ctyp,"int"))
    type << "INTEGER";
  else if (!strcmp(ctyp,"COMPLEX") || !strcmp(ctyp,"complex"))
    type << "COMPLEX";
  else
    type << "REAL";

  return type;
}

ISA_FUNC(VHDLTarget,HLLTarget);
