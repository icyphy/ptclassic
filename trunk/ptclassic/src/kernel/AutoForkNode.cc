/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 11/7/90

 This glass is a Geodesic that automatically generates auto-forks
 when attempts are made to connect multiple outputs to it.  It forbids
 the specification of multiple inputs.
*************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "AutoForkNode.h"
#include "KnownBlock.h"
#include "Galaxy.h"
#include "miscFuncs.h"

// variable used for generating names
int AutoForkNode::nF = 0;

// generate a new name for autofork stars
const char* AutoForkNode::autoForkName() {
	char buf[16];
	sprintf (buf, "!af%d", ++nF);
	return savestring (buf);
}
	
// this type is persistent
int AutoForkNode::isItPersistent () const {
	return TRUE;
}

// make a new source connection
PortHole* AutoForkNode::setSourcePort (GenericPort &sp, int delay) {
	if (originatingPort) {
		Error::abortRun(*this, 
			"multiple output ports found on the same node");
		return 0;
	}
// MultiPortHole: in this case the Geodesic is really not used, we
// just set up to make connections to the given multiporthole by setting
// forkOutput to be equal to it.
	if (sp.isItMulti()) {
		if (forkOutput) {
			Error::abortRun(*this, 
					"Multiport input must be first");
			return 0;
		}
		forkOutput = (MultiPortHole *)&sp;
		if (destinationPort) {
			forkOutput->connect(*destinationPort,0);
			PortHole* result = destinationPort->far();
			destinationPort = 0;
			return result;
		}
		else return &sp.newConnection();
	}
// normal case, simple enough.
	originatingPort = &sp.newConnection();
	portHoleConnect();
	numInitialParticles = delay;
	return originatingPort;
}

// make a new destination connection, possibly autoforking
// this REQUIRES that there be a star in the current domain named Fork
// with a porthole named input and a multiporthole named output.
PortHole* AutoForkNode::setDestPort (GenericPort &gp) {
	PortHole& dp = gp.newConnection();
	// already autoforked -- connect new port to Fork output multiport
	if (forkOutput) {
		forkOutput->connect(dp, 0);
		return &dp;
	}
	// no output -- use this one
	else if (!destinationPort) {
		destinationPort = &dp;
	}
	// second connection: must make a Fork
	else {
		PortHole * forkInput;

		// create the Fork star
		if ((forkStar = KnownBlock::clone("Fork")) == 0 ||
		    (forkOutput = forkStar->multiPortWithName("output")) == 0
		    || (forkInput = forkStar->portWithName("input")) == 0) {
			Error::abortRun (*this, "can't create Fork star");
			if (forkStar) {
				delete forkStar;
				forkStar = 0;
			}
			return 0;
		}

		// install fork in parent galaxy
		parent()->asGalaxy().addBlock(*forkStar, autoForkName());

		// connect old output connection to Fork instead.
		// also connect new output, and connect to the Fork.
		forkOutput->connect(*destinationPort, 0);
		destinationPort = forkInput;
		forkOutput->connect(dp, 0);
	}
	portHoleConnect();
	return &dp;
}

// destructor
AutoForkNode::~AutoForkNode () {
// for now rely on parent galaxy's destructor to wipe out forkStar
//	delete forkStar;
	// remaining stuff handled by baseclass destructor
}
