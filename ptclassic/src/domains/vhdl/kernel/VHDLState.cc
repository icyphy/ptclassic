static const char file_id[] = "VHDLState.cc";
/******************************************************************
Version identification:
@(#)VHDLState.cc	1.7 05/31/97

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

 Methods for VHDL states.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLState.h"

// Constructors.
VHDLState :: VHDLState() {
    name = "UNINITIALIZED";
    type = "UNINITIALIZED";
    lastRef = "UNINITIALIZED";
    firstRef = "UNINITIALIZED";
    initVal = "UNINITIALIZED";
    constant = -99999;
    lastFiring = -99999;
    firstFiringName = "UNINITIALIZED";
    lastFiringName = "UNINITIALIZED";
    constRefFiringList.initialize();
    VHDLTypedObj::initialize();
}

// Destructor.
VHDLState :: ~VHDLState() {}

/*
// Return a pointer to a new copy of the VHDLState.
VHDLState* VHDLState :: newCopy() {
  VHDLState* newState = new VHDLState(name, type, lastRef, firstRef, initVal,
                                      constant);
  return newState;
}
*/

// Class identification.
const char* VHDLState :: className() const { return "VHDLState"; }

ISA_FUNC(VHDLState,VHDLTypedObj);
/*
// Return a pointer to a new copy of the list.
VHDLStateList* VHDLStateList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLStateList* newStateList = new VHDLStateList;
  VHDLStateListIter stateNext(*this);
  VHDLState* nextState;
  // Iterate through this list, and create new copies of each entry.
  while ((nextState = stateNext++) != 0) {
    VHDLState* newState = nextState->newCopy();
    newStateList->put(*newState);
  }

  return newStateList;
}
*/
