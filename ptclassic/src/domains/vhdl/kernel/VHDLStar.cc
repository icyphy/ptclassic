static const char file_id[] = "VHDLStar.cc";
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

 Programmer: Edward A. Lee, Michael C. Williamson

 This is the baseclass for stars that generate VHDL language code

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLStar.h"
#include "VHDLTarget.h"
#include <fstream.h>

// The following is defined in VHDLDomain.cc -- this forces that module
// to be included if any VHDL stars are linked in.
extern const char VHDLdomainName[];

// My domain.
const char* VHDLStar :: domain() const { return VHDLdomainName; }

// Perform initialization.
void VHDLStar :: initialize() {
  CGStar::initialize();
  firing = 0;
  // Initialize hashstrings for quick comparison.
  hashBLANK = hashstring("");
}

// Run this star.
int VHDLStar :: run() {
  int status = 0;
  firing++;
  status = targ()->runIt(this);
  updateOffsets();
  return status;
}

// Expand macros.  Return empty StringList on error.  ArgList must be
// passed by reference so that the StringList is not consolidated.
StringList VHDLStar :: expandMacro(const char* func, const StringList&
				   argList) {
	StringList s;
	StringListIter arg(argList);
	const char* arg1 = arg++;
	const char* arg2 = arg++;
	const char* arg3 = arg++;

	// pattern match from a range of macros
	if (matchMacro(func, argList, "ref", 2))
	  s = expandRef(arg1, arg2, "");
	else if (matchMacro(func, argList, "ref", 1))
	  s = expandRef(arg1, "", "");
	else if (matchMacro(func, argList, "refCx", 2))
	  s = expandRef(arg1, "", arg2);
	else if (matchMacro(func, argList, "refCx", 3))
	  s = expandRef(arg1, arg2, arg3);
	else if (matchMacro(func, argList, "val", 1))
	  s = expandVal(arg1);
	else if (matchMacro(func, argList, "size", 1))
	  s = expandSize(arg1);
	else if (matchMacro(func, argList, "label", 1))
	  s = codeblockSymbol.lookup(arg1);
	else if (matchMacro(func, argList, "codeblockSymbol", 1))
	  s = codeblockSymbol.lookup(arg1);
	else if (matchMacro(func, argList, "starSymbol", 1))
	  s = starSymbol.lookup(arg1);
	else if (matchMacro(func, argList, "sharedSymbol", 2))
	  s = lookupSharedSymbol(arg1, arg2);
	else if (matchMacro(func, argList, "interOp", 2))
	  s = expandInterOp(arg1, arg2, "");
	else if (matchMacro(func, argList, "interOp", 3))
	  s = expandInterOp(arg1, arg2, arg3);
	else if (matchMacro(func, argList, "assign", 1))
	  s = expandAssign(arg1);
	else if (matchMacro(func, argList, "temp", 2))
	  s = expandTemp(arg1, arg2);
	else if (matchMacro(func, argList, "define", 3))
	  s = expandDefine(arg1, arg2, arg3);
	else macroError(func, argList);

	return s;
}

// Reference to State or PortHole with offset and real/imag part.
StringList VHDLStar :: expandRef(const char* name, const char* offset,
				 const char* part) {
  StringList ref;
  State* state;
  VHDLPortHole* port;
  StringList portName = expandPortName(name);
  State* offsetState;
  StringList offsetVal;

  ref.initialize();

  // Check if offset is a State reference.
  // Convert offset State value to offset.
  if ((offsetState = stateWithName(offset)) != NULL) {
    // Get State value as a string.
    if (offsetState->isA("IntState")) {
      offsetVal = expandVal(offset);
      offset = offsetVal;
    }
    else {
      codeblockError(offset, " is not an IntState");
      ref.initialize();
      return ref;
    }
  }

  // Check if name is a State reference.
  if ((state = stateWithName(name)) != NULL) {

    if (state->isArray()) {
      ref = starSymbol.lookup(state->name());

      // generate constant for index from state
      if (offsetState != NULL) {
	int offsetInt = *(IntState*)offsetState;
	targ()->registerState(state, ref, firing, offsetInt);
	ref << "_P" << offsetInt;
      }

      // generate constant for index
      else {
	// must first convert offset from char* to int
	int offsetInt = 0;
	sscanf(offset, "%d", &offsetInt);
	targ()->registerState(state, ref, firing, offsetInt);
	ref << "_P" << offsetInt;
      }
    }

    // FIXME: Check if offset present before complaining about not array
    // Error:  Referencing a non-array state with an index.
    else {
      // If not an array state, but an offset was specified,
      // you have to wonder.  May want to reactivate this error.
//      codeblockError(name, " is not an array state but is referenced as one");
//      ref.initialize();

      ref = starSymbol.lookup(state->name());
      targ()->registerState(state, ref, firing);
    }
    return ref;
  }

  // Check if name is a PortHole reference.
  // Expand PortHole reference with offset.
  else if ((port = (VHDLPortHole*) genPortWithName(portName)) != NULL) {

    // Error:  Referencing a MultiPortHole as if it were a single PortHole.
    if (multiPortWithName(portName)) {
      codeblockError(portName,
		     " is a MultiPortHole referenced as a single PortHole");
      ref.initialize();
      return ref;
    }

    // Check for embedded buffering.
    if (port->embedded()) {
      cout << "Port " << port->name() << " is embedded\n";
      cout << " in " << port->embedded()->name() << "\n";
      // Take a dare:
      port = (VHDLPortHole*)port->embedded();

    }

    ref << port->getGeoName();

    int offsetInt, tokenNum;

    if (offsetState != NULL) {
      // Generate constant for index from state.
      offsetInt = *(IntState*)offsetState;
    }
    else {
      // Generate constant for index from string.
      // Must first convert offset from char* to int.
      if (hashstring(offset) == hashBLANK) {
	offsetInt = 0;
      }
      else {
	sscanf(offset, "%d", &offsetInt);
      }
    }

    tokenNum = port->getOffset() - offsetInt;

    if (tokenNum >= 0) {
      ref << "_" << tokenNum;
    }
    else { // (tokenNum < 0)
      ref << "_N" << (-tokenNum);
    }

    targ()->registerPortHole(port, ref, firing, tokenNum, part);
  }

  // Error:  Couldn't find a State or a PortHole with given name.
  else {
    codeblockError(name,
		   " is referenced, but not defined as a State or PortHole");
    ref.initialize();
  }

  ref << part;
  return ref;
}

