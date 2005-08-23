static const char file_id[] = "VHDLGeneric.cc";
/******************************************************************
Version identification:
@(#)VHDLGeneric.cc	1.10 07/31/96

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Methods for VHDL generics.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLGeneric.h"

// Constructors.
VHDLGeneric :: VHDLGeneric() {
  defaultVal = "";
  mapping = "";
  VHDLTypedObj::initialize();
}

// Destructor.
VHDLGeneric :: ~VHDLGeneric() {}

// Return a pointer to a new copy of the VHDLGeneric.
VHDLGeneric* VHDLGeneric :: newCopy() {
  VHDLGeneric* newGeneric = new VHDLGeneric(name, type, defaultVal, mapping);
  return newGeneric;
}

// Class identification.
const char* VHDLGeneric :: className() const { return "VHDLGeneric"; }

ISA_FUNC(VHDLGeneric,VHDLTypedObj);

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

// Allocate memory for a new VHDLGeneric and put it in the list.
void VHDLGenericList :: put(StringList name, StringList type,
			    StringList defaultVal/*=""*/,
			    StringList mapping/*=""*/) {
  VHDLGeneric* newGeneric = new VHDLGeneric(name, type, defaultVal, mapping);
  this->put(*newGeneric);
}
