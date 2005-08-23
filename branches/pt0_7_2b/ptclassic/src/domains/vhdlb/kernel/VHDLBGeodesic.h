#ifndef _VHDLBGeodesic_h
#define _VHDLBGeodesic_h 1
/******************************************************************
Version identification:
@(#)VHDLBGeodesic.h	1.5 10/8/95

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

 Programmer: Edward A. Lee

 Geodesic for VHDLB code generation.
 In addition to fork management provide by VHDLBGeodesics,
 they manage definition and naming of buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "CGGeodesic.h"

class VHDLBGeodesic : public CGGeodesic {
public:
	VHDLBGeodesic() : bufName(0) {}
	~VHDLBGeodesic() {delete [] bufName;}

	// class identification
	int isA(const char*) const;

	void setBufName(char* n) {delete [] bufName; bufName = savestring(n);}
	char* getBufName() const;

private:
	// Name of the VHDLB variable used as a buffer
	char* bufName;
};
#endif
