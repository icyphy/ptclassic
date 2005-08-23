static const char file_id[] = "AutoFork.cc";
/**************************************************************************
Version identification:
@(#)AutoFork.cc	2.12	11/10/97

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

 Programmer: J. Buck
 Date of creation: 2/13/92

 This used to be part of AutoForkNode.  It is now a separate class,
 to make it easier to define other types of auto-forking nodes
 (such as AsmForkNode for assembly language stars).

*************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "AutoFork.h"
#include "Geodesic.h"
#include "PortHole.h"
#include "Error.h"
#include "KnownBlock.h"
#include "Galaxy.h"

AutoFork::AutoFork(Geodesic& g)
: geo(g), forkStar(0), forkInput(0), forkOutput(0) {}

PortHole* AutoFork::setSource (GenericPort &sp, int numDelays,
			       const char* initDelayValues) {
	if (geo.originatingPort) {
		Error::abortRun(geo, 
			"multiple output ports found on the same node");
		return 0;
	}
// MultiPortHole: in this case the Geodesic is really not used, we
// just set up to make connections to the given multiporthole by setting
// forkOutput to be equal to it.
	if (sp.isItMulti()) {
		if (forkOutput) {
			Error::abortRun(geo, 
					"Multiport input must be first");
			return 0;
		}
		forkOutput = (MultiPortHole *)&sp;
		if (geo.destinationPort) {
			forkOutput->connect(*geo.destinationPort,0);
			PortHole* result = geo.destinationPort->far();
			geo.destinationPort = 0;
			return result;
		}
		else return &sp.newConnection();
	}
// normal case, simple enough.
	geo.originatingPort = &sp.newConnection();
	geo.portHoleConnect();
	geo.setDelay(numDelays, initDelayValues);
	return geo.originatingPort;
}

// if alwaysFork is set, we always make a fork star.
PortHole* AutoFork::setDest (GenericPort &gp, int alwaysFork) {
	PortHole& dp = gp.newConnection();
	// already autoforked -- connect new port to Fork output multiport
	if (forkOutput) {
		forkOutput->connect(dp, 0);
		return &dp;
	}
	// no output -- use this one
	else if (alwaysFork == 0 && !geo.destinationPort) {
		geo.destinationPort = &dp;
	}
	// second connection: must make a Fork
	else {
		Galaxy& parentGal = geo.parent()->asGalaxy();
		const char* dom = parentGal.domain();

		// create the Fork star
		if ((forkStar = KnownBlock::makeNew("Fork", dom)) == 0 ||
		    (forkOutput = forkStar->multiPortWithName("output")) == 0
		    || (forkInput = forkStar->portWithName("input")) == 0) {
			Error::abortRun (geo, "can't create Fork star");
			if (forkStar) {
				LOG_DEL; delete forkStar;
				forkStar = 0;
			}
			forkInput = 0;
			forkOutput = 0;
			return 0;
		}

		// install fork in parent galaxy
		parentGal.addBlock(*forkStar, autoForkName());

		// connect old output connection to Fork instead.
		// also connect new output, and connect to the Fork.
		if (!alwaysFork)
			forkOutput->connect(*geo.destinationPort, 0);
		geo.destinationPort = forkInput;
		forkOutput->connect(dp, 0);
	}
	geo.portHoleConnect();
	return &dp;
}

int AutoFork::nF = 0;

// Generate a name for the auto-fork, and hash and return it.
// If the geodesic is named, its name is appended to "auto-fork-"
// to form the auto-fork name.  Otherwise, a unique integer is
// appended.
const char* AutoFork::autoForkName() {
	StringList buf = "auto-fork-";
	const char* geoName = geo.name();
	if (geoName && *geoName)
		buf << geoName;		// geodesic is named
	else
		buf << ++nF;		// geodesic is unnamed
	return hashstring (buf);
}

// destructor: remove fork star if present.  We rely on the geodesic's
// destructor to do any disconnection work that remains after deleting
// the fork star.

AutoFork::~AutoFork() {
	if (forkStar) {
		forkStar->parent()->asGalaxy().removeBlock(*forkStar);
		LOG_DEL; delete forkStar;
	}
}
