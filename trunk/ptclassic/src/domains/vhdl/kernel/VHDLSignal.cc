static const char file_id[] = "VHDLSignal.cc";
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

 Methods for VHDL signals.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLSignal.h"

// Constructors.
VHDLSignal :: VHDLSignal() {
  VHDLObj::initialize();
}

VHDLSignal :: VHDLSignal(const char* n, Block* p, const char* d) : VHDLObj(n,p,d) {
  VHDLObj::initialize();
}

// Destructor.
VHDLSignal :: ~VHDLSignal() {}

// Return a pointer to a new copy of the VHDLSignal.
VHDLSignal* VHDLSignal :: newCopy() {
  VHDLSignal* newSignal = new VHDLSignal;
  newSignal->name = this->name;
  newSignal->type = this->type;
  newSignal->from = this->from;
  newSignal->to = this->to;

  return newSignal;
}

// Class identification.
const char* VHDLSignal :: className() const { return "VHDLSignal"; }

ISA_FUNC(VHDLSignal,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLSignalList* VHDLSignalList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLSignalList* newSignalList = new VHDLSignalList;
  VHDLSignalListIter signalNext(*this);
  VHDLSignal* nextSignal;
  // Iterate through this list, and create new copies of each entry.
  while ((nextSignal = signalNext++) != 0) {
    VHDLSignal* newSignal = nextSignal->newCopy();
    newSignalList->put(*newSignal);
  }

  return newSignalList;
}
