/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer: J. Buck

This file is the support for the "AnyCG" pseudo-domain.  It defines
AnyCGStar's methods and makes an AnyCGDomain class and prototype
object.

I would prefer not needing the AnyCG domain object, and have a way
of registering it as a pseudo-domain instead.

*******************************************************************/

static const char file_id[] = "AnyCGStar.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "AnyCGStar.h"
#include "Domain.h"
#include "CGWormhole.h"
#include "AutoForkNode.h"

const char* AnyCGStar::domain() const { return "AnyCG";}

ISA_FUNC(AnyCGStar,CGStar);

class AnyCGDomain : public Domain {
private:
	void complain() {
		Error::abortRun("AnyCG is a pseudo-domain -- do not attempt",
			"\nto build galaxies using this domain");
	}
public:
	// new wormhole
        Star& newWorm(Galaxy&, Target*)  {
		complain();
                LOG_NEW; return *new AnyCGStar;
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
                LOG_NEW; return *new CGfromUniversal;
        }

	// new toUniversal EventHorizon
        EventHorizon& newTo() {
		complain();
                LOG_NEW; return *new CGtoUniversal;
        }

	// new node (geodesic)
	Geodesic& newNode() { complain(); LOG_NEW; return *new AutoForkNode;}

	// constructor
	AnyCGDomain() : Domain("AnyCG") {}
};

// declare a prototype
static AnyCGDomain proto;



