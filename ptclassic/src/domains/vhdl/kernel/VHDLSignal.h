#ifndef _VHDLSignal_h
#define _VHDLSignal_h 1
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

 Methods defining VHDL signals.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLTypedObj.h"
#include "VHDLPort.h"

class VHDLPort;

class VHDLSignal : public VHDLTypedObj
{
 public:
  // Constructors.
  VHDLSignal();
  VHDLSignal(const char* n, const char* t, VHDLPort* s)
    : VHDLTypedObj(n,t), source (s) {}

  // Destructor.
  ~VHDLSignal();

  // Pointer to source VHDLPort.
  VHDLPort* source;
  // For muxs: integer control value associated
  // with this signal being selected.
  int controlValue;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLSignal.
  VHDLSignal* newCopy();

  void setSource(VHDLPort* newSource) { source = newSource; }
  VHDLPort* getSource() { return source; }
  void disconnect() { source = NULL; }
  void setControlValue(int newControlValue)
    { controlValue = newControlValue; }
  int getControlValue() { return controlValue; }

 protected:
 private:
};

class VHDLSignalList : public VHDLTypedObjList
{
  friend class VHDLSignalListIter;

 public:
  // Add VHDLSignal to list.
  void put(VHDLSignal& v) { VHDLTypedObjList::put(v); }

  // Return first VHDLSignal on list (const, non-const forms).
  VHDLSignal* head() { return (VHDLSignal*) VHDLTypedObjList::head(); }
  const VHDLSignal* head() const {
    return (const VHDLSignal*) VHDLTypedObjList::head();
  }

  // Remove a VHDLSignal from the list.
  // Note:  the VHDLSignal is not deleted.
  int remove (VHDLSignal* v) { return VHDLTypedObjList::remove(v); }

  // Find VHDLSignal with given name (const, non-const forms).
  VHDLSignal* vhdlSignalWithName(const char* name) {
    return (VHDLSignal*) vhdlTypedObjWithName(name);
  }

  const VHDLSignal* vhdlSignalWithName(const char* name) const {
    return (const VHDLSignal*) vhdlTypedObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLSignalList* newCopy();

  // Allocate memory for a new VHDLSignal and put it in the list.
  void put(StringList, StringList, VHDLPort* =NULL);

  // Allocate memory for a new VHDLSignal and put it in the list.
  // Also, return a pointer to the new VHDLSignal.
  VHDLSignal* add(StringList, StringList, VHDLPort* =NULL);
};

class VHDLSignalListIter : public VHDLTypedObjListIter {
 public:
  VHDLSignalListIter(VHDLSignalList& l) : VHDLTypedObjListIter(l) {}
  VHDLSignal* next() { return (VHDLSignal*) VHDLTypedObjListIter::next(); }
  VHDLSignal* operator++(POSTFIX_OP) { return next(); }
  VHDLTypedObjListIter::reset;
};

#endif
