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

 Methods for VHDL firings.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLFiring.h"

// Constructors.
VHDLFiring :: VHDLFiring() {
  starClassName = "UNINITIALIZED";
  genericList = new VHDLGenericList;
  portList = new VHDLPortList;
  variableList = new VHDLVariableList;
  // Leave action blank since it's optional.
  action = "";
  genericMapList = new VHDLGenericList;
  portMapList = new VHDLPortList;
  signalList = new VHDLSignalList;
  // Leave decls blank since it's optional.
  decls = "";
  noSensitivities = 0;
  noOutclocking = 0;
  VHDLObj::initialize();
}

// Destructor.
VHDLFiring :: ~VHDLFiring() {}

// Return a pointer to a new copy of the VHDLFiring.
VHDLFiring* VHDLFiring :: newCopy() {
  VHDLFiring* newFiring = new VHDLFiring;
  newFiring->setName(this->name);
  newFiring->starClassName = this->starClassName;
  newFiring->genericList = this->genericList->newCopy();
  newFiring->portList = this->portList->newCopy();
  newFiring->decls = this->decls;
  newFiring->variableList = this->variableList->newCopy();
  newFiring->action = this->action;
  newFiring->genericMapList = this->genericMapList->newCopy();
  newFiring->portMapList = this->portMapList->newCopy();
  newFiring->signalList = this->signalList->newCopy();
  newFiring->noSensitivities = this->noSensitivities;
  newFiring->noOutclocking = this->noOutclocking;

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
