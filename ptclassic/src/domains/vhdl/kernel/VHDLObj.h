#ifndef _VHDLObj_h
#define _VHDLObj_h 1
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

 Methods defining VHDL objects.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "NamedObj.h"
#include "HashTable.h"

class VHDLObj : public NamedObj
{
 public:
  // Constructors.
  VHDLObj();
  VHDLObj(const char* n) { setName(n); }

  // Destructor.
  ~VHDLObj();

  // Name.
  const char* name;

  // Initializer.
  /* virtual */ void initialize();

  // Set the name.
  void setName(const char*);
  // Get the name.
  const char* getName() { return name; }

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLObj.
  VHDLObj* newCopy();

 protected:
 private:
};

class VHDLObjList : public NamedObjList
{
  friend class VHDLObjListIter;

public:
  // Constructor.
  VHDLObjList();

  // Destructor.
  ~VHDLObjList();

  // Initializer.
  void initialize();

  // Add VHDLObj to list.
  void put(VHDLObj&);

  // Add a VHDLObjList to this one, one VHDLObj at a time.
  void addList(VHDLObjList&);

  // Return first VHDLObj on list (const, non-const forms).
  VHDLObj* head() { return (VHDLObj*) NamedObjList::head(); }
  const VHDLObj* head() const {
    return (const VHDLObj*) NamedObjList::head();
  }

  // Remove a VHDLObj from the list.
  // Note:  the VHDLObj is not deleted.
  int remove (VHDLObj* v) { return NamedObjList::remove(v); }

  // Find VHDLObj with given name (const, non-const forms).
  VHDLObj* vhdlObjWithName(const char* name) {
    // NOTE: The following doesn't work because I don't use
    // the same name() facility in VHDLObj's.  I re-implement this.
    //    return (VHDLObj*) objWithName(name);
    NamedObjListIter nextObj(*this);
    VHDLObj* vObj;
    while ((vObj = (VHDLObj*) nextObj++) != 0) {
      // NOTE: I use vObj->name and not vObj->name()
      if (!strcmp(name,(vObj->name))) {
	return vObj;
      }
    }
    return NULL;
  }

  const VHDLObj* vhdlObjWithName(const char* name) const {
    return (const VHDLObj*) objWithName(name);
  }

  // Check to see if an item with the given name is in the list.
  int inList(VHDLObj*);

  // StringList argument version.
  int inList(StringList);

 protected:
  HashTable myTable;

 private:

};

// An iterator class for VHDLObjList.
class VHDLObjListIter : public NamedObjListIter {
 public:
  VHDLObjListIter(VHDLObjList& l) : NamedObjListIter(l) {}
  VHDLObj* next() { return (VHDLObj*) NamedObjListIter::next(); }
  VHDLObj* operator++(POSTFIX_OP) { return next(); }
  NamedObjListIter::reset;
};

#endif
