static const char file_id[] = "VHDLTarget.cc";
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

#include "VHDLTarget.h"
#include "CGUtilities.h"
#include "ConversionTable.h"
#include <fstream.h>


// HPPA CC under HPUX10.01 cannot deal with arrays, the message is:
//  'sorry, not implemented: general initializer in initializer lists'
// if we have an array:
//  static TypeConversionTable cgcCnvTable[7] = {
//   {  COMPLEX, 	FIX, 		"CxToFix"	},
// So, we create a class and let it do the work.
class VHDLConversionTable: public ConversionTable {
public:
  VHDLConversionTable() {
    tblRow(  COMPLEX, 	FLOAT,		"CxToFloat"	);
    tblRow(  COMPLEX, 	FIX, 		"CxToFix"	);
    tblRow(  COMPLEX, 	ANYTYPE,	"CxToFloat"	);
    tblRow(  FLOAT, 	COMPLEX,	"FloatToCx"	);
    tblRow(  FLOAT, 	FIX,		"FloatToFix"	);
    tblRow(  FLOAT, 	INT,		"FloatToInt"	);
    tblRow(  FIX,	COMPLEX,	"FixToCx"	);
    tblRow(  FIX,	FLOAT,		"FixToFloat"	);
    tblRow(  FIX,	FIX,		"FixToFix"	);
    tblRow(  FIX,	INT,		"FixToInt"	);
    tblRow(  FIX,	ANYTYPE,	"FixToFloat"	);
    tblRow(  INT,	FLOAT,		"IntToFloat"	);
    tblRow(  INT,	FIX,		"IntToFix"	);
    tblRow(  ANYTYPE, 	COMPLEX,	"FloatToCx"	);  
    tblRow(  ANYTYPE, 	FIX,		"FloatToFix"	);
  }
};
static VHDLConversionTable vhdlConversionTable;

// Constructor
VHDLTarget :: VHDLTarget(const char* name, const char* starclass,
			 const char* desc, const char* assocDomain) :
HLLTarget(name, starclass, desc, assocDomain) {
  // Add additional codeStreams.
  addCodeStreams();

  // Initialize codeStreams.
  initCodeStreams();

  // Initialize lists.
  variableList.initialize();
  localVariableList.initialize();

  // Make states defined in CGTarget settable.
  displayFlag.setAttributes(A_SETTABLE);

  // Set the default to display the code.
  displayFlag.setInitValue("YES");

  // Set the default to not use loop scheduling.
  loopingLevel.setInitValue("0");
  
  // Initialize hashstrings for quick comparison.
  hashINTEGER = hashstring("INTEGER");
  hashINT = hashstring("INT");
  hashint = hashstring("int");
  hashFLOAT = hashstring("FLOAT");
  hashfloat = hashstring("float");
  hashREAL = hashstring("REAL");
  hashreal = hashstring("real");
  hashCOMPLEX = hashstring("COMPLEX");
  hashcomplex = hashstring("complex");
  hashCHARACTER = hashstring("CHARACTER");
  hashIN = hashstring("IN");
  hashOUT = hashstring("OUT");

  // Initialize type conversion table
  typeConversionTable = &vhdlConversionTable;
}

// Clone
Block* VHDLTarget :: makeNew() const {
  LOG_NEW; return new VHDLTarget(name(), starType(), descriptor(),
 				 getAssociatedDomain());
}

// Keep this at the top of this file to please the compiler.
// Add code to the beginning of a CodeStream instead of the end.
void VHDLTarget :: prepend(StringList code, CodeStream& stream) {
  StringList temp;
  temp << code << stream;
  stream.initialize();
  stream << temp;
}

