static const char file_id[] = "VHDLReg.cc";
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

 Methods for VHDL regs.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLReg.h"

// Constructors.
VHDLReg :: VHDLReg() {
  input = NULL;
  output = NULL;
  clock = NULL;
  type = "UNINITIALIZED";
  VHDLFiring::initialize();
}

// Destructor.
VHDLReg :: ~VHDLReg() {}

// Return a pointer to a new copy of the VHDLReg.
VHDLReg* VHDLReg :: newCopy() {
  VHDLReg* newReg = new VHDLReg;
  newReg->setName(this->name);
  newReg->starClassName = this->starClassName;
  newReg->genericList = this->genericList->newCopy();
  newReg->portList = this->portList->newCopy();
  newReg->decls = this->decls;
  newReg->variableList = this->variableList->newCopy();
  newReg->action = this->action;
  newReg->genericMapList = this->genericMapList->newCopy();
  newReg->portMapList = this->portMapList->newCopy();
  newReg->signalList = this->signalList->newCopy();
  newReg->noSensitivities = this->noSensitivities;
  newReg->noOutclocking = this->noOutclocking;

  newReg->input = this->input;
  newReg->output = this->output;
  newReg->clock = this->clock;
  newReg->type = this->type;

  return newReg;
}

// Class identification.
const char* VHDLReg :: className() const { return "VHDLReg"; }

ISA_FUNC(VHDLReg,VHDLFiring);

// Return a pointer to a new copy of the list.
VHDLRegList* VHDLRegList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLRegList* newRegList = new VHDLRegList;
  VHDLRegListIter regNext(*this);
  VHDLReg* nextReg;
  // Iterate through this list, and create new copies of each entry.
  while ((nextReg = regNext++) != 0) {
    VHDLReg* newReg = nextReg->newCopy();
    newRegList->put(*newReg);
  }

  return newRegList;
}
