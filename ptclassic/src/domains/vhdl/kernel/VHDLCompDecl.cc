static const char file_id[] = "VHDLCompDecl.cc";
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

 Methods for VHDL CompDecls.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLCompDecl.h"

// Constructors.
VHDLCompDecl :: VHDLCompDecl() {
  VHDLObj::initialize();
}

VHDLCompDecl :: VHDLCompDecl(const char* n, Block* p, const char* d) : VHDLObj(n,p,d) {
  VHDLObj::initialize();
}

// Destructor.
VHDLCompDecl :: ~VHDLCompDecl() {}

// Return a pointer to a new copy of the VHDLCompDecl.
VHDLCompDecl* VHDLCompDecl :: newCopy() {
  VHDLCompDecl* newCompDecl = new VHDLCompDecl;
  newCompDecl->name = this->name;
  newCompDecl->portList = this->portList->newCopy();
  newCompDecl->genList = this->genList->newCopy();

  return newCompDecl;
}

// Class identification.
const char* VHDLCompDecl :: className() const { return "VHDLCompDecl"; }

ISA_FUNC(VHDLCompDecl,VHDLObj);
