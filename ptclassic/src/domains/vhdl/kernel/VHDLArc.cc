static const char file_id[] = "VHDLArc.cc";
/******************************************************************
Version identification:
@(#)VHDLArc.cc	1.7 07/31/96

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

 Methods for VHDL arcs.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLArc.h"

// Constructors.
VHDLArc :: VHDLArc() {
  VHDLTypedObj::initialize();
}

// Destructor.
VHDLArc :: ~VHDLArc() {}

// Return a pointer to a new copy of the VHDLArc.
VHDLArc* VHDLArc :: newCopy() {
  VHDLArc* newArc = new VHDLArc(name, type, lowWrite, highWrite, lowRead,
				highRead);
  return newArc;
}

// Class identification.
const char* VHDLArc :: className() const { return "VHDLArc"; }

ISA_FUNC(VHDLArc,VHDLTypedObj);

// Use the name as a key to find the arc.
VHDLArc* VHDLArcList :: arcWithName(const char* name) {
  return (VHDLArc*) myTable.lookup(name);
}

// Return a pointer to a new copy of the list.
VHDLArcList* VHDLArcList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLArcList* newArcList = new VHDLArcList;
  VHDLArcListIter arcNext(*this);
  VHDLArc* nextArc;
  // Iterate through this list, and create new copies of each entry.
  while ((nextArc = arcNext++) != 0) {
    VHDLArc* newArc = nextArc->newCopy();
    newArcList->put(*newArc);
  }

  return newArcList;
}
