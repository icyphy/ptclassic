static const char file_id[] = "VHDLTarget.cc";
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

 Base target for VHDL code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLTarget.h"

// Add code to the beginning of a CodeStream instead of the end.
void prepend(StringList code, CodeStream& stream)
{
  StringList temp;
  temp << code;
  temp << stream;
  stream.initialize();
  stream << temp;
}

// constructor
VHDLTarget :: VHDLTarget(const char* name,const char* starclass,
		       const char* desc) : HLLTarget(name,starclass,desc) {
}

// Setup the target.
void VHDLTarget :: setup() {
	if (galaxy()) setStarIndices(*galaxy()); 
	HLLTarget::setup();
}

// Initial stage of code generation.
void VHDLTarget :: headerCode()
{
  // Do nothing.
}

// Trailer code.
void VHDLTarget :: trailerCode()
{
  // Do nothing.
}

// Combine all sections of code.
void VHDLTarget :: frameCode()
{
  StringList code = headerComment();

  // Prepend the header.
  prepend(code, myCode);
}

// Write the code to a file.
void VHDLTarget :: writeCode()
{
  writeFile(myCode,".vhd",displayFlag);
}

// Compile the code.
int VHDLTarget :: compileCode()
{
  // Do nothing.
}

// Run the code.
int VHDLTarget :: runCode()
{
  // Do nothing.
}

/////////////////////////////////////////
// codeGenInit
/////////////////////////////////////////

int VHDLTarget :: codeGenInit() {
  // call initCode for each star.
  GalStarIter nextStar(*galaxy());
  VHDLStar* s;
  while ((s = (VHDLStar*) nextStar++) != 0) {
    s->initCode();	
  }
  return TRUE;
}

ISA_FUNC(VHDLTarget,HLLTarget);
