static const char file_id[] = "VHDLToken.cc";
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

 Programmer: Edward A. Lee, Michael C. Williamson

 Methods for VHDL tokens.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLToken.h"

// Constructors.
VHDLToken :: VHDLToken() {
  VHDLTypedObj::initialize();
}

// Destructor.
VHDLToken :: ~VHDLToken() {}

// Return a pointer to a new copy of the VHDLToken.
VHDLToken* VHDLToken :: newCopy() {
  VHDLToken* newToken = new VHDLToken(name, type, arc, tokenNumber,
				      sourceFiring, destFirings);
  return newToken;
}

// Class identification.
const char* VHDLToken :: className() const { return "VHDLToken"; }

ISA_FUNC(VHDLToken,VHDLTypedObj);

// Use the name as a key to find the token.
VHDLToken* VHDLTokenList :: tokenWithName(const char* name) {
  return (VHDLToken*) myTable.lookup(name);
}

// Return a pointer to a new copy of the list.
VHDLTokenList* VHDLTokenList :: newCopy() {
  // Put new copy of each entry into the new list.
  VHDLTokenList* newTokenList = new VHDLTokenList;
  VHDLTokenListIter tokenNext(*this);
  VHDLToken* nextToken;
  // Iterate through this list, and create new copies of each entry.
  while ((nextToken = tokenNext++) != 0) {
    VHDLToken* newToken = nextToken->newCopy();
    newTokenList->put(*newToken);
  }

  return newTokenList;
}