// Expand an expression interspersing an operator in a list of arguments.
StringList VHDLStar :: expandInterOp(const char* oper, const char* args,
				     const char* part) {
  StringList interOp;
  StringList argList;
  StringList finalList;

  // Need to test whether or not it actually uses all remaining
  // tokens in macro arg as the "args" or if it just takes the
  // next one in the comma-separated list.
  
  // First parse out a list of args from the input string.
  while ((*args) != 0) {
    const int MAX_LENGTH = 64;
    static char dest[MAX_LENGTH];
    char* c = dest;
    int i = 0;
    while (((*args == ',') || (*args == ' ')) && (args != 0)) {
      args++;
    }
    while (((*args != ',') || (*args != ' ')) && (i < MAX_LENGTH) &&
	   (*args != 0)) {
      *c++ = *args++;
      i++;
    }
    *c = 0;
    argList << dest;
  }

  // Next go through the list and test each for multi-ness.
  // If multi, then further iterate through each element.
  StringListIter nextArg(argList);
  const char* item;
  MultiPortHole* multiPort;
  while ((item = nextArg++) != 0) {
    if((multiPort = multiPortWithName(item)) != 0) {
      MPHIter nextPort(*multiPort);
      VHDLPortHole* port;
      while ((port = (VHDLPortHole*) nextPort++) != 0) {
	if (hashstring(part) == hashBLANK) {
	  finalList << expandRef(port->name(), "", "");
	}
	else {
	  finalList << expandRef(port->name(), "", part);
	}
      }
    }
    else {
      if (hashstring(part) == hashBLANK) {
	finalList << expandRef(item, "", "");
      }
      else {
	finalList << expandRef(item, "", part);
      }
    }
  }

  // Intersperse the operator string throughout.
  StringListIter finalIter(finalList);
  interOp << finalIter++;
  while ((item = finalIter++) != 0) {
    interOp << " ";
    interOp << oper;
    interOp << " ";
    interOp << item;
  }
  return interOp;
}

// Assignment operator, depending on variable or signal.
const char* VHDLStar :: expandAssign(const char* name) {
  const char* assign;
  State* state;
  VHDLPortHole* port;
  StringList portName = expandPortName(name);

  // Check if it's a PortHole reference.
  if ((port = (VHDLPortHole*) genPortWithName(portName)) != 0) {
    assign = targ()->portAssign();
  }
  // Check if it's a State reference.
  else if ((state = stateWithName(name)) != 0) {
    assign = targ()->stateAssign();
  }
  // Error:  couldn't find a PortHole or a State with given name.
  else {
    codeblockError(name, " is not defined as a state or port");
    assign = "XXX";
  }

  return assign;
}

// Temproary variable reference.
const char* VHDLStar :: expandTemp(const char* name, const char* type) {
  const char* temp;
  State* state;
  VHDLPortHole* port;
  StringList portName = expandPortName(name);

  // Check if it's a PortHole reference.
  if ((port = (VHDLPortHole*) genPortWithName(portName)) != 0) {
    codeblockError(name, " is already defined as a port");
    temp = "XXX";
  }
  // Check if it's a State reference.
  else if ((state = stateWithName(name)) != 0) {
    codeblockError(name, " is already defined as a state");
    temp = "XXX";
  }
  // Register it as temporary storage.
  else {
    temp = starSymbol.lookup(name);
    targ()->registerTemp(temp, type);
  }

  return temp;
}

// Constant variable reference.
const char* VHDLStar :: expandDefine(const char* name, const char* type,
				     const char* init) {
  const char* define;
  State* state;
  VHDLPortHole* port;
  StringList portName = expandPortName(name);

  // Check if it's a PortHole reference.
  if ((port = (VHDLPortHole*) genPortWithName(portName)) != 0) {
    codeblockError(name, " is already defined as a port");
    define = "XXX";
  }
  // Check if it's a State reference.
  else if ((state = stateWithName(name)) != 0) {
    codeblockError(name, " is already defined as a state");
    define = "XXX";
  }
  // Register it as temporary storage.
  else {
    define = starSymbol.lookup(name);
    targ()->registerDefine(define, type, init);
  }

  return define;
}

// Update the offset read and write pointers to the porthole queues.
void VHDLStar :: updateOffsets() {
  // Iterate through all the ports and change pointers according to numXfer()
  BlockPortIter nextPort(*this);
  VHDLPortHole* port;
  while ((port = (VHDLPortHole*) nextPort++) != 0) {
    port->updateOffset();
  }
}

ISA_FUNC(VHDLStar,CGStar);
