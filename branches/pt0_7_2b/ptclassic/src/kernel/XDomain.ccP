// A device to produce the correct portholes, wormholes, event horizons,
// etc, for the <T> domain
// $Id$ (<T>Domain.cc)

#include "Domain.h"
#include "<T>Scheduler.h"
#include "<T>Wormhole.h"
#include "<T>Connect.h"
#include "<T>WormConnect.h"

extern const char <T>domainName[] = "<T>";

class <T>Domain : public Domain {
public:
	// new scheduler
	Scheduler& newSched() { return *new <T>Scheduler;}

	// new wormhole
	Star& newWorm(Galaxy& innerGal)  {
		return *new <T>Wormhole(innerGal);
	}

	// new input porthole
	PortHole& newInPort() { return *new In<T>Port;}

	// new output porthole
	PortHole& newOutPort() { return *new Out<T>Port;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { return *new <T>fromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { return *new <T>toUniversal;}

	// constructor
	<T>Domain() : Domain("<T>") {}
};

// declare a prototype
static <T>Domain proto;
