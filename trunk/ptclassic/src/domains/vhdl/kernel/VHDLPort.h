#ifndef _VHDLPort_h
#define _VHDLPort_h 1
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

 Methods defining VHDL ports.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLTypedObj.h"
#include "VHDLSignal.h"
#include "VHDLVariable.h"
#include "VHDLFiring.h"

class VHDLSignal;
class VHDLFiring;

class VHDLPort : public VHDLTypedObj
{
 public:
  // Constructors.
  VHDLPort();
  VHDLPort(const char* n, const char* t, const char* d, const char* m, VHDLSignal* s, VHDLVariable* v = NULL,
	   VHDLFiring* f = NULL) : VHDLTypedObj(n,t), direction(d), mapping(m), signal(s), variable(v), firing(f) {}

  // Destructor.
  ~VHDLPort();

  // Data direction.
  StringList direction;
  // Map binding.
  StringList mapping;
  // Signal connection.
  VHDLSignal* signal;
  // Internal variable.
  VHDLVariable* variable;
  // Parent firing.
  VHDLFiring* firing;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLPort.
  VHDLPort* newCopy();

  void setDirec(const char* newDirec) { direction = newDirec; }
  StringList getDirec() { return direction; }
  void setVar(VHDLVariable* newVar) { variable = newVar; }
  VHDLVariable* getVar() { return variable; }
  void setFire(VHDLFiring* newFire) { firing = newFire; }
  VHDLFiring* getFire() { return firing; }

  void connect(VHDLSignal*);

 protected:
 private:
};

class VHDLPortList : public VHDLTypedObjList
{
  friend class VHDLPortListIter;

 public:
  // Add VHDLPort to list.
  void put(VHDLPort& v) { VHDLTypedObjList::put(v); }

  // Return first VHDLPort on list (const, non-const forms).
  VHDLPort* head() { return (VHDLPort*) VHDLTypedObjList::head(); }
  const VHDLPort* head() const {
    return (const VHDLPort*) VHDLTypedObjList::head();
  }

  // Remove a VHDLPort from the list.
  // Note:  the VHDLPort is not deleted.
  int remove (VHDLPort* v) { return VHDLTypedObjList::remove(v); }

  // Find VHDLPort with given name (const, non-const forms).
  VHDLPort* vhdlPortWithName(const char* name) {
    return (VHDLPort*) vhdlTypedObjWithName(name);
  }

  const VHDLPort* vhdlPortWithName(const char* name) const {
    return (const VHDLPort*) vhdlTypedObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLPortList* newCopy();

  // Allocate memory for a new VHDLPort and put it in the list.
  void put(StringList, StringList, StringList, StringList="",
	   VHDLSignal* =NULL, VHDLVariable* =NULL, VHDLFiring* =NULL);
};

class VHDLPortListIter : public VHDLTypedObjListIter {
 public:
  VHDLPortListIter(VHDLPortList& l) : VHDLTypedObjListIter(l) {}
  VHDLPort* next() { return (VHDLPort*) VHDLTypedObjListIter::next(); }
  VHDLPort* operator++(POSTFIX_OP) { return next(); }
  VHDLTypedObjListIter::reset;
};

#endif
