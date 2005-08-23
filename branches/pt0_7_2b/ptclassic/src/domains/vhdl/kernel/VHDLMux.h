#ifndef _VHDLMux_h
#define _VHDLMux_h 1
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

 Methods defining VHDL muxs.

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

class VHDLMux : public VHDLFiring
{
 public:
  // Constructors.
  VHDLMux();

  // Destructor.
  ~VHDLMux();

  // Input signal list.
  VHDLSignalList* inputList;
  // Output signal.
  VHDLSignal* output;
  // Control signal.
  VHDLSignal* control;
  // Data type.
  StringList type;

  void addInput(VHDLSignal* nextInput)
    { inputList->put(*nextInput); }
  VHDLSignalList* getInputs() { return inputList; }
  void setOutput(VHDLSignal* newOutput)
    { output = newOutput; }
  VHDLSignal* getOutput() { return output; }
  void setControl(VHDLSignal* newControl)
    { control = newControl; }
  VHDLSignal* getControl() { return control; }
  void setType(const char* newType) { type = newType; }
  StringList getType() { return type; }

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  VHDLMux* newCopy();

 protected:
 private:
};

class VHDLMuxList : public VHDLObjList
{
  friend class VHDLMuxListIter;

 public:
  // Add VHDLMux to list.
  void put(VHDLMux& v) { VHDLObjList::put(v); }

  // Return first VHDLMux on list (const, non-const forms).
  VHDLMux* head() { return (VHDLMux*) VHDLObjList::head(); }
  const VHDLMux* head() const {
    return (const VHDLMux*) VHDLObjList::head();
  }

  // Remove a VHDLMux from the list.
  // Note:  the VHDLMux is not deleted.
  int remove (VHDLMux* v) { return VHDLObjList::remove(v); }

  // Find VHDLMux with given name (const, non-const forms).
  VHDLMux* vhdlMuxWithName(const char* name) {
    return (VHDLMux*) vhdlObjWithName(name);
  }

  const VHDLMux* vhdlMuxWithName(const char* name) const {
    return (const VHDLMux*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLMuxList* newCopy();

};

class VHDLMuxListIter : public VHDLObjListIter {
 public:
  VHDLMuxListIter(VHDLMuxList& l) : VHDLObjListIter(l) {}
  VHDLMux* next() { return (VHDLMux*) VHDLObjListIter::next(); }
  VHDLMux* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
