#ifndef _VHDLPort_h
#define _VHDLPort_h 1
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

 Methods defining VHDL ports.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLObjList.h"

class VHDLPort : public VHDLObj
{
 public:
  // Constructors.
  VHDLPort();
  VHDLPort(const char*, Block*, const char*);

  // Destructor.
  ~VHDLPort();

  // Name.
//  StringList name;
  // Data type.
  StringList type;
  // Data direction.
  StringList direction;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLPort.
  VHDLPort* newCopy();

 protected:
 private:
};

class VHDLPortList : public VHDLObjList
{
  friend class VHDLPortListIter;

 public:
  // Add VHDLPort to list.
  void put(VHDLPort& v) { VHDLObjList::put(v); }

  // Return first VHDLPort on list (const, non-const forms).
  VHDLPort* head() { return (VHDLPort*) VHDLObjList::head(); }
  const VHDLPort* head() const {
    return (const VHDLPort*) VHDLObjList::head();
  }

  // Remove a VHDLPort from the list.
  // Note:  the VHDLPort is not deleted.
  int remove (VHDLPort* v) { return VHDLObjList::remove(v); }

  // Find VHDLPort with given name (const, non-const forms).
  VHDLPort* vhdlPortWithName(const char* name) {
    return (VHDLPort*) vhdlObjWithName(name);
  }

  const VHDLPort* vhdlPortWithName(const char* name) const {
    return (const VHDLPort*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLPortList* newCopy();

};

class VHDLPortListIter : public VHDLObjListIter {
 public:
  VHDLPortListIter(VHDLPortList& l) : VHDLObjListIter(l) {}
  VHDLPort* next() { return (VHDLPort*) VHDLObjListIter::next(); }
  VHDLPort* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
