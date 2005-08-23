#ifndef _VHDLGeneric_h
#define _VHDLGeneric_h 1
/******************************************************************
Version identification:
@(#)VHDLGeneric.h	1.8 07/31/96

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

 Programmer: Michael C. Williamson

 Methods defining VHDL generics.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLTypedObj.h"

class VHDLGeneric : public VHDLTypedObj
{
 public:
  // Constructors.
  VHDLGeneric();
  VHDLGeneric(const char* n, const char* t, const char* d,
	      const char* m)
    : VHDLTypedObj(n,t), defaultVal(d), mapping(m) {}

  // Destructor.
  ~VHDLGeneric();

  // Default value.
  StringList defaultVal;
  // Map binding.
  StringList mapping;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLGeneric.
  VHDLGeneric* newCopy();

  void setDefaultVal(const char* newDefaultVal) { defaultVal = newDefaultVal; }
  void setMapping(const char* newMapping) { mapping = newMapping; }

 protected:
 private:
};

class VHDLGenericList : public VHDLTypedObjList
{
  friend class VHDLGenericListIter;

 public:
  // Add VHDLGeneric to list.
  void put(VHDLGeneric& v) { VHDLTypedObjList::put(v); }

  // Return first VHDLGeneric on list (const, non-const forms).
  VHDLGeneric* head() { return (VHDLGeneric*) VHDLTypedObjList::head(); }
  const VHDLGeneric* head() const {
    return (const VHDLGeneric*) VHDLTypedObjList::head();
  }

  // Remove a VHDLGeneric from the list.
  // Note:  the VHDLGeneric is not deleted.
  int remove (VHDLGeneric* v) { return VHDLTypedObjList::remove(v); }

  // Find VHDLGeneric with given name (const, non-const forms).
  VHDLGeneric* vhdlGenericWithName(const char* name) {
    return (VHDLGeneric*) vhdlTypedObjWithName(name);
  }

  const VHDLGeneric* vhdlGenericWithName(const char* name) const {
    return (const VHDLGeneric*) vhdlTypedObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLGenericList* newCopy();

  // Allocate memory for a new VHDLGeneric and put it in the list.
  void put(StringList, StringList, StringList="", StringList="");
};

class VHDLGenericListIter : public VHDLTypedObjListIter {
 public:
  VHDLGenericListIter(VHDLGenericList& l) : VHDLTypedObjListIter(l) {}
  VHDLGeneric* next() { return (VHDLGeneric*) VHDLTypedObjListIter::next(); }
  VHDLGeneric* operator++(POSTFIX_OP) { return next(); }
  VHDLTypedObjListIter::reset;
};

#endif
