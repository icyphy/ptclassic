#ifndef _CGWormBase_h
#define _CGWormBase_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Wormhole.h"
#include "Profile.h"
#include "MultiTarget.h"
#include "EventHorizon.h"

/*******************************************************************
 SCCS Version identification :
 @(#)CGWormBase.h	1.9	1/1/96

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

 Programmer : Soonhoi Ha
 Date of Creation : 2/21/93
	
It is the base wormhole class for code generation domains.
For now, it supports CGDDF domain only. To support other code generation
domain inside, further modification is required.

********************************************************************/

// CGWormBase can not contain a timed-domain. It may contain another
// type of CGdomain.

	//////////////////////////////
	// CGWormBase
	//////////////////////////////

class CGWormBase : public Wormhole {
public:
	// Constructor
	CGWormBase(Star& s, Galaxy& g, Target* t = 0);

	// return TRUE if inside domain is a code generation domain.
    	int isCGinside() { 
	    return myTarget()->isA("CGTarget");
	}

        // execution time
        int execTime;
};

#endif
