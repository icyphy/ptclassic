static const char file_id[] = "VHDLDependency.cc";
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

 Methods for VHDL dependencies.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLDependency.h"

// Constructors.
VHDLDependency :: VHDLDependency() {
  source = NULL;
  sink = NULL;
  VHDLObj::initialize();
}

// Destructor.
VHDLDependency :: ~VHDLDependency() {}

// Return a pointer to a new copy of the VHDLDependency.
VHDLDependency* VHDLDependency :: newCopy() {
  VHDLDependency* newDependency = new VHDLDependency;
  newDependency->setName(this->name);
  newDependency->source = this->source;
  newDependency->sink = this->sink;

  return newDependency;
}

// Class identification.
const char* VHDLDependency :: className() const { return "VHDLDependency"; }

ISA_FUNC(VHDLDependency,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLDependencyList* VHDLDependencyList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLDependencyList* newDependencyList = new VHDLDependencyList;
  VHDLDependencyListIter DependencyNext(*this);
  VHDLDependency* nextDependency;
  // Iterate through this list, and create new copies of each entry.
  while ((nextDependency = DependencyNext++) != 0) {
    VHDLDependency* newDependency = nextDependency->newCopy();
    newDependencyList->put(*newDependency);
  }

  return newDependencyList;
}
