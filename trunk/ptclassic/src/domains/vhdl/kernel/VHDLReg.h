#ifndef _VHDLReg_h
#define _VHDLReg_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1996-%Q% The Regents of the University of California.
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

 Methods defining VHDL regs.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLFiring.h"
#include "VHDLGeneric.h"
#include "VHDLPort.h"
#include "VHDLVariable.h"
#include "VHDLSignal.h"

class VHDLPort;
class VHDLPortList;
class VHDLSignalList;

class VHDLReg : public VHDLFiring
{
 public:
  // Constructors.
  VHDLReg();

  // Destructor.
  ~VHDLReg();

  // Input signal.
  VHDLSignal* input;
  // Output signal.
  VHDLSignal* output;
  // Clock signal.
  VHDLSignal* clock;
  // Data type.
  StringList type;

  void setInput(VHDLSignal* newInput)
    { input = newInput; }
  VHDLSignal* getInput() { return input; }
  void setOutput(VHDLSignal* newOutput)
    { output = newOutput; }
  VHDLSignal* getOutput() { return output; }
  void setClock(VHDLSignal* newClock)
    { clock = newClock; }
  VHDLSignal* getClock() { return clock; }
  void setType(const char* newType) { type = newType; }
  StringList getType() { return type; }

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  VHDLReg* newCopy();

 protected:
 private:
};

class VHDLRegList : public VHDLObjList
{
  friend class VHDLRegListIter;

 public:
  // Add VHDLReg to list.
  void put(VHDLReg& v) { VHDLObjList::put(v); }

  // Return first VHDLReg on list (const, non-const forms).
  VHDLReg* head() { return (VHDLReg*) VHDLObjList::head(); }
  const VHDLReg* head() const {
    return (const VHDLReg*) VHDLObjList::head();
  }

  // Remove a VHDLReg from the list.
  // Note:  the VHDLReg is not deleted.
  int remove (VHDLReg* v) { return VHDLObjList::remove(v); }

  // Find VHDLReg with given name (const, non-const forms).
  VHDLReg* vhdlRegWithName(const char* name) {
    return (VHDLReg*) vhdlObjWithName(name);
  }

  const VHDLReg* vhdlRegWithName(const char* name) const {
    return (const VHDLReg*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLRegList* newCopy();

};

class VHDLRegListIter : public VHDLObjListIter {
 public:
  VHDLRegListIter(VHDLRegList& l) : VHDLObjListIter(l) {}
  VHDLReg* next() { return (VHDLReg*) VHDLObjListIter::next(); }
  VHDLReg* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
