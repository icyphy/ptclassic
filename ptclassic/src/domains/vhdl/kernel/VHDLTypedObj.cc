static const char file_id[] = "VHDLTypedObj.cc";
/******************************************************************
Version identification:
@(#)VHDLTypedObj.cc	1.2 07/31/96

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

 Methods for VHDL TypedObjs.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLTypedObj.h"

// Constructors.
VHDLTypedObj :: VHDLTypedObj() {
  type = "";
  VHDLObj::initialize();
}

// Destructor.
VHDLTypedObj :: ~VHDLTypedObj() {}

// Return a pointer to a new copy of the VHDLTypedObj.
VHDLTypedObj* VHDLTypedObj :: newCopy() {
  VHDLTypedObj* newTypedObj = new VHDLTypedObj(name, type);
  return newTypedObj;
}

// Class identification.
const char* VHDLTypedObj :: className() const { return "VHDLTypedObj"; }

ISA_FUNC(VHDLTypedObj,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLTypedObjList* VHDLTypedObjList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLTypedObjList* newTypedObjList = new VHDLTypedObjList;
  VHDLTypedObjListIter TypedObjNext(*this);
  VHDLTypedObj* nextTypedObj;
  // Iterate through this list, and create new copies of each entry.
  while ((nextTypedObj = TypedObjNext++) != 0) {
    VHDLTypedObj* newTypedObj = nextTypedObj->newCopy();
    newTypedObjList->put(*newTypedObj);
  }

  return newTypedObjList;
}

// Allocate memory for a new VHDLTypedObj and put it in the list.
void VHDLTypedObjList :: put(StringList name, StringList type) {
  VHDLTypedObj* newTypedObj = new VHDLTypedObj(name, type);
  this->put(*newTypedObj);
}
