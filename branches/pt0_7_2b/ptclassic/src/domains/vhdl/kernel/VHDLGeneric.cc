static const char file_id[] = "VHDLGeneric.cc";
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

 Methods for VHDL generics.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLGeneric.h"

// Constructors.
VHDLGeneric :: VHDLGeneric() {
  VHDLObj::initialize();
}

VHDLGeneric :: VHDLGeneric(const char* n, Block* p, const char* d) : VHDLObj(n,p,d) {
  VHDLObj::initialize();
}

// Destructor.
VHDLGeneric :: ~VHDLGeneric() {}

// Return a pointer to a new copy of the VHDLGeneric.
VHDLGeneric* VHDLGeneric :: newCopy() {
  VHDLGeneric* newGeneric = new VHDLGeneric;
  newGeneric->name = this->name;
  newGeneric->type = this->type;
  newGeneric->defaultVal = this->defaultVal;

  return newGeneric;
}

// Class identification.
const char* VHDLGeneric :: className() const { return "VHDLGeneric"; }

ISA_FUNC(VHDLGeneric,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLGenericList* VHDLGenericList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLGenericList* newGenericList = new VHDLGenericList;
  VHDLGenericListIter genericNext(*this);
  VHDLGeneric* nextGeneric;
  // Iterate through this list, and create new copies of each entry.
  while ((nextGeneric = genericNext++) != 0) {
    VHDLGeneric* newGeneric = nextGeneric->newCopy();
    newGenericList->put(*newGeneric);
  }

  return newGenericList;
}
