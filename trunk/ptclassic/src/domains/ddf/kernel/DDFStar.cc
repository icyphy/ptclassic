static const char file_id[] = "DDFStar.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DDFStar.h"
#include "Scheduler.h"
#include "DDFWormhole.h"

/*******************************************************************

	class DDFStar methods

********************************************************************/


// initialize DDF specific members
void DDFStar :: initialize() {
	clearWaitPort();
	DataFlowStar::initialize();
	// special care for DDFSelf star (recursion)
	// adjust numberTokens
	if (isItSelf()) {
		// If error happens (from setup call), return
		if (Scheduler :: haltRequested()) return;
		BlockPortIter nextp(*this);
		for (int i = numberPorts(); i > 0; i--) {
			DDFPortHole& dp = *(DDFPortHole*) nextp++;
			int nTok = dp.imagePort->numberTokens;
			if (nTok > 1)
				dp.setDDFParams(nTok);
		}
	}
}

int DDFStar :: run() { return Star :: run(); }
		
// The following is defined in DDFDomain.cc -- this forces that module
// to be included if any DDF stars are linked in.
extern const char DDFdomainName[];

const char* DDFStar :: domain () const { return DDFdomainName;}

int DDFStar :: isItSelf() { return FALSE ;}

// return NULL
DDFWormhole* DDFStar :: myWorm() { return NULL; }

const char* DDFStar :: readTypeName () { return "unspecified method";}

ISA_FUNC(DDFStar,DynDFStar);