// Register a read or write to an arc and the offset.
void VHDLTarget :: registerArcRef(VHDLPortHole* port, int tokenNum) {
  StringList direction = port->direction();
  const char* hashDirec = hashstring(direction);
  StringList name = port->getGeoName();
  int noSuchArc = 1;
  
  if (!arcList.inList(name)) {
    noSuchArc = 1;
  }
  else {
    noSuchArc = 0;
    // Search through the arc list for an arc with the given name.
    // If one is found, update it's low/high write/read markers.
    VHDLArc* arc = arcList.arcWithName(name);
    {
      {
	noSuchArc = 0;
	if (hashDirec == hashIN) {
	  if (tokenNum < arc->lowRead) arc->lowRead = tokenNum;
	  if (tokenNum > arc->highRead) arc->highRead = tokenNum;
	}
	else if (hashDirec == hashOUT) {
	  if (tokenNum < arc->lowWrite) arc->lowWrite = tokenNum;
	  if (tokenNum > arc->highWrite) arc->highWrite = tokenNum;
	}
	else {
	  Error::error(*port, " is neither IN nor OUT");
	}
      }
    }
  }
  
  // If no arc with the given name is in the list, then create one.
  if (noSuchArc) {
    VHDLArc* newArc = new VHDLArc;
    newArc->setName(name);
    newArc->setType(port->dataType());
    if (hashDirec == hashOUT) {
      newArc->lowWrite = tokenNum;
      newArc->highWrite = tokenNum;
      newArc->lowRead = port->geo().firstGet();
      newArc->highRead = port->geo().firstGet();
    }
    else if (hashDirec == hashIN) {
      newArc->lowWrite = port->geo().firstPut();
      newArc->highWrite = port->geo().firstPut();
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
  initVHDLObjLists();
  initCodeStreams();
}

// Main routine.
int VHDLTarget :: runIt(VHDLStar* s) {
  StringList code = "\n\t-- Star ";
  code << s->fullName() << " (class " << s->className() << ") \n";
  myCode << code;
  // process action; running star modifies myCode.
  int status = ((CGStar*) s)->CGStar::run();

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
  const char* galName = hashstring(galaxy()->name());

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
  myCode << "\n";
  myCode << "-- wrap-around assignments\n";
  VHDLArcListIter nextArc(arcList);
  VHDLArc* arc;
  while ((arc = nextArc++) != 0) {
    const char* hashArcType = hashstring(arc->type);
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
      myCode << destName << " := " << sourceName << ";\n";
      
      // Must also create variables for those lines which are neither read nor
      // written by a $ref() - e.g. if more delays than tokens read.
      // However, do not create a variable if it's a wormhole input.
      // Will have created a system port input instead.

      {
	// Allocate memory for a new VHDLVariable and put it in the list.
	VHDLVariable* newvar = new VHDLVariable;
	newvar->setName(sourceName);
	newvar->type = arc->type;
	if (hashArcType == hashINTEGER) {
	  newvar->initVal = "0";
	}
	else {
	  newvar->initVal = "0.0";
	}
	variableList.put(*newvar);
      }

      {
	// Allocate memory for a new VHDLVariable and put it in the list.
	VHDLVariable* newvar = new VHDLVariable;
	newvar->setName(destName);
	newvar->type = arc->type;
	if (hashArcType == hashINTEGER) {
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
  variable_declarations << addVariableDecls(&variableList);
  variable_declarations << addVariableDecls(&localVariableList);

  /* The old way:
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
  */

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
  
  // myCode contains the main action of the main process
  code << myCode;
  
  code << "\n" << loopCloser;
  code << "\n" << architecture_body_closer;

  myCode = code;
  
  // Initialize lists.
  variableList.initialize();
}

// Write the code to a file.
void VHDLTarget :: writeCode() {
  // Clean up the code by wrapping around long lines as separate lines.
  // !Warning! Not currently working properly!
//wrapAround(&myCode);
  writeFile(myCode,".vhdl",displayFlag);
  singleUnderscore();
}

// FIXME: Need to do this inside before code is generated instead.
// Change all double underscores to single ones to ensure proper identifiers.
void VHDLTarget :: singleUnderscore() {
  StringList command = "";
  command << "cd " << (const char*) destDirectory;
  command << " ; ";
  command << "rm -f " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "sed s/\"__\"/\"_\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  system(command);
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
    // Put this delay here to allow connections to be established in
    // delta-time before any reads or writes are attempted.  If you
    // attempt to read or write before all socket connections get
    // established, you will block and deadlock with the other side
    // that is still waiting to accept a connection.
    loopOpener << indent(depth) << "wait for 1 ns;" << "\n";
  }
  return;
}

// Generate code to end an iterative procedure
void VHDLTarget :: endIteration(int /*reps*/, int depth) {
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
      if (p->isItOutput()) {
	p->setGeoName(symbol(ptSanitize(p->name())));
	if (p->embedded()) {
	  VHDLPortHole* ep = (VHDLPortHole*)p->embedded();
	  p->setGeoName(symbol(ptSanitize(ep->name())));
	  cout << "Setting geo name for port " << p->name() <<
	    " to " << p->getGeoName() << "\n";
	}
      }
    }
  }
  nextStar.reset();
  while ((s = nextStar++) != NULL) {
    BlockPortIter nextPort(*s);
    VHDLPortHole* p;
    while ((p = (VHDLPortHole*) nextPort++) != NULL) {
      if (p->getGeoName() == NULL) {
	p->setGeoName(symbol(ptSanitize(p->name())));
	if (p->embedded()) {
	  VHDLPortHole* ep = (VHDLPortHole*)p->embedded();
	  p->setGeoName(symbol(ptSanitize(ep->name())));
	  cout << "Setting geo name for port " << p->name() <<
	    " to " << p->getGeoName() << "\n";
	}
      }
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
void VHDLTarget :: registerState(State* state, const char* varName,
				 int /*thisFiring =-1*/, int pos/*=-1*/) {
  StringList ref = varName;
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
	else if (state->isA("ComplexArrayState")) {
	  initVal << (*((IntArrayState *) state))[pos];
	}
	else {
	  Error::error(*state, "is not of a known ArrayState type");
	}
      }
    }
  }
  else {
    state->initialize();
    initVal = state->currentValue();
  }

  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->setName(ref);
  newvar->type = stateType(state);
  newvar->initVal = initVal;
  variableList.put(*newvar);
}

// Register PortHole reference.
void VHDLTarget :: registerPortHole(VHDLPortHole* port, const char* varName,
				    int /*firing*/,
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

  ref << part;
  
  //FIXME: May want to do something different if it's a wormhole port.
  //Such as, what was done in StructTarget: make extra port to VHDL module.

  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->setName(ref);
  newvar->type = port->dataType();
  if (hashstring(newvar->type) == hashINTEGER) {
    newvar->initVal = "0";
  }
  else {
    newvar->initVal = "0.0";
  }
  variableList.put(*newvar);
}

// Register the temporary storage reference.
void VHDLTarget :: registerTemp(const char* temp, const char* type) {
  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->setName(temp);
  newvar->type = sanitizeType(type);
  if ((hashstring(newvar->type) == hashINTEGER) ||
      (hashstring(newvar->type) == hashINT) ||
      (hashstring(newvar->type) == hashint)) {
    newvar->initVal = "0";
  }
  else {
    newvar->initVal = "0.0";
  }
  //  variableList.put(*newvar);
  localVariableList.put(*newvar);
}

// Register the constant storage reference.
void VHDLTarget :: registerDefine(const char* define, const char* type,
				  const char* init) {
  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->setName(define);
  newvar->type = sanitizeType(type);
  newvar->initVal = init;
  //  variableList.put(*newvar);
  localVariableList.put(*newvar);
}

// Return the VHDL type corresponding to the State type.
const char* VHDLTarget :: stateType(const State* st) {
  const char* type;
  
  if (st->isA("FloatState") || st->isA("FloatArrayState"))
    type = hashREAL;
  else if (st->isA("IntState") || st->isA("IntArrayState"))
    type = hashINTEGER;
  else if (st->isA("ComplexState") || st->isA("ComplexArrayState"))
    type = hashREAL;
  else if (st->isA("StringState") || st->isA("StringArrayState"))
    type = hashCHARACTER;
  else
    type = hashREAL;

  return type;
}

// Return the VHDL type corresponding to the given const char*.
const char* VHDLTarget :: sanitizeType(const char* ctyp) {
  const char* type;
  const char* hashtype = hashstring(ctyp);
  
  if ((hashtype == hashFLOAT) || (hashtype == hashfloat) ||
      (hashtype == hashREAL) || (hashtype == hashreal))
    type = hashREAL;
  else if ((hashtype == hashINTEGER) ||
	   (hashtype == hashINT) || (hashtype == hashint))
    type = hashINTEGER;
  else if ((hashtype == hashCOMPLEX) || (hashtype == hashcomplex))
    type = hashREAL;
  else
    type = hashstring("UNRECOGNIZED_TYPE");

  return type;
}

ISA_FUNC(VHDLTarget,HLLTarget);

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
  addStream("simWrapup", &simWrapup);
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
  simWrapup.initialize();
}

// Initialize VHDLObjLists.
void VHDLTarget :: initVHDLObjLists() {
  arcList.initialize();
  variableList.initialize();
  localVariableList.initialize();
}

// Return a set of comments based on firingList.
StringList VHDLTarget :: addFiringComments(VHDLFiringList* firingList,
					   int level/*=0*/) {
  StringList all;
  if (firingList->head()) {
    StringList body;

    level++;

    int firCount = 0;
    VHDLFiringListIter nextFiring(*firingList);
    VHDLFiring* nfir;
    while ((nfir = nextFiring++) != 0) {
      level++;
      body << indent(level) << "-- Firing " << nfir->name << " (Star class "
	   << nfir->starClassName << ");\n";
      firCount++;
      level--;
    }
    level--;

    if (firCount) {
      all << body;
    }
  }
  return all;
}

// Return a generic clause based on genList.
StringList VHDLTarget :: addGenericRefs(VHDLGenericList* genList, int level/*=0*/) {
  StringList all;
  if (genList->head()) {
    StringList opener, body, closer;

    level++;
    opener << indent(level) << "generic(\n";

    int genCount = 0;
    VHDLGenericListIter nextGeneric(*genList);
    VHDLGeneric* ngen;
    while ((ngen = nextGeneric++) != 0) {
      level++;
      if (genCount) {
	body << ";\n";
      }
      body << indent(level) << ngen->name << ": " << ngen->type;
      if (ngen->defaultVal.length() > 0) {
	body << " := " << ngen->defaultVal;
      }
      genCount++;
      level--;
    }
    closer << "\n";
    closer << indent(level) << ");\n";
    level--;

    if (genCount) {
      all << opener << body << closer;
    }
  }
  return all;
}

// Return a port clause based on portList.
StringList VHDLTarget :: addPortRefs(VHDLPortList* portList, int level/*=0*/) {
  StringList all;
  if (portList->head()) {
    StringList opener, body, closer;

    level++;
    opener << indent(level) << "port(\n";

    int portCount = 0;
    VHDLPortListIter nextPort(*portList);
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
    closer << "\n";
    closer << indent(level) << ");\n";
    level--;

    if (portCount) {
      all << opener << body << closer;
    }
  }
  return all;
}

// Add in signal declarations here from signalList.
StringList VHDLTarget :: addSignalDeclarations(VHDLSignalList* signalList,
					       int level/*=0*/) {
  StringList all;
  VHDLSignalListIter nextSignal(*signalList);
  VHDLSignal* signal;
  while ((signal = nextSignal++) != 0) {
    level++;
    all << indent(level) << "signal " << signal->name << ": "
	<< signal->type << ";\n";
    level--;
  }
  return all;
}

// Return component declarations based on compDeclList.
StringList VHDLTarget :: addComponentDeclarations(VHDLCompDeclList* compDeclList,
						    int level/*=0*/) {
  // HashTable to keep track of which components already declared.
  HashTable myTable;
  myTable.clear();

  StringList all;
  VHDLCompDeclListIter nextCompDecl(*compDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    if (!(myTable.hasKey(compDecl->type))) {
      myTable.insert(compDecl->type, compDecl);

      level++;
      all << indent(level) << "component " << compDecl->type << "\n";

      // Add in generic refs here from genList.
      if (compDecl->genList->head()) {
	level++;
	all << indent(level) << "generic(\n";
	VHDLGenericListIter nextGen(*(compDecl->genList));
	VHDLGeneric* ngen;
	int genCount = 0;
	while ((ngen = nextGen++) != 0) {
	  level++;
	  if (genCount) {
	    all << ";\n";
	  }
	  all << indent(level) << ngen->name << ": " << ngen->type;
	  if (ngen->defaultVal.length() > 0) {
	    all << " := " << ngen->defaultVal;
	  }
	  genCount++;
	  level--;
	}
	all << "\n";
	all << indent(level) << ");\n";
	level--;
      }
    
      // Add in port refs here from portList.
      if (compDecl->portList->head()) {
	level++;
	all << indent(level) << "port(\n";
	VHDLPortListIter nextPort(*(compDecl->portList));
	VHDLPort* nport;
	int portCount = 0;
	while ((nport = nextPort++) != 0) {
	  level++;
	  if (portCount) {
	    all << ";\n";
	  }
	  all << indent(level) << nport->name << ": " << nport->direction << " "
	      << nport->type;
	  portCount++;
	  level--;
	}
	all << "\n";
	all << indent(level) << ");\n";
	level--;
      }
    
      all << indent(level) << "end component;\n";
      level--;
    }
  }
  return all;
}

// Return component mappings based on compDeclList.
StringList VHDLTarget :: addComponentMappings(VHDLCompDeclList* compDeclList,
					      int level/*=0*/) {
  StringList all;
  VHDLCompDeclListIter nextCompDecl(*compDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    level++;
    all << indent(level) << compDecl->name << ": " << compDecl->type << "\n";

    // Add in generic maps here from genMapList.
    if (compDecl->genList->head()) {
      level++;
      all << indent(level) << "generic map(\n";
      VHDLGenericListIter nextGen(*(compDecl->genList));
      VHDLGeneric* ngen;
      int genCount = 0;
      while ((ngen = nextGen++) != 0) {
	level++;
	if (genCount) {
	  all << ",\n";
	}
	all << indent(level) << ngen->name << " => " << ngen->mapping;
	genCount++;
	level--;
      }
      all << "\n";
      all << indent(level) << ")\n";
      level--;
    }

    // Add in port maps here from portMapList.
    if (compDecl->portList->head()) {
      level++;
      all << indent(level) << "port map(\n";
      VHDLPortListIter nextPort(*(compDecl->portList));
      VHDLPort* nport;
      int portCount = 0;
      while ((nport = nextPort++) != 0) {
	level++;
	if (portCount) {
	  all << ",\n";
	}
	all << indent(level) << nport->name << " => " << nport->mapping;
	portCount++;
	level--;
      }
      all << "\n";
      all << indent(level) << ")\n";
      level--;
    }
    
    all << indent(level) << ";\n";
    level--;
  }
  return all;
}

// Return variable declarations based on variableList.
StringList VHDLTarget :: addVariableDecls(VHDLVariableList* varList,
					  int level/*=0*/) {
  StringList all;
  VHDLVariableListIter nextVariable(*varList);
  VHDLVariable* variable;
  while ((variable = nextVariable++) != 0) {
    level++;
    all << "variable ";
    all << variable->name;
    all << ": ";
    all << variable->type;
    if ((variable->initVal).length() > 0) {
      all << " := ";
      all << variable->initVal;
    }
    all << ";\n";
    level--;
  }
  return all;
}
