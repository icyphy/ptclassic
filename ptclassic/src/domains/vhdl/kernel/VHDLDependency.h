#ifndef _VHDLDependency_h
#define _VHDLDependency_h 1
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

 Methods defining VHDL dependencies.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"

class VHDLDependency : public VHDLObj
{
 public:
  // Constructors.
  VHDLDependency();

  // Destructor.
  ~VHDLDependency();

  // Source of dependency.
  VHDLObj* source;
  // Sink of dependency.
  VHDLObj* sink;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  VHDLDependency* newCopy();

 protected:
 private:
};

class VHDLDependencyList : public VHDLObjList
{
  friend class VHDLDependencyListIter;

 public:
  // Add VHDLDependency to list.
  void put(VHDLDependency& v) { VHDLObjList::put(v); }

  // Return first VHDLDependency on list (const, non-const forms).
  VHDLDependency* head() { return (VHDLDependency*) VHDLObjList::head(); }
  const VHDLDependency* head() const {
    return (const VHDLDependency*) VHDLObjList::head();
  }

  // Remove a VHDLDependency from the list.
  // Note:  the VHDLDependency is not deleted.
  int remove (VHDLDependency* v) { return VHDLObjList::remove(v); }

  // Find VHDLDependency with given name (const, non-const forms).
  VHDLDependency* vhdlDependencyWithName(const char* name) {
    return (VHDLDependency*) vhdlObjWithName(name);
  }

  const VHDLDependency* vhdlDependencyWithName(const char* name) const {
    return (const VHDLDependency*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLDependencyList* newCopy();

};

class VHDLDependencyListIter : public VHDLObjListIter {
 public:
  VHDLDependencyListIter(VHDLDependencyList& l) : VHDLObjListIter(l) {}
  VHDLDependency* next() { return (VHDLDependency*) VHDLObjListIter::next(); }
  VHDLDependency* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
