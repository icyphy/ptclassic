static const char file_id[] = "VHDLMux.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Methods for VHDL muxs.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLMux.h"

// Constructors.
VHDLMux :: VHDLMux() {
  inputList = new VHDLSignalList;
  output = NULL;
  control = NULL;
  type = "UNINITIALIZED";
  VHDLFiring::initialize();
}

// Destructor.
VHDLMux :: ~VHDLMux() {}

// Return a pointer to a new copy of the VHDLMux.
VHDLMux* VHDLMux :: newCopy() {
  VHDLMux* newMux = new VHDLMux;
  newMux->setName(this->name);
  newMux->starClassName = this->starClassName;
  newMux->genericList = this->genericList->newCopy();
  newMux->portList = this->portList->newCopy();
  newMux->decls = this->decls;
  newMux->variableList = this->variableList->newCopy();
  newMux->action = this->action;
  newMux->genericMapList = this->genericMapList->newCopy();
  newMux->portMapList = this->portMapList->newCopy();
  newMux->signalList = this->signalList->newCopy();
  newMux->noSensitivities = this->noSensitivities;
  newMux->noOutclocking = this->noOutclocking;

  return newMux;
}

// Class identification.
const char* VHDLMux :: className() const { return "VHDLMux"; }

ISA_FUNC(VHDLMux,VHDLFiring);

// Return a pointer to a new copy of the list.
VHDLMuxList* VHDLMuxList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLMuxList* newMuxList = new VHDLMuxList;
  VHDLMuxListIter muxNext(*this);
  VHDLMux* nextMux;
  // Iterate through this list, and create new copies of each entry.
  while ((nextMux = muxNext++) != 0) {
    VHDLMux* newMux = nextMux->newCopy();
    newMuxList->put(*newMux);
  }

  return newMuxList;
}
