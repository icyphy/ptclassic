static const char file_id[] = "CGWormTarget.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

Programmer: Jose Luis Pino

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGWormTarget.h"
// #include "CodeStream.h"
#include <iostream.h>

CGWormTarget(const char* name,const char* starType,const char* desc):
CGSharedBus(name,starType,desc) {
//    childType.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
    childTargets.put("default-CGC ","default-CGC");
    schedName.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
    oneStarOneProc.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
}

void CGWormTarget::setup() {
    oneStarOneProc.setCurrentValue("YES");
    schedName.setCurrentValue("Hier");
    // CodeStream childTargets;
    GalStarIter nextStar;
    CGStar* star;
    while ((star = (CGStar*)nextStar++) != NULL) {
	Wormhole* worm = ((CGStar*)star)->asWormhole();
	BlockPortIter nextPort(*star);
	PortHole* port;
	while ((port = nextPort++) != NULL) {
	    if (! port->far()->parent()->isA("CGCStar") && 
		strcmp(worm->insideDomain(),"CGC") != 0) {
		PortHole* farPort = port->far();
		int numDelays = port->numInitDelays();
		const char* delayValues = port->initDelayValues();
		port->disconnect();
	    }
	}
    }
    CGSharedBus::setup();
}

static CGWormTarget targ("WormTarget","CGStar","A CG wormhole target");

static KnownTarget entry(targ,"WormTarget");
