static const char file_id[] = "VHDLStar.cc";
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

 Programmer: Edward A. Lee, Michael C. Williamson

 This is the baseclass for stars that generate VHDL language code

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLGeodesic.h"
#include "VHDLStar.h"
#include "VHDLTarget.h"
#include "ComplexState.h"
#include "Tokenizer.h"
#include <ctype.h>
//#include <ostream.h>

// The following is defined in VHDLDomain.cc -- this forces that module
// to be included if any VHDL stars are linked in.
extern const char VHDLdomainName[];

// My domain.
const char* VHDLStar :: domain() const { return VHDLdomainName; }

// Sanitize a string so that it is usable as a VHDL identifier.
const char* VHDLStar :: sanitize(const char* string) {
    const int MAX_LENGTH = 64;
    static char clean[MAX_LENGTH];
    char* c = clean;
    int i = 0;

    // Check for leading digit.
    if (isdigit(*string)) {
	*c++ = '_';
	i++;
    }

    // Replace strange charachters.
    while (++i < MAX_LENGTH && *string) {
	*c++ = isalnum(*string) ? *string : '_';
	string++;
    }
    *c = 0;

    return clean;
}

// Expand macros.  Return empty StringList on error.  ArgList must be
// passed by reference so that the StringList is not consolidated.
StringList VHDLStar :: expandMacro(const char* func, const StringList&
				   argList) {
	StringList s;
	StringListIter arg(argList);
	const char* arg1 = arg++;
	const char* arg2 = arg++;

	// ref2 provided for backward compatibility
	if (matchMacro(func, argList, "ref2", 2)) s = expandRef(arg1, arg2);
	else if (matchMacro(func, argList, "ref", 2)) s = expandRef(arg1, arg2);
	else if (matchMacro(func, argList, "ref", 1)) s = expandRef(arg1);
	else if (matchMacro(func, argList, "val", 1)) s = expandVal(arg1);
	else if (matchMacro(func, argList, "size", 1)) s = expandSize(arg1);
	else if (matchMacro(func, argList, "label", 1)) s = codeblockSymbol.lookup(arg1);
	else if (matchMacro(func, argList, "codeblockSymbol", 1)) s = codeblockSymbol.lookup(arg1);
	else if (matchMacro(func, argList, "starSymbol", 1)) s = starSymbol.lookup(arg1);
	else if (matchMacro(func, argList, "sharedSymbol", 2)) s = lookupSharedSymbol(arg1, arg2);
	else if (matchMacro(func, argList, "interOp", 2)) s = expandInterOp(arg1, arg2);
	else if (matchMacro(func, argList, "assign", 1)) s = expandAssign(arg1);
	else if (matchMacro(func, argList, "temp", 2)) s = expandTemp(arg1, arg2);
	else macroError(func, argList);

	return s;
}

// Reference to State or PortHole.
StringList VHDLStar :: expandRef(const char* name) {
  StringList ref;
  State* state;
  VHDLPortHole* port;
  StringList portName = expandPortName(name);
  
  ref.initialize();
  
  // Check if it's a State reference.
  if ((state = stateWithName(name)) != 0) {
    StringList type;
    StringList initVal;

//    ref << starSymbol.lookup(state->name());
//    ref << targ()->sanitizedFullName(*state);
//    ref << starSymbol.lookup(tempName);
    StringList tempName = targ()->sanitizedFullName(*state);
    ref = sanitize(tempName);
    
    if (firing >= 0) ref << "_" << firing;
    type = targ()->stateType(state);
    initVal = state->initValue();

//    registerState(state);
//    cout << "Call to targ()->registerState(state);\n";
    targ()->registerState(state, firing);

    // Register this variable reference for use by target.
//    registerVariable(ref, type, initVal);
  }
  
  // Check if it's a PortHole reference.
  else if ((port = (VHDLPortHole*) genPortWithName(portName)) != 0) {
    StringList mapping;
    StringList direction;
    StringList type;

    if (multiPortWithName(portName)) {
      codeblockError(portName, " is a multiport referenced as a single port");
      ref.initialize();
      return ref;
    }
    
    ref << port->getGeoName();
    ref << "_";
    ref << port->getOffset();
    
    mapping = ref;
    direction = port->direction();
    type = port->dataType();
    
    targ()->registerPortHole(port, port->getOffset());

    // Register this port reference for use by target.
//    registerPort(ref, direction, type);
    // Register this port mapping for use by target.
//    registerPortMap(ref, mapping);
    // Register the signal mapped to for use by target.
//    registerSignal(ref, type, ref, ref);
  }

  // Error:  couldn't find a State or a PortHole with given name.
  else {
    codeblockError(name, " is not defined as a state or port");
    ref.initialize();
  }
  
  return ref;
}

