static const char file_id[] = "VHDLObj.cc";
/******************************************************************
Version identification:
@(#)VHDLObj.cc	1.8 07/31/96

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

 Methods for VHDL objects and VHDL object lists.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLObj.h"

// Constructors.
VHDLObj :: VHDLObj() {
  initialize();
}

// Destructor.
VHDLObj :: ~VHDLObj() {}

// Initializer.
void VHDLObj :: initialize() {
  name = "UNINITIALIZED";
}

// Return a pointer to a new copy of the VHDLObj.
VHDLObj* VHDLObj :: newCopy() {
  VHDLObj* newObj = new VHDLObj(this->name);
  return newObj;
}

// Set the name.
void VHDLObj :: setName(const char* newName) {
  name = hashstring(newName);
}

// Class identification.
const char* VHDLObj :: className() const { return "VHDLObj"; }

ISA_FUNC(VHDLObj,NamedObj);

// Constructors.
VHDLObjList :: VHDLObjList() {
  initialize();
}

// Destructor.
VHDLObjList :: ~VHDLObjList() {}

// Initializer.
void VHDLObjList :: initialize() {
  myTable.clear();
  NamedObjList::initialize();
}

// Add VHDLObj to list.
void VHDLObjList :: put(VHDLObj& v) {
  if (this->inList(&v)) return;
  else {
    NamedObjList::put(v);
    myTable.insert(v.name, &v);
  }
}

// Add a VHDLObjList to this one, one VHDLObj at a time.
void VHDLObjList :: addList(VHDLObjList& from) {
  VHDLObjListIter nextObj(from);
  VHDLObj* nObj;
  while ((nObj = nextObj++) != 0) {
    this->put(*nObj);
  }
}

// Check to see if an item with the given name is in the list.
int VHDLObjList :: inList(VHDLObj* item) {
  return myTable.hasKey(item->name);
}

// StringList argument version.
int VHDLObjList :: inList(StringList item) {
  return myTable.hasKey(item);
}
