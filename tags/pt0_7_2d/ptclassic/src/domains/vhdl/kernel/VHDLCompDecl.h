#ifndef _VHDLCompDecl_h
#define _VHDLCompDecl_h 1
/******************************************************************
Version identification:
@(#)VHDLCompDecl.h	1.6 07/31/96

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Methods defining VHDL CompDecls.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLPort.h"
#include "VHDLGeneric.h"

class VHDLCompDecl : public VHDLObj
{
 public:
  // Constructors.
  VHDLCompDecl();
  VHDLCompDecl(const char* n, VHDLPortList* pl, VHDLGenericList* gl,
	       const char* t, VHDLPortList* pml, VHDLGenericList* gml)
    : VHDLObj(n), portList(pl), genList(gl), type(t), portMapList(pml),
    genMapList(gml) {}

  // Destructor.
  ~VHDLCompDecl();

  // Port list.
  VHDLPortList* portList;
  // Generic list.
  VHDLGenericList* genList;
  // Type.
  StringList type;
  // Port map list.
  VHDLPortList* portMapList;
  // Generic map list.
  VHDLGenericList* genMapList;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  VHDLCompDecl* newCopy();

 protected:
 private:
};

class VHDLCompDeclList : public VHDLObjList
{
  friend class VHDLCompDeclListIter;

 public:
  // Add VHDLCompDecl to list.
  void put(VHDLCompDecl& v) { VHDLObjList::put(v); }

  // Return first VHDLCompDecl on list (const, non-const forms).
  VHDLCompDecl* head() { return (VHDLCompDecl*) VHDLObjList::head(); }
  const VHDLCompDecl* head() const {
    return (const VHDLCompDecl*) VHDLObjList::head();
  }

  // Remove a VHDLCompDecl from the list.
  // Note:  the VHDLCompDecl is not deleted.
  int remove (VHDLCompDecl* v) { return VHDLObjList::remove(v); }

  // Find VHDLCompDecl with given name (const, non-const forms).
  VHDLCompDecl* vhdlCompDeclWithName(const char* name) {
    return (VHDLCompDecl*) vhdlObjWithName(name);
  }

  const VHDLCompDecl* vhdlCompDeclWithName(const char* name) const {
    return (const VHDLCompDecl*) vhdlObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLCompDeclList* newCopy();

  // Allocate memory for a new VHDLCompDecl and put it in the list.
  void put(StringList, VHDLPortList*, VHDLGenericList*,
	   StringList, VHDLPortList*, VHDLGenericList*);
};

class VHDLCompDeclListIter : public VHDLObjListIter {
 public:
  VHDLCompDeclListIter(VHDLCompDeclList& l) : VHDLObjListIter(l) {}
  VHDLCompDecl* next() { return (VHDLCompDecl*) VHDLObjListIter::next(); }
  VHDLCompDecl* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