// Reference to State or PortHole with offset.
StringList VHDLStar :: expandRef(const char* name, const char* offset) {
  StringList ref;
  State *state;
  State *offsetState;
  StringList offsetVal;
  VHDLPortHole* port;
  StringList portName = expandPortName(name);

  ref.initialize();
    
  // Use State value as offset (if such a State exists).
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

  // Expand array State reference.
  if ((state = stateWithName(name)) != NULL) {
    StringList type;
    StringList initVal;

    if (state->isArray()) {
//      Error::error(*state, "...VHDLStar not set up to handle array states.");

//      ref << starSymbol.lookup(state->name());
//      ref << targ()->sanitizedFullName(*state);
//      ref << starSymbol.lookup(tempName);

//      int offsetInt = *(IntState*)offsetState;
//      ref << "_" << offsetInt;
      StringList tempName = targ()->sanitizedFullName(*state);
      ref = sanitize(tempName);

      targ()->registerState(state, firing);

      if (firing >= 0) ref << "_" << firing;
    }
    else {
      codeblockError(name, " is not an array state");
      ref.initialize();
    }
    type = targ()->stateType(state);
    initVal = state->initValue();

    // Register this variable reference for use by target.
//    registerVariable(ref, type, initVal);
  }

  // Expand PortHole reference with offset.
  else if (port = (VHDLPortHole*) genPortWithName(portName)) {
    StringList mapping;
    StringList direction;
    StringList type;

    if (multiPortWithName(portName)) {
      codeblockError(portName, " is a multiport referenced as a single port");
      ref.initialize();
      return ref;
    }
    
    ref << port->getGeoName();

    // Use array notation for large buffers and for embedded buffers
    // which are referenced through a pointer.
    if (port->bufSize() > 1) {
      ref << "_";

      // generate constant for index from state
      if (offsetState != NULL) {
	int offsetInt = *(IntState*)offsetState;
	ref << (port->getOffset() + offsetInt);
    targ()->registerPortHole(port, offsetInt);
      }

      // generate constant for index
      else {
	// must first convert offset from char* to int
	int offsetInt = 0;
	for (int i=0; offset[i]>='0' && offset[i]<='9'; i++) {
	  offsetInt = 10*offsetInt + (offset[i]-'0');
	}
	ref << (port->getOffset() + offsetInt);
    targ()->registerPortHole(port, offsetInt);
      }
    }
    mapping = ref;
    direction = port->direction();
    type = port->dataType();
    
    // Register this port reference for use by target.
//    registerPort(ref, direction, type);
    // Register this port mapping for use by target.
//    registerPortMap(ref, mapping);
    // Register the signal mapped to for use by target.
//    registerSignal(ref, type, ref, ref);
  }

  return ref;
}

