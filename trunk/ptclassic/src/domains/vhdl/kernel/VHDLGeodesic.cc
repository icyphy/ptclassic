static const char file_id[] = "VHDLGeodesic.cc";
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

 geodesics for VHDL stars.  In addition to the functions of CG
 geodesics, they manage memory allocation for buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLGeodesic.h"
#include "VHDLPortHole.h"
#include "Error.h"
#include <ostream.h>

// Return the name assigned to the buffer.
// If I am a fork destination, my buffer name is that of the source.
char* VHDLGeodesic :: getBufName() const {
  const VHDLPortHole* p = (const VHDLPortHole*)src();
  return p ? p->geo().getBufName() : bufName;
}

// Initialize the geodesic.
void VHDLGeodesic :: initialize() {
  nextIn = 0;
  nextOut = 0;
  CGGeodesic::initialize();
}

// Update token put position by specified number of tokens.
void VHDLGeodesic :: putTokens(int num) {
//  cout << getBufName() << " nextIn from " << nextIn << " to ";
  nextIn += num;
//  cout << nextIn << "\n";
}

// Update token get position by specified number of tokens.
void VHDLGeodesic :: getTokens(int num) {
//  cout << getBufName() << " nextOut from " << nextOut << " to ";
  nextOut += num;
//  cout << nextOut << "\n";
  if (nextOut > nextIn) {
    // Don't worry about it (for now) if over-read from wormhole port.
    if (!(destPort()->atBoundary())) {
      Error::error(*this, "Read from geodesic beyond available tokens");
    }
  }
}

ISA_FUNC(VHDLGeodesic,CGGeodesic);
