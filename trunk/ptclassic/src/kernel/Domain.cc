/*******************************************************************
SCCS Version identification :
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck
 Date of creation: 6/29/90

 The Domain class.  We declare one instance of each derived domain.
 This class provides a way to get the proper types of various objects
 for a domain, and allows Wormholes to be automatically generated.

*******************************************************************/

#include "Domain.h"
#include "Star.h"
#include "Galaxy.h"
#include "Scheduler.h"

int Domain::numDomains = 0;

Domain* Domain::allDomains[NUMDOMAINS];

// return ptr to domain matching given name
Domain* Domain::named(const char* nm) {
	if (!nm) return 0;
	for (int i = 0; i < numDomains; i++) {
		if (strcmp (nm, allDomains[i]->name) == 0)
			return allDomains[i];
	}
	return 0;
}

// get the domain of a block.  For a galaxy, it is the domain of the
// first star we find (since we don't allow multiple domains in the
// same galaxy, this is not ambiguous)

Domain* Domain::domainOf (Block& b) {
	Block *pb = &b;
	if (pb->isItAtomic()) return named(pb->asStar().domain());
	return named(pb->asGalaxy().myDomain);
}

