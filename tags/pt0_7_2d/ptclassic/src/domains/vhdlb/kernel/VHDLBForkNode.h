#ifndef _VHDLBForkNode_h
#define _VHDLBForkNode_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
@(#)VHDLBForkNode.h	1.5 3/2/95

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

 This class is an auto-forking version of VHDLBGeodesic.  It is to
 VHDLBGeodesic as AutoForkNode is to Geodesic.  A template could be
 used to generate both.

*************************************************************************/
#include "VHDLBGeodesic.h"
#include "AutoFork.h"

class VHDLBForkNode : public VHDLBGeodesic {
public:
	// constructor: make the VHDLBFork object refer to me.
	VHDLBForkNode() : af(*this) {}
	int isItPersistent () const;
	PortHole* setSourcePort (GenericPort &, int numDelays = 0,
				 const char* initDelayValues = 0);
	PortHole* setDestPort (GenericPort &);

	// class identification
	int isA(const char*) const;

private:
	AutoFork af;
};
#endif
