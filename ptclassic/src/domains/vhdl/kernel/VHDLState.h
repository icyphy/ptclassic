#ifndef _VHDLState_h
#define _VHDLState_h 1
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

 Programmer: Michael C. Williamson

 Methods defining VHDL states.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLObjList.h"

class VHDLState : public VHDLObj {
 public:
  // Constructors.
  VHDLState();
//  VHDLState(const char*, Block*, const char*);

  // Destructor.
  ~VHDLState();

  // Last reference to this state.
  StringList lastRef;
  // First reference to this state.
  StringList firstRef;
  // Initial value.
  StringList initVal;
  // Type.
  StringList type;

  // Last firing to access this state.
  int lastFiring;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;
/*
  // Return a pointer to a new copy of the VHDLState.
  VHDLState* newCopy();
*/

 protected:
 private:
};

class VHDLStateList : public VHDLObjList
{
  friend class VHDLStateListIter;

 public:
  // Add VHDLState to list.
  void put(VHDLState& v) { VHDLObjList::put(v); }

  // Return first VHDLState on list (const, non-const forms).
  VHDLState* head() { return (VHDLState*) VHDLObjList::head(); }
  const VHDLState* head() const {
    return (const VHDLState*) VHDLObjList::head();
  }

  // Remove a VHDLState from the list.
  // Note:  the VHDLState is not deleted.
  int remove (VHDLState* v) { return VHDLObjList::remove(v); }

  // Find VHDLState with given name (const, non-const forms).
  VHDLState* vhdlStateWithName(const char* name) {
    return (VHDLState*) vhdlObjWithName(name);
  }

  const VHDLState* vhdlStateWithName(const char* name) const {
    return (const VHDLState*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLStateList* newCopy();

};

class VHDLStateListIter : public VHDLObjListIter {
 public:
  VHDLStateListIter(VHDLStateList& l) : VHDLObjListIter(l) {}
  VHDLState* next() { return (VHDLState*) VHDLObjListIter::next(); }
  VHDLState* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
