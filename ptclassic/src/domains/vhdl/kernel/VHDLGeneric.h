#ifndef _VHDLGeneric_h
#define _VHDLGeneric_h 1
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

 Methods defining VHDL generics.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLObjList.h"

class VHDLGeneric : public VHDLObj
{
 public:
  // Constructors.
  VHDLGeneric();
  VHDLGeneric(const char*, Block*, const char*);

  // Destructor.
  ~VHDLGeneric();

  // Name.
//  StringList name;
  // Data type.
  StringList type;
  // Default value.
  StringList defaultVal;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLGeneric.
  VHDLGeneric* newCopy();

 protected:
 private:
};

class VHDLGenericList : public VHDLObjList
{
  friend class VHDLGenericListIter;

 public:
  // Add VHDLGeneric to list.
  void put(VHDLGeneric& v) { VHDLObjList::put(v); }

  // Return first VHDLGeneric on list (const, non-const forms).
  VHDLGeneric* head() { return (VHDLGeneric*) VHDLObjList::head(); }
  const VHDLGeneric* head() const {
    return (const VHDLGeneric*) VHDLObjList::head();
  }

  // Remove a VHDLGeneric from the list.
  // Note:  the VHDLGeneric is not deleted.
  int remove (VHDLGeneric* v) { return VHDLObjList::remove(v); }

  // Find VHDLGeneric with given name (const, non-const forms).
  VHDLGeneric* vhdlGenericWithName(const char* name) {
    return (VHDLGeneric*) vhdlObjWithName(name);
  }

  const VHDLGeneric* vhdlGenericWithName(const char* name) const {
    return (const VHDLGeneric*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLGenericList* newCopy();

};

class VHDLGenericListIter : public VHDLObjListIter {
 public:
  VHDLGenericListIter(VHDLGenericList& l) : VHDLObjListIter(l) {}
  VHDLGeneric* next() { return (VHDLGeneric*) VHDLObjListIter::next(); }
  VHDLGeneric* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
