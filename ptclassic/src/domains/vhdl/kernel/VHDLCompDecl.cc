static const char file_id[] = "VHDLCompDecl.cc";
/******************************************************************
Version identification:
@(#)VHDLCompDecl.cc	1.8 07/31/96

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

 Methods for VHDL CompDecls.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLCompDecl.h"

// Constructors.
VHDLCompDecl :: VHDLCompDecl() {
  VHDLObj::initialize();
}

// Destructor.
VHDLCompDecl :: ~VHDLCompDecl() {}

// Return a pointer to a new copy of the VHDLCompDecl.
VHDLCompDecl* VHDLCompDecl :: newCopy() {
  VHDLCompDecl* newCompDecl = new VHDLCompDecl;
  newCompDecl->setName(this->name);
  newCompDecl->portList = this->portList->newCopy();
  newCompDecl->genList = this->genList->newCopy();
  newCompDecl->type = this->type;
  newCompDecl->portMapList = this->portMapList->newCopy();
  newCompDecl->genMapList = this->genMapList->newCopy();

  return newCompDecl;
}

// Class identification.
const char* VHDLCompDecl :: className() const { return "VHDLCompDecl"; }

ISA_FUNC(VHDLCompDecl,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLCompDeclList* VHDLCompDeclList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLCompDeclList* newCompDeclList = new VHDLCompDeclList;
  VHDLCompDeclListIter compDeclNext(*this);
  VHDLCompDecl* nextCompDecl;
  // Iterate through this list, and create new copies of each entry.
  while ((nextCompDecl = compDeclNext++) != 0) {
    VHDLCompDecl* newCompDecl = nextCompDecl->newCopy();
    newCompDeclList->put(*newCompDecl);
  }

  return newCompDeclList;
}

// Allocate memory for a new VHDLCompDecl and put it in the list.
void VHDLCompDeclList :: put(StringList name, VHDLPortList* portList,
			     VHDLGenericList* genList,
			     StringList type, VHDLPortList* portMapList,
			     VHDLGenericList* genMapList) {
  VHDLCompDecl* newCompDecl = new VHDLCompDecl(name, portList, genList,
					       type, portMapList, genMapList);
  this->put(*newCompDecl);
}
