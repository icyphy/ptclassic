static const char file_id[] = "VHDLFiring.cc";
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

 Methods for VHDL CompDecls.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLFiring.h"

// Constructors.
VHDLFiring :: VHDLFiring() {
  VHDLObj::initialize();
}

VHDLFiring :: VHDLFiring(const char* n, Block* p, const char* d) : VHDLObj(n,p,d) {
  VHDLObj::initialize();
}

// Destructor.
VHDLFiring :: ~VHDLFiring() {}

// Return a pointer to a new copy of the VHDLFiring.
VHDLFiring* VHDLFiring :: newCopy() {
  VHDLFiring* newFiring = new VHDLFiring;
  newFiring->name = hashstring(this->name);
  newFiring->starClassName = this->starClassName;
  newFiring->genericList = this->genericList->newCopy();
  newFiring->portList = this->portList->newCopy();
  newFiring->variableList = this->variableList->newCopy();
  newFiring->portVarList = this->portVarList->newCopy();
  newFiring->action = this->action;
  newFiring->varPortList = this->varPortList->newCopy();
  newFiring->genericMapList = this->genericMapList->newCopy();
  newFiring->portMapList = this->portMapList->newCopy();
  newFiring->signalList = this->signalList->newCopy();

  return newFiring;
}

// Class identification.
const char* VHDLFiring :: className() const { return "VHDLFiring"; }

ISA_FUNC(VHDLFiring,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLFiringList* VHDLFiringList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLFiringList* newFiringList = new VHDLFiringList;
  VHDLFiringListIter firingNext(*this);
  VHDLFiring* nextFiring;
  // Iterate through this list, and create new copies of each entry.
  while ((nextFiring = firingNext++) != 0) {
    VHDLFiring* newFiring = nextFiring->newCopy();
    newFiringList->put(*newFiring);
  }

  return newFiringList;
}
