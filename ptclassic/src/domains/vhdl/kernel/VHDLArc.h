#ifndef _VHDLArc_h
#define _VHDLArc_h 1
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

 Methods defining VHDL arcs.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLObjList.h"

class VHDLArc : public VHDLObj
{
 public:
  // Constructors.
  VHDLArc();
  VHDLArc(const char*, Block*, const char*);

  // Destructor.
  ~VHDLArc();

  // Name.
//  StringList name;
  // Data Type.
  StringList type;
  // Low write marker.
  int lowWrite;
  // High write marker.
  int highWrite;
  // Low read marker.
  int lowRead;
  // High read marker.
  int highRead;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLArc.
  VHDLArc* newCopy();

 protected:
 private:
};

class VHDLArcList : public VHDLObjList
{
  friend class VHDLArcListIter;

 public:
  // Add VHDLArc to list.
  void put(VHDLArc& v) { VHDLObjList::put(v); }

  // Return first VHDLArc on list (const, non-const forms).
  VHDLArc* head() { return (VHDLArc*) VHDLObjList::head(); }
  const VHDLArc* head() const {
    return (const VHDLArc*) VHDLObjList::head();
  }

  // Remove a VHDLArc from the list.
  // Note:  the VHDLArc is not deleted.
  int remove (VHDLArc* v) { return VHDLObjList::remove(v); }

  // Find VHDLArc with given name (const, non-const forms).
  VHDLArc* vhdlArcWithName(const char* name) {
    return (VHDLArc*) vhdlObjWithName(name);
  }

  const VHDLArc* vhdlArcWithName(const char* name) const {
    return (const VHDLArc*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLArcList* newCopy();

};

class VHDLArcListIter : public VHDLObjListIter {
 public:
  VHDLArcListIter(VHDLArcList& l) : VHDLObjListIter(l) {}
  VHDLArc* next() { return (VHDLArc*) VHDLObjListIter::next(); }
  VHDLArc* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
