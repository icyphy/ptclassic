static const char file_id[] = "Domain.cc";
/*******************************************************************
SCCS Version identification :
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

 Programmer: J. T. Buck
 Date of creation: 6/29/90

 The Domain class.  We declare one instance of each derived domain.
 This class provides a way to get the proper types of various objects
 for a domain, and allows Wormholes to be automatically generated.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Domain.h"
#include "Star.h"
#include "Galaxy.h"
#include "Scheduler.h"
#include "EventHorizon.h"
#include "Error.h"

int Domain::numDomains = 0;

Domain* Domain::allDomains[NUMDOMAINS];

Domain :: Domain (const char* domname) {
	index = numDomains++;
	allDomains[index] = this;
	myName = domname;
}

// return ptr to domain matching given name
Domain* Domain::named(const char* nm) {
	if (!nm) return 0;
	for (int i = 0; i < numDomains; i++) {
		if (strcmp (nm, allDomains[i]->myName) == 0)
			return allDomains[i];
	}
	return 0;
}

// get the domain of a block.

Domain* Domain::of (Block& b) {
	return named(b.domain());
}

// default implementations provided for domains without wormhole
// support.

Star& Domain :: newWorm(Galaxy&,Target*)  {
	// return *new XXXWormhole(innerGal,innerTarget);
	Error::abortRun("No wormhole implemented for domain ",name());
		// Following is a hack
		LOG_NEW; return *(new Star);
}

// "fake" eventhorizon to use if there are none.

class JunkHorizon : public PortHole, public EventHorizon {
public:
	JunkHorizon() : EventHorizon(this) {}
	int isItOutput() const { return 0;}
	int isItInput() const { return 0;}
	void initialize() {}
};

EventHorizon& Domain :: newFrom() {
	Error::abortRun("No wormhole implemented for domain ",name());
	// hack
	LOG_NEW; return *new JunkHorizon;
}

EventHorizon& Domain :: newTo() {
	return Domain :: newFrom();
}

int Domain :: isGalWorm() {
	return FALSE;
}

