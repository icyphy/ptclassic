#ifndef _VHDLTypedObj_h
#define _VHDLTypedObj_h 1
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

 Methods defining VHDL TypedObjs.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"

class VHDLTypedObj : public VHDLObj
{
 public:
  // Constructors.
  VHDLTypedObj();
  VHDLTypedObj(const char* n, const char* t)
    : VHDLObj(n), type(t) {}

  // Destructor.
  ~VHDLTypedObj();

  // Data type.
  StringList type;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLTypedObj.
  VHDLTypedObj* newCopy();

  void setType(const char* newType) { type = newType; }
  StringList getType() { return type; }

 protected:
 private:
};

class VHDLTypedObjList : public VHDLObjList
{
  friend class VHDLTypedObjListIter;

 public:
  // Add VHDLTypedObj to list.
  void put(VHDLTypedObj& v) { VHDLObjList::put(v); }

  // Return first VHDLTypedObj on list (const, non-const forms).
  VHDLTypedObj* head() { return (VHDLTypedObj*) VHDLObjList::head(); }
  const VHDLTypedObj* head() const {
    return (const VHDLTypedObj*) VHDLObjList::head();
  }

  // Remove a VHDLTypedObj from the list.
  // Note:  the VHDLTypedObj is not deleted.
  int remove (VHDLTypedObj* v) { return VHDLObjList::remove(v); }

  // Find VHDLTypedObj with given name (const, non-const forms).
  VHDLTypedObj* vhdlTypedObjWithName(const char* name) {
    return (VHDLTypedObj*) vhdlObjWithName(name);
  }

  const VHDLTypedObj* vhdlTypedObjWithName(const char* name) const {
    return (const VHDLTypedObj*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLTypedObjList* newCopy();

  // Allocate memory for a new VHDLTypedObj and put it in the list.
  void put(StringList, StringList);
};

class VHDLTypedObjListIter : public VHDLObjListIter {
 public:
  VHDLTypedObjListIter(VHDLTypedObjList& l) : VHDLObjListIter(l) {}
  VHDLTypedObj* next() { return (VHDLTypedObj*) VHDLObjListIter::next(); }
  VHDLTypedObj* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