// Expand an expression interspersing an operator in a list of arguments.
StringList VHDLStar :: expandInterOp(const char* oper, const char*
				     args) {
  StringList interOp;
  StringList argList;
  StringList finalList;

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
        finalList << expandRef(port->name());
      }
    }
    else {
      finalList << expandRef(item);
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
StringList VHDLStar :: expandAssign(const char* name) {
  StringList assign;
  State* state;
  VHDLPortHole* port;
  StringList portName = expandPortName(name);

  assign.initialize();
  
  // Check if it's a State reference.
  if ((state = stateWithName(name)) != 0) {
    assign << ":=";
  }

  // Check if it's a PortHole reference.
  else if ((port = (VHDLPortHole*) genPortWithName(portName)) != 0) {
    assign << "<=";
  }

  // Error:  couldn't find a State or a PortHole with given name.
  else {
    codeblockError(name, " is not defined as a state or port");
    assign.initialize();
  }

  return assign;
}

// Temproary variable reference.
StringList VHDLStar :: expandTemp(const char* name, const char* type) {
  StringList temp;
  State* state;
  VHDLPortHole* port;
  StringList portName = expandPortName(name);

  temp.initialize();
  
  // Check if it's a State reference.
  if ((state = stateWithName(name)) != 0) {
    codeblockError(name, " is already defined as a state");
  }

  // Check if it's a PortHole reference.
  else if ((port = (VHDLPortHole*) genPortWithName(portName)) != 0) {
    codeblockError(name, " is already defined as a port");
  }

  // Register it as temporary storage.
  else {
    temp = starSymbol.lookup(name);
    targ()->registerTemp(temp, type);
  }

  return temp;
}

// Perform initialization.
void VHDLStar :: initialize() {
	CGStar::initialize();
	firing = 0;
}

// Run this star.
int VHDLStar :: run() {
  int status = 0;
  firing++;
/*
  firingPortList.initialize();
  firingGenericList.initialize();
  firingVariableList.initialize();
  firingPortMapList.initialize();
  firingGenericMapList.initialize();
  */
  status = targ()->runIt(this);
  updateOffsets();
  return status;
}

/*
// Register port reference for use by target.
void VHDLStar :: registerPort(StringList ref, StringList direction,
			      StringList type) {
  if (firingPortList.inList(ref)) return;

  // Allocate memory for a new VHDLPort and put it in the list.
  VHDLPort* newport = new VHDLPort;
  newport->name = ref;
  newport->direction = direction;
  newport->type = type;
  firingPortList.put(*newport);
}
*/
/*
// Register generic reference for use by target.
void VHDLStar :: registerGeneric(StringList ref, StringList type,
				 StringList defaultVal) {
  if (firingGenericList.inList(ref)) return;

  // Allocate memory for a new VHDLGeneric and put it in the list.
  VHDLGeneric* newgen = new VHDLGeneric;
  newgen->name = ref;
  newgen->type = type;
  newgen->defaultVal = defaultVal;
  firingGenericList.put(*newgen);
}
*/
/*
// Register variable reference for use by target.
void VHDLStar :: registerVariable(StringList ref, StringList type,
				  StringList initVal) {
  if (firingVariableList.inList(ref)) return;
  
  // Allocate memory for a new VHDLVariable and put it in the list.
  VHDLVariable* newvar = new VHDLVariable;
  newvar->name = ref;
  newvar->type = type;
  newvar->initVal = initVal;
  firingVariableList.put(*newvar);
}
*/
/*
// Register the signal mapped to for use by target.
void VHDLStar :: registerSignal(StringList name, StringList type,
			    	StringList from, StringList to) {
  if (firingSignalList.inList(name)) return;
  
  // Allocate memory for a new VHDLGenericMap and put it in the list.
  VHDLSignal* newSignal = new VHDLSignal;
  newSignal->name = name;
  newSignal->type = type;
  newSignal->from = from;
  newSignal->to = to;
  firingSignalList.put(*newSignal);
}
*/
/*
// Register port mapping for use by target.
void VHDLStar :: registerPortMap(StringList ref, StringList mapping) {
  if (firingPortMapList.inList(ref)) return;
  
  // Allocate memory for a new VHDLPortMap and put it in the list.
  VHDLPortMap* newPortMap = new VHDLPortMap;
  newPortMap->name = ref;
  newPortMap->mapping = mapping;
  firingPortMapList.put(*newPortMap);
}
*/
/*
// Register generic mapping for use by target.
void VHDLStar :: registerGenericMap(StringList ref, StringList mapping) {
  if (firingGenericMapList.inList(ref)) return;
  
  // Allocate memory for a new VHDLGenericMap and put it in the list.
  VHDLGenericMap* newGenericMap = new VHDLGenericMap;
  newGenericMap->name = ref;
  newGenericMap->mapping = mapping;
  firingGenericMapList.put(*newGenericMap);
}
*/

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
