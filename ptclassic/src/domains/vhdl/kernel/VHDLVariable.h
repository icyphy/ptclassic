#ifndef _VHDLVariable_h
#define _VHDLVariable_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: Michael C. Williamson

 Methods defining VHDL variables.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLTypedObj.h"

class VHDLVariable : public VHDLTypedObj
{
 public:
  // Constructors.
  VHDLVariable();
  VHDLVariable(const char* n, const char* t)
    : VHDLTypedObj(n,t), initVal("") {}
  VHDLVariable(const char* n, const char* t, const char* iv)
    : VHDLTypedObj(n,t), initVal(iv) {}

  // Destructor.
  ~VHDLVariable();

  // Initial value.
  StringList initVal;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLVariable.
  VHDLVariable* newCopy();

 protected:
 private:
};

class VHDLVariableList : public VHDLTypedObjList
{
  friend class VHDLVariableListIter;

 public:
  // Add VHDLVariable to list.
  void put(VHDLVariable& v) { VHDLTypedObjList::put(v); }

  // Return first VHDLVariable on list (const, non-const forms).
  VHDLVariable* head() { return (VHDLVariable*) VHDLTypedObjList::head(); }
  const VHDLVariable* head() const {
    return (const VHDLVariable*) VHDLTypedObjList::head();
  }

  // Remove a VHDLVariable from the list.
  // Note:  the VHDLVariable is not deleted.
  int remove (VHDLVariable* v) { return VHDLTypedObjList::remove(v); }

  // Find VHDLVariable with given name (const, non-const forms).
  VHDLVariable* vhdlVariableWithName(const char* name) {
    return (VHDLVariable*) vhdlTypedObjWithName(name);
  }

  const VHDLVariable* vhdlVariableWithName(const char* name) const {
    return (const VHDLVariable*) vhdlTypedObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLVariableList* newCopy();

  // Allocate memory for a new VHDLVariable and put it in the list.
  void put(StringList, StringList, StringList);
};

class VHDLVariableListIter : public VHDLTypedObjListIter {
 public:
  VHDLVariableListIter(VHDLVariableList& l) : VHDLTypedObjListIter(l) {}
  VHDLVariable* next() { return (VHDLVariable*) VHDLTypedObjListIter::next(); }
  VHDLVariable* operator++(POSTFIX_OP) { return next(); }
  VHDLTypedObjListIter::reset;
};

#endif
