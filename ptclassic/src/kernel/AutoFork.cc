static const char file_id[] = "AutoFork.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

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

PortHole* AutoFork::setSource (GenericPort &sp, int delay) {
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
	geo.numInitialParticles = delay;
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
		PortHole * forkInput;

		// create the Fork star
		if ((forkStar = KnownBlock::makeNew("Fork")) == 0 ||
		    (forkOutput = forkStar->multiPortWithName("output")) == 0
		    || (forkInput = forkStar->portWithName("input")) == 0) {
			Error::abortRun (geo, "can't create Fork star");
			if (forkStar) {
				LOG_DEL; delete forkStar;
				forkStar = 0;
			}
			return 0;
		}

		// install fork in parent galaxy
		geo.parent()->asGalaxy().addBlock(*forkStar, autoForkName());

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

const char* AutoFork::autoForkName() {
	char buf[16];
	sprintf (buf, "auto-fork-%d", ++nF);
	return hashstring (buf);
}
