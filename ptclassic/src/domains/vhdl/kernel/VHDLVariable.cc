static const char file_id[] = "VHDLVariable.cc";
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

 Methods for VHDL variables.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLVariable.h"

// Constructors.
VHDLVariable :: VHDLVariable() {
  VHDLObj::initialize();
}

VHDLVariable :: VHDLVariable(const char* n, Block* p, const char* d) : VHDLObj(n,p,d) {
  VHDLObj::initialize();
}

// Destructor.
VHDLVariable :: ~VHDLVariable() {}

// Return a pointer to a new copy of the VHDLVariable.
VHDLVariable* VHDLVariable :: newCopy() {
  VHDLVariable* newVariable = new VHDLVariable;
  newVariable->name = this->name;
  newVariable->type = this->type;
  newVariable->initVal = this->initVal;

  return newVariable;
}

// Class identification.
const char* VHDLVariable :: className() const { return "VHDLVariable"; }

ISA_FUNC(VHDLVariable,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLVariableList* VHDLVariableList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLVariableList* newVariableList = new VHDLVariableList;
  VHDLVariableListIter variableNext(*this);
  VHDLVariable* nextVariable;
  // Iterate through this list, and create new copies of each entry.
  while ((nextVariable = variableNext++) != 0) {
    VHDLVariable* newVariable = nextVariable->newCopy();
    newVariableList->put(*newVariable);
  }

  return newVariableList;
}

// Allocate memory for a new VHDLVariable and put it in the list.
void VHDLVariableList :: put(StringList name, StringList type,
			     StringList initVal) {
  if (this->inList(name)) return;
  VHDLVariable* newVar = new VHDLVariable;
  newVar->name = name;
  newVar->type = type;
  newVar->initVal = initVal;
  this->put(*newVar);
}
