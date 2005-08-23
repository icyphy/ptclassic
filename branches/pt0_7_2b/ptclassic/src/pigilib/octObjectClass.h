/**************************************************************************
Version identification:
@(#)octObjectClass.h	1.4	11/27/95

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
		       
 Programmer:  Brian Evans
 Date of creation: October 1995

This file implements a class that handles allocation and deallocation
of oct objects.

**************************************************************************/

#ifndef _octObjectClass_h
#define _octObjectClass_h 1

extern "C" {
#include "oct.h"
#include "octIfc.h"
}

class octObjectClass {

public:
	// constructor
        octObjectClass() {
		facet.type = OCT_UNDEFINED_OBJECT;
		facet.objectId = 0;
	}

	// destructor
        ~octObjectClass() { FreeOctMembers(&facet); }

	// Cast to octObject*
	inline operator octObject* () { return &facet; }

protected:
	octObject	facet;
};

#endif		// _POct_h
