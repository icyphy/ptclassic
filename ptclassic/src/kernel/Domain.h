/*******************************************************************
SCCS Version identification :
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: J. T. Buck
 Date of creation: 6/29/90
 Modified: 9/22/90, by EAL, to add mechanism for reading all known
		domain names.

 The Domain class.  We declare one instance of each derived domain.
 This class provides a way to get the proper types of various objects
 for a domain, and allows Wormholes to be automatically generated.

*******************************************************************/

#ifndef _Domain_h
#define _Domain_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "Block.h"

class EventHorizon;
class Geodesic;
class Target;

const int NUMDOMAINS = 20;		// maximum # of domains

class Domain {
public:
	Domain (const char* domname);
	// method for creating new wormholes
	virtual Star& newWorm(Galaxy& innerGal,Target* innerTarget = 0);

	// methods for creating event horizons
	virtual EventHorizon& newFrom();
	virtual EventHorizon& newTo();

	// Create a geodesic for a point-to-point connection
	// or a "node" suitable for multi-point connections.
	virtual Geodesic& newGeo(int multi = FALSE);

	// return true if wormholes should be made for all galaxies
	virtual int isGalWorm();

	// if non-null, return requirement for targets for use with this
	// domain
	virtual const char* requiredTarget() { return 0;}

	// name of this domain
	const char* name() { return myName;}

	// find Domain with given name
	static Domain* named(const char* name);

	// find Domain corresponding to argument Block
	static Domain* of(const Block& b) { return named(b.domain());}

	// return # of domains and nth domain
	static int number() {return numDomains;}
	static const char* nthName(int n) {return (allDomains[n])->myName;}

	// subdomains of this domain
	StringList subDomains;
private:
	// lists of domains
	static int numDomains;
	static Domain* allDomains[NUMDOMAINS];

	int index;
	const char* myName;
};
#endif
