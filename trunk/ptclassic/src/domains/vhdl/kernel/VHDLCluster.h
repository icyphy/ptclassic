#ifndef _VHDLCluster_h
#define _VHDLCluster_h 1
/******************************************************************
Version identification:
@(#)VHDLCluster.h	1.5 07/31/96

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

 Methods defining VHDL Clusters.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLObj.h"
#include "VHDLFiring.h"

class VHDLCluster : public VHDLObj
{
 public:
  // Constructors.
  VHDLCluster();

  // Destructor.
  ~VHDLCluster();

  // Firing list.
  VHDLFiringList* firingList;

  // Class Idenitification.
  /* virtual */ int isA(const char*) const;
  /* virtual */ const char* className() const;

  VHDLCluster* newCopy();

  // Return a generic list built from all the firings in the cluster.
  VHDLGenericList* genericList();
  // Return a port list built from all the firings in the cluster.
  VHDLPortList* portList();

 protected:
 private:
};

class VHDLClusterList : public VHDLObjList
{
  friend class VHDLClusterListIter;

 public:
  // Add VHDLCluster to list.
  void put(VHDLCluster& v) { VHDLObjList::put(v); }

  // Return first VHDLCluster on list (const, non-const forms).
  VHDLCluster* head() { return (VHDLCluster*) VHDLObjList::head(); }
  const VHDLCluster* head() const {
    return (const VHDLCluster*) VHDLObjList::head();
  }

  // Remove a VHDLCluster from the list.
  // Note:  the VHDLCluster is not deleted.
  int remove (VHDLCluster* v) { return VHDLObjList::remove(v); }

  // Find VHDLCluster with given name (const, non-const forms).
  VHDLCluster* vhdlClusterWithName(const char* name) {
    return (VHDLCluster*) vhdlObjWithName(name);
  }

  const VHDLCluster* vhdlClusterWithName(const char* name) const {
    return (const VHDLCluster*) vhdlObjWithName(name);
  }

};

class VHDLClusterListIter : public VHDLObjListIter {
 public:
  VHDLClusterListIter(VHDLClusterList& l) : VHDLObjListIter(l) {}
  VHDLCluster* next() { return (VHDLCluster*) VHDLObjListIter::next(); }
  VHDLCluster* operator++(POSTFIX_OP) { return next(); }
  VHDLObjListIter::reset;
};

#endif
