static const char file_id[] = "VHDLPort.cc";
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

 Programmer: Edward A. Lee, Michael C. Williamson

 Methods for VHDL ports.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLPort.h"

// Constructors.
VHDLPort :: VHDLPort() {
  VHDLObj::initialize();
}

VHDLPort :: VHDLPort(const char* n, Block* p, const char* d) : VHDLObj(n,p,d) {
  VHDLObj::initialize();
}

// Destructor.
VHDLPort :: ~VHDLPort() {}

// Return a pointer to a new copy of the VHDLPort.
VHDLPort* VHDLPort :: newCopy() {
  VHDLPort* newPort = new VHDLPort;
  newPort->name = this->name;
  newPort->direction = this->direction;
  newPort->type = this->type;

  return newPort;
}

// Class identification.
const char* VHDLPort :: className() const { return "VHDLPort"; }

ISA_FUNC(VHDLPort,VHDLObj);

// Return a pointer to a new copy of the list.
VHDLPortList* VHDLPortList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLPortList* newPortList = new VHDLPortList;
  VHDLPortListIter portNext(*this);
  VHDLPort* nextPort;
  // Iterate through this list, and create new copies of each entry.
  while ((nextPort = portNext++) != 0) {
    VHDLPort* newPort = nextPort->newCopy();
    newPortList->put(*newPort);
  }

  return newPortList;
}

// Allocate memory for a new VHDLPort and put it in the list.
void VHDLPortList :: put(StringList name, StringList direction,
			 StringList type) {
  if (this->inList(name)) return;
  VHDLPort* newPort = new VHDLPort;
  newPort->name = name;
  newPort->direction = direction;
  newPort->type = type;
  this->put(*newPort);
}
