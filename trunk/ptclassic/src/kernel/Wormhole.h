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

	Wormholes are defined in this file.
	A Wormhole is a Star with a pointer to a component Galaxy,
	and a Target to fire the blocks in the Galaxy.
	From the outside, #in%_Wormhole looks exactly like a % Star.

	Wormhole differs from "Universe" in that it needs interface
	between two totally different domains. 

	Derivation graph of Wormhole is
	Runnable -->   Wormhole --> SDFWormhole <-- SDFStar
		 		--> DEWormhole  <-- DEStar
		 		--> ??Wormhole  <-- ??Star

********************************************************************/

	//////////////////////////////
	// Wormhole
	//////////////////////////////
	
class Wormhole : public Runnable {
public:
	// constructors.  We never use plain Wormholes, we always have
	// class SDFWormhole : public Wormhole, public SDFStar
	// here we do
	// SDFWormhole::SDFWormhole(Galaxy& g,Target* t) :
	//	Wormhole(this,g,t) { buildEventHorizons();}

	Wormhole(Star& self, Galaxy& g, Target* innerTarget = 0);

	// This form builds the Target from the KnownTarget list by
	// using targetName
	Wormhole(Star& self, Galaxy& g, const char* targetName = 0);

	void setup();

	int run();

	// return the inside Domain
	const char* insideDomain() const { return gal.domain(); }

	// redefine setStopTime()
	void setStopTime(double stamp);

protected :
	// function to connect inner galaxy ports to outside through
	// event horizons of proper type
	void buildEventHorizons ();

	// function to delete the event horizons and inside galaxy,
	// to be called from XXXWormhole destructors.
	void freeContents ();

	// get the stopping condition for the inner domain.
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
