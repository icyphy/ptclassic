static const char file_id[] = "VHDLGeodesic.cc";
/******************************************************************
Version identification:
@(#)VHDLGeodesic.cc	1.8 02/01/96

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

 geodesics for VHDL stars.  In addition to the functions of CG
 geodesics, they manage memory allocation for buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLGeodesic.h"
#include "VHDLPortHole.h"
#include "Error.h"

// Return the name assigned to the buffer.
// If I am a fork destination, my buffer name is that of the source.
char* VHDLGeodesic :: getBufName() const {
  const VHDLPortHole* p = (const VHDLPortHole*)src();
  return p ? p->geo().getBufName() : bufName;
}

// Initialize the geodesic.
void VHDLGeodesic :: initialize() {
  // Must call CGGeodesic::initialize() first else numInit() won't work.
  CGGeodesic::initialize();

// Old: - Causes VaryDelay demo to fail
//  firstIn = numInit();
//  firstOut = 0;

// New: - Seems to work like it should
  firstIn = 0;
  firstOut = -numInit();

//  nextIn = firstIn - 1;
//  nextOut = firstOut - 1;
  nextIn = firstIn;
  nextOut = firstOut;
}

// Update token put position by specified number of tokens.
void VHDLGeodesic :: putTokens(int num) {
  nextIn += num;
}

// Update token get position by specified number of tokens.
void VHDLGeodesic :: getTokens(int num) {
  nextOut += num;
  if (nextOut > nextIn) {
    // Don't worry about it (for now) if over-read from wormhole port.
    if (!(destPort()->atBoundary())) {
      Error::error(*this, "Read from geodesic beyond available tokens");
    }
  }
}

ISA_FUNC(VHDLGeodesic,CGGeodesic);
