#ifndef _VHDLPortVar_h
#define _VHDLPortVar_h 1
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

 Methods defining VHDL PortVars.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLObjList.h"

class VHDLPortVar : public VHDLObj
{
 public:
  // Constructors.
  VHDLPortVar();
  VHDLPortVar(const char*, Block*, const char*);

  // Destructor.
  ~VHDLPortVar();

  // Note - "name" corresponds to the port name.
  // Name.
//  StringList name;
  // Map binding.
  StringList variable;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  VHDLPortVar* newCopy();

 protected:
 private:
};

class VHDLPortVarList : public VHDLObjList
{
  friend class VHDLPortVarListIter;

 public:
  // Add VHDLPortVar to list.
  void put(VHDLPortVar& v) { VHDLObjList::put(v); }

  // Return first VHDLPortVar on list (const, non-const forms).
  VHDLPortVar* head() { return (VHDLPortVar*) VHDLObjList::head(); }
  const VHDLPortVar* head() const {
    return (const VHDLPortVar*) VHDLObjList::head();
  }

  // Remove a VHDLPortVar from the list.
  // Note:  the VHDLPortVar is not deleted.
  int remove (VHDLPortVar* v) { return VHDLObjList::remove(v); }

  // Find VHDLPortVar with given name (const, non-const forms).
  VHDLPortVar* vhdlPortVarWithName(const char* name) {
    return (VHDLPortVar*) vhdlObjWithName(name);
  }

  const VHDLPortVar* vhdlPortVarWithName(const char* name) const {
    return (const VHDLPortVar*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLPortVarList* newCopy();

  // Put a new item in the list.
  void put(StringList, StringList);
};

class VHDLPortVarListIter : public VHDLObjListIter {
 public:
  VHDLPortVarListIter(VHDLPortVarList& l) : VHDLObjListIter(l) {}
  VHDLPortVar* next() { return (VHDLPortVar*) VHDLObjListIter::next(); }
  VHDLPortVar* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
