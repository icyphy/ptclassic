static const char file_id[] = "VHDLState.cc";
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

 Methods for VHDL states.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLState.h"

// Constructors.
VHDLState :: VHDLState() {
//  VHDLObj::initialize();
//  State::initialize();
}

/*
  VHDLState :: VHDLState(const char* n, Block* p, const char* d) : State(n,p,d) {
//  VHDLObj::initialize();
  State::initialize();
}
*/

// Destructor.
VHDLState :: ~VHDLState() {}

/*
// Return a pointer to a new copy of the VHDLState.
VHDLState* VHDLState :: newCopy() {
  VHDLState* newState = new VHDLState;
  newState->name = this->name;
  newState->lastFiring = this->lastFiring;
  newState->type = this->type;

  return newState;
}
*/

// Class identification.
const char* VHDLState :: className() const { return "VHDLState"; }

ISA_FUNC(VHDLState,VHDLObj);
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
