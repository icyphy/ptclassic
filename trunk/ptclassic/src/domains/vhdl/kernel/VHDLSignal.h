#ifndef _VHDLSignal_h
#define _VHDLSignal_h 1
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

 Methods defining VHDL signals.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLObjList.h"

class VHDLSignal : public VHDLObj
{
 public:
  // Constructors.
  VHDLSignal();
  VHDLSignal(const char*, Block*, const char*);

  // Destructor.
  ~VHDLSignal();

  // Name.
//  StringList name;
  // Type.
  StringList type;
  // Name of port coming from.
  StringList from;
  // Name of port going to.
  StringList to;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLSignal.
  VHDLSignal* newCopy();

 protected:
 private:
};

class VHDLSignalList : public VHDLObjList
{
  friend class VHDLSignalListIter;

 public:
  // Add VHDLSignal to list.
  void put(VHDLSignal& v) { VHDLObjList::put(v); }

  // Return first VHDLSignal on list (const, non-const forms).
  VHDLSignal* head() { return (VHDLSignal*) VHDLObjList::head(); }
  const VHDLSignal* head() const {
    return (const VHDLSignal*) VHDLObjList::head();
  }

  // Remove a VHDLSignal from the list.
  // Note:  the VHDLSignal is not deleted.
  int remove (VHDLSignal* v) { return VHDLObjList::remove(v); }

  // Find VHDLSignal with given name (const, non-const forms).
  VHDLSignal* vhdlSignalWithName(const char* name) {
    return (VHDLSignal*) vhdlObjWithName(name);
  }

  const VHDLSignal* vhdlSignalWithName(const char* name) const {
    return (const VHDLSignal*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLSignalList* newCopy();

  // Allocate memory for a new VHDLSignal and put it in the list.
  void put(StringList, StringList, StringList, StringList);
};

class VHDLSignalListIter : public VHDLObjListIter {
 public:
  VHDLSignalListIter(VHDLSignalList& l) : VHDLObjListIter(l) {}
  VHDLSignal* next() { return (VHDLSignal*) VHDLObjListIter::next(); }
  VHDLSignal* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
