#ifndef _VHDLToken_h
#define _VHDLToken_h 1
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

 Methods defining VHDL tokens.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLTypedObj.h"

class VHDLArc;
class VHDLFiring;
class VHDLFiringList;

class VHDLToken : public VHDLTypedObj
{
 public:
  // Constructors.
  VHDLToken();
  VHDLToken(const char* n, const char* t, VHDLArc* a, int tn,
	    VHDLFiring* sf, VHDLFiringList* dfs)
    : VHDLTypedObj(n,t), arc(a), tokenNumber(tn), sourceFiring(sf),
    destFirings(dfs) {}

  // Destructor.
  ~VHDLToken();

  // VHDLArc associated with.
  VHDLArc* arc;
  // Token number on arc.
  int tokenNumber;
  // Source firing.
  VHDLFiring* sourceFiring;
  // Dest firings.
  VHDLFiringList* destFirings;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  // Return a pointer to a new copy of the VHDLToken.
  VHDLToken* newCopy();

 protected:
 private:
};

class VHDLTokenList : public VHDLTypedObjList
{
  friend class VHDLTokenListIter;

 public:
  // Add VHDLToken to list.
  void put(VHDLToken& v) { VHDLTypedObjList::put(v); }

  // Use the name as a key to find the token.
  VHDLToken* tokenWithName(const char*);

  const VHDLToken* vhdlTokenWithName(const char* name) const {
    return (const VHDLToken*) vhdlTypedObjWithName(name);
  }

  // Return first VHDLToken on list (const, non-const forms).
  VHDLToken* head() { return (VHDLToken*) VHDLTypedObjList::head(); }
  const VHDLToken* head() const {
    return (const VHDLToken*) VHDLTypedObjList::head();
  }

  // Remove a VHDLToken from the list.
  // Note:  the VHDLToken is not deleted.
  int remove (VHDLToken* v) { return VHDLTypedObjList::remove(v); }

  // Find VHDLToken with given name (const, non-const forms).
  VHDLToken* vhdlTokenWithName(const char* name) {
    return (VHDLToken*) vhdlTypedObjWithName(name);
  }

  // Return a pointer to a new copy of the list.
  VHDLTokenList* newCopy();

};

class VHDLTokenListIter : public VHDLTypedObjListIter {
 public:
  VHDLTokenListIter(VHDLTokenList& l) : VHDLTypedObjListIter(l) {}
  VHDLToken* next() { return (VHDLToken*) VHDLTypedObjListIter::next(); }
  VHDLToken* operator++(POSTFIX_OP) { return next(); }
  VHDLTypedObjListIter::reset;
};

#endif
