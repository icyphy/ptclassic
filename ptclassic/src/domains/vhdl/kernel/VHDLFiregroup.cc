static const char file_id[] = "VHDLFiregroup.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1996-%Q% The Regents of the University of California.
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

 Methods for VHDL firegroups.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLFiregroup.h"

// Constructors.
VHDLFiregroup :: VHDLFiregroup() {
  firingList = new VHDLFiringList;
  VHDLObj::initialize();
}

// Destructor.
VHDLFiregroup :: ~VHDLFiregroup() {}

// Return a pointer to a new copy of the VHDLFiregroup.
VHDLFiregroup* VHDLFiregroup :: newCopy() {
  VHDLFiregroup* newFiregroup = new VHDLFiregroup;
  newFiregroup->setName(this->name);
  newFiregroup->firingList = this->firingList->newCopy();

  return newFiregroup;
}

// Class identification.
const char* VHDLFiregroup :: className() const { return "VHDLFiregroup"; }

ISA_FUNC(VHDLFiregroup,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLFiregroupList* VHDLFiregroupList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLFiregroupList* newFiregroupList = new VHDLFiregroupList;
  VHDLFiregroupListIter firegroupNext(*this);
  VHDLFiregroup* nextFiregroup;
  // Iterate through this list, and create new copies of each entry.
  while ((nextFiregroup = firegroupNext++) != 0) {
    VHDLFiregroup* newFiregroup = nextFiregroup->newCopy();
    newFiregroupList->put(*newFiregroup);
  }

  return newFiregroupList;
}
