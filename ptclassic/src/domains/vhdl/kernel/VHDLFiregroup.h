#ifndef _VHDLFiregroup_h
#define _VHDLFiregroup_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Methods defining VHDL firegroups.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLFiring.h"

class VHDLFiregroup : public VHDLObj
{
 public:
  // Constructors.
  VHDLFiregroup();

  // Destructor.
  ~VHDLFiregroup();

  VHDLFiringList* firingList;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  VHDLFiregroup* newCopy();

 protected:
 private:
};

class VHDLFiregroupList : public VHDLObjList
{
  friend class VHDLFiregroupListIter;

 public:
  // Add VHDLFiregroup to list.
  void put(VHDLFiregroup& v) { VHDLObjList::put(v); }

  // Return first VHDLFiregroup on list (const, non-const forms).
  VHDLFiregroup* head() { return (VHDLFiregroup*) VHDLObjList::head(); }
  const VHDLFiregroup* head() const {
    return (const VHDLFiregroup*) VHDLObjList::head();
  }

  // Remove a VHDLFiregroup from the list.
  // Note:  the VHDLFiregroup is not deleted.
  int remove (VHDLFiregroup* v) { return VHDLObjList::remove(v); }

  // Find VHDLFiregroup with given name (const, non-const forms).
  VHDLFiregroup* vhdlFiregroupWithName(const char* name) {
    return (VHDLFiregroup*) vhdlObjWithName(name);
  }

  const VHDLFiregroup* vhdlFiregroupWithName(const char* name) const {
    return (const VHDLFiregroup*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLFiregroupList* newCopy();

};

class VHDLFiregroupListIter : public VHDLObjListIter {
 public:
  VHDLFiregroupListIter(VHDLFiregroupList& l) : VHDLObjListIter(l) {}
  VHDLFiregroup* next() { return (VHDLFiregroup*) VHDLObjListIter::next(); }
  VHDLFiregroup* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
