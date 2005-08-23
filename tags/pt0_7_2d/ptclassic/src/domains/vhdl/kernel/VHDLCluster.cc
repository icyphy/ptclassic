static const char file_id[] = "VHDLCluster.cc";
/******************************************************************
Version identification:
@(#)VHDLCluster.cc	1.7 07/31/96

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

 Programmer: Edward A. Lee, Michael C. Williamson

 Methods for VHDL CompDecls.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLCluster.h"

// Constructors.
VHDLCluster :: VHDLCluster() {
  VHDLObj::initialize();
}

// Destructor.
VHDLCluster :: ~VHDLCluster() {}

// Return a pointer to a new copy of the VHDLCluster.
VHDLCluster* VHDLCluster :: newCopy() {
  VHDLCluster* newCluster = new VHDLCluster;
  newCluster->setName(this->name);
  newCluster->firingList = this->firingList->newCopy();

  return newCluster;
}

// Return a generic list built from all the firings in the cluster.
VHDLGenericList* VHDLCluster :: genericList() {
  VHDLGenericList* clusterGenericList = new VHDLGenericList;

  VHDLFiringListIter nextFiring(*(this->firingList));
  VHDLFiring* nFiring;
  while ((nFiring = nextFiring++) != 0) {
    VHDLGenericListIter nextGeneric(*(nFiring->genericList));
    VHDLGeneric* nGeneric;
    while ((nGeneric = nextGeneric++) != 0) {
      clusterGenericList->put(*nGeneric);
    }
  }

  return clusterGenericList;
}

// Return a port list built from all the firings in the cluster.
VHDLPortList* VHDLCluster :: portList() {
  VHDLPortList* clusterPortList = new VHDLPortList;

  VHDLFiringListIter nextFiring(*(this->firingList));
  VHDLFiring* nFiring;
  while ((nFiring = nextFiring++) != 0) {
    VHDLPortListIter nextPort(*(nFiring->portList));
    VHDLPort* nPort;
    while ((nPort = nextPort++) != 0) {
      clusterPortList->put(*nPort);
    }
  }

  return clusterPortList;
}

// Class identification.
const char* VHDLCluster :: className() const { return "VHDLCluster"; }

ISA_FUNC(VHDLCluster,VHDLObj);
