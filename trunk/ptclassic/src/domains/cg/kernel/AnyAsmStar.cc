/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

This file is the support for the "AnyAsm" pseudo-domain.  It defines
AnyAsmStar's methods and makes an AnyAsmDomain class and prototype
object.

I would prefer not needing the AnyAsm domain object, and have a way
of registering it as a pseudo-domain instead.

*******************************************************************/

static const char file_id[] = "AnyAsmStar.cc";

#include "AnyAsmStar.h"
#include "Domain.h"
#include "SDFWormhole.h"
#include "AutoForkNode.h"

const char* AnyAsmStar::domain() const { return "AnyAsm";}

ISA_FUNC(AnyAsmStar,AsmStar);

class AnyAsmDomain : public Domain {
private:
	void complain() {
		Error::abortRun("AnyAsm is a pseudo-domain -- do not attempt",
			"\nto build galaxies using this domain");
	}
public:
	// new wormhole
        Star& newWorm(Galaxy&, Target*)  {
		complain();
                LOG_NEW; return *new AnyAsmStar;
        }

	// new input porthole
	PortHole& newInPort() {
		complain();
		LOG_NEW; return *new InCGPort;
	}

	// new output porthole
	PortHole& newOutPort() { return newInPort();}

	// new fromUniversal EventHorizon
        EventHorizon& newFrom() {
		complain();
                LOG_NEW; return *new SDFfromUniversal;
        }

	// new toUniversal EventHorizon
        EventHorizon& newTo() {
		complain();
                LOG_NEW; return *new SDFtoUniversal;
        }

	// new node (geodesic)
	Geodesic& newNode() { complain(); LOG_NEW; return *new AutoForkNode;}

	// constructor
	AnyAsmDomain() : Domain("AnyAsm") {}
};

// declare a prototype
static AnyAsmDomain proto;



