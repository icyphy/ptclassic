static const char file_id[] = "VHDLPortVar.cc";
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

 Methods for VHDL PortVars.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLPortVar.h"

// Constructors.
VHDLPortVar :: VHDLPortVar() {
  VHDLObj::initialize();
}

// Destructor.
VHDLPortVar :: ~VHDLPortVar() {}

// Return a pointer to a new copy of the VHDLPort.
VHDLPortVar* VHDLPortVar :: newCopy() {
  VHDLPortVar* newPortVar = new VHDLPortVar;
  newPortVar->name = hashstring(this->name);
  newPortVar->variable = this->variable;

  return newPortVar;
}

// Class identification.
const char* VHDLPortVar :: className() const { return "VHDLPortVar"; }

ISA_FUNC(VHDLPortVar,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLPortVarList* VHDLPortVarList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLPortVarList* newPortVarList = new VHDLPortVarList;
  VHDLPortVarListIter PortVarNext(*this);
  VHDLPortVar* nextPortVar;
  // Iterate through this list, and create new copies of each entry.
  while ((nextPortVar = PortVarNext++) != 0) {
    VHDLPortVar* newPortVar = nextPortVar->newCopy();
    newPortVarList->put(*newPortVar);
  }

  return newPortVarList;
}

// Allocate memory for a new VHDLPortVar and put it in the list.
void VHDLPortVarList :: put(StringList name, StringList variable) {
  if (this->inList(name)) return;
  VHDLPortVar* portVar = new VHDLPortVar;
  portVar->name = hashstring(name);
  portVar->variable = variable;
  this->put(*portVar);
}
