// Domain.cc
// $Id$

#include "Domain.h"
#include "Star.h"
#include "Galaxy.h"

int Domain::numDomains = 0;

Domain* Domain::allDomains[NUMDOMAINS];

// return ptr to domain matching given name
Domain* Domain::named(const char* nm) {

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
	while (!pb->isItAtomic()) {
		Galaxy& g = pb->asGalaxy();
		if (g.numberBlocks() == 0) return 0;
		pb = &g.nextBlock();
	}
	return named(pb->asStar().domain());
}

