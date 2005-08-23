static const char file_id[] = "VHDLBGeodesic.cc";
/******************************************************************
Version identification:
@(#)VHDLBGeodesic.cc	1.4 3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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

 Programmer: Edward A. Lee

 geodesics for VHDLB stars.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLBGeodesic.h"
#include "VHDLBPortHole.h"
#include "Error.h"

ISA_FUNC(VHDLBGeodesic,CGGeodesic);

// Return the name assigned to the buffer.
// If I am a fork destination, my buffer name is that of the source.
// Need to change this for the VHDLB code to be meaningful
char* VHDLBGeodesic::getBufName() const {
	const VHDLBPortHole* p = (const VHDLBPortHole*)src();
	return p ? p->geo().getBufName() : bufName;
}
