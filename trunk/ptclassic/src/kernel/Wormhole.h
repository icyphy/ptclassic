/* 
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
*/
#ifndef _Wormhole_h
#define _Wormhole_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"
#include "Universe.h"
#include "Galaxy.h"
#include "StringList.h"
#include "EventHorizon.h"


/*******************************************************************
 SCCS Version identification :
 $Id$

 Programmer : Soonhoi Ha
 Date of Creation : 3/19/90
 Date of Revision : 5/31/90
 Date of Revision : 6/13/90
 Date of Revision : 6/20/90 

	The Wormhole class is defined in this file.
	A Wormhole is a Star with a component Galaxy
	and a Target to fire the blocks in the Galaxy.
	To the outside, a Wormhole behaves exactly like 
	a Star of the outside domain, although the Galaxy encapsulated
	in the Wormhole may be of a different domain.

	A Wormhole differs from a Universe in that it needs an
	interface between the inside and outside domains, which are
	often totally different. 

	The Wormhole class is intended to be used as a "mix-in"
	class with XXXStar to derive XXXWormhole.  The derivation
	graph of Wormhole is:
	Runnable -->   Wormhole --> SDFWormhole <-- SDFStar
		 		--> DEWormhole  <-- DEStar
		 		--> XXXWormhole  <-- XXXStar

********************************************************************/

	//////////////////////////////
	// Wormhole
	//////////////////////////////
	
class Wormhole : public Runnable {
public:
	// Constructors.  We never use plain Wormholes.  We always have
	// class SDFWormhole : public Wormhole, public SDFStar, etc.
	// There we do
	// SDFWormhole::SDFWormhole(Galaxy& g,Target* t) :
	//	Wormhole(*this,g,t) { buildEventHorizons();}

	Wormhole(Star& self, Galaxy& g, Target* innerTarget = 0);

	// This form builds the Target from the KnownTarget list by
	// using targetName.
	Wormhole(Star& self, Galaxy& g, const char* targetName = 0);

	// return the inside domain
	const char* insideDomain() const { return gal.domain(); }

	// Return the scheduler of the outer domain.
	// Note thar the scheduler of the inner domain can be get
	// by scheduler() method of derived wormhole classes.
	Scheduler* outerSched();

	// redefine setStopTime()
	void setStopTime(double stamp);

	// explode myself to expose the inside Galaxy.
	Galaxy* explode();

	// Return a reference to the internal galaxy.  This is a non-const
	// function because the galaxy may be modified by the calling function.
        Galaxy& insideGalaxy() { return gal; }
	

protected :
	// The following three methods define the default, common behavior
	// of the "setup", "begin", and "go" methods of the XXXWormhole
	// classes.  Those methods of the XXXWormhole classes should be
	// defined to invoke these methods, and possibly do some other
	// domain-specific things.

	// Defines the default, common behavior of XXXWormhole::setup().
	void setup();

	// Defines the default, common behavior of XXXWormhole::begin().
	void begin();

	// Defines the default, common behavior of XXXWormhole::go().
	int run();

	// function to connect inner galaxy ports to outside through
	// event horizons of proper type
	void buildEventHorizons ();

	// function to delete the event horizons and inside galaxy,
	// to be called from XXXWormhole destructors
	void freeContents ();

	// Get the stopping condition for the inner domain.
	// Must be redefined in the derived class.
	virtual double getStopTime() = 0;

	// arrange things after run if necessary
	virtual void sumUp();

	StringList print (int recursive) const;

	// reference to my Galaxy part
	Galaxy& gal;
private :
	// reference to myself as a Star
	Star& selfStar;

	// flag saying whether to zap event horizons in destructor
	short dynamicHorizons;

	// check ready
	int checkReady() const;

};

#endif
