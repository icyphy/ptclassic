static const char file_id[] = "<T>Domain.cc";
/**********************************************************************
Version identification:
$Id$ (<T>Domain.cc)

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 7/2/90

 WARNING -- XDomain.ccP is a template file that is used to generate
 domain description modules.  If the name of this file is not XDomain.ccP,
 DO NOT EDIT IT!!!

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the <T> domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "Target.h"
#include "KnownTarget.h"
#include "<T>Scheduler.h"
#include "<T>Wormhole.h"
#include "<T>Geodesic.h"

extern const char <T>domainName[] = "<T>";

class <T>Domain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new <T>Wormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new <T>fromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new <T>toUniversal;}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new <T>Geodesic;}

	// constructor
	<T>Domain() : Domain("<T>") {}
};

// declare a prototype
static <T>Domain proto;

// declare the default Target object

class <T>Target : public Target {
public:
	<T>Target() : Target("default-<T>","<T>Star","default <T> target"){}
	void setup()
	{
		if (!scheduler()) { LOG_NEW; setSched(new <T>Scheduler); }
		scheduler()->setGalaxy(*galaxy());
		Target::setup();
	}
	Block* makeNew() const { LOG_NEW; return new <T>Target;}
	~<T>Target() { delSched();}
};

static <T>Target default<T>target;
static KnownTarget entry(default<T>target,"default-<T>");
