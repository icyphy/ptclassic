#ifndef _VHDLFiring_h
#define _VHDLFiring_h 1
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

 Methods defining VHDL firings.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLGeneric.h"
#include "VHDLPort.h"
#include "VHDLVariable.h"
#include "VHDLSignal.h"

class VHDLPort;
class VHDLPortList;
class VHDLSignalList;

class VHDLFiring : public VHDLObj
{
 public:
  // Constructors.
  VHDLFiring();

  // Destructor.
  ~VHDLFiring();

  // Star class name.
  StringList starClassName;
  // Generic list.
  VHDLGenericList* genericList;
  // Port list.
  VHDLPortList* portList;
  // Variable list.
  VHDLVariableList* variableList;
  // Action.
  StringList action;
  // Generic map list.
  VHDLGenericList* genericMapList;
  // Port map list.
  VHDLPortList* portMapList;
  // Signal list.
  VHDLSignalList* signalList;
  // Extra variable declarations.
  StringList decls;
  // Suppress sensitivity lists.
  int noSensitivities;
  // Suppress output clocking.
  int noOutclocking;
  // Firegroup serial number
  int groupNum;
  // Starting time.
  int startTime;
  // Ending time.
  int endTime;
  // Latency (note: should be endTime - startTime).
  int latency;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  VHDLFiring* newCopy();

 protected:
 private:
};

class VHDLFiringList : public VHDLObjList
{
  friend class VHDLFiringListIter;

 public:
  // Add VHDLFiring to list.
  void put(VHDLFiring& v) { VHDLObjList::put(v); }

  // Return first VHDLFiring on list (const, non-const forms).
  VHDLFiring* head() { return (VHDLFiring*) VHDLObjList::head(); }
  const VHDLFiring* head() const {
    return (const VHDLFiring*) VHDLObjList::head();
  }

  // Remove a VHDLFiring from the list.
  // Note:  the VHDLFiring is not deleted.
  int remove (VHDLFiring* v) { return VHDLObjList::remove(v); }

  // Find VHDLFiring with given name (const, non-const forms).
  VHDLFiring* vhdlFiringWithName(const char* name) {
    return (VHDLFiring*) vhdlObjWithName(name);
  }

  const VHDLFiring* vhdlFiringWithName(const char* name) const {
    return (const VHDLFiring*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLFiringList* newCopy();

};

class VHDLFiringListIter : public VHDLObjListIter {
 public:
  VHDLFiringListIter(VHDLFiringList& l) : VHDLObjListIter(l) {}
  VHDLFiring* next() { return (VHDLFiring*) VHDLObjListIter::next(); }
  VHDLFiring* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
