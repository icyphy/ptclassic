#ifndef _VHDLVariable_h
#define _VHDLVariable_h 1
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

 Methods defining VHDL variables.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLObjList.h"

class VHDLVariable : public VHDLObj
{
 public:
  // Constructors.
  VHDLVariable();
  VHDLVariable(const char*, Block*, const char*);

  // Destructor.
  ~VHDLVariable();

  // Name.
//  StringList name;
  // Data type.
  StringList type;
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

class VHDLVariableList : public VHDLObjList
{
  friend class VHDLVariableListIter;

 public:
  // Add VHDLVariable to list.
  void put(VHDLVariable& v) { VHDLObjList::put(v); }

  // Return first VHDLVariable on list (const, non-const forms).
  VHDLVariable* head() { return (VHDLVariable*) VHDLObjList::head(); }
  const VHDLVariable* head() const {
    return (const VHDLVariable*) VHDLObjList::head();
  }

  // Remove a VHDLVariable from the list.
  // Note:  the VHDLVariable is not deleted.
  int remove (VHDLVariable* v) { return VHDLObjList::remove(v); }

  // Find VHDLVariable with given name (const, non-const forms).
  VHDLVariable* vhdlVariableWithName(const char* name) {
    return (VHDLVariable*) vhdlObjWithName(name);
  }

  const VHDLVariable* vhdlVariableWithName(const char* name) const {
    return (const VHDLVariable*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLVariableList* newCopy();

};

class VHDLVariableListIter : public VHDLObjListIter {
 public:
  VHDLVariableListIter(VHDLVariableList& l) : VHDLObjListIter(l) {}
  VHDLVariable* next() { return (VHDLVariable*) VHDLObjListIter::next(); }
  VHDLVariable* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
