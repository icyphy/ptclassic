#ifndef _Wormhole_h
#define _Wormhole_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "type.h"
#include "Universe.h"
#include "Galaxy.h"
#include "StringList.h"
#include "WormConnect.h"


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
	and a Scheduler to fire the blocks in the Galaxy.
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
	// Connect the Wormhole Ports to the inside Galaxy.
	void ghostConnect(EventHorizon& from, EventHorizon& to) {
		from.ghostConnect(to);
	}

	void setup() { initSched(); 
		       scheduler->stopBeforeDeadlocked = FALSE ;}

	void run() { if (!checkReady()) return;
		     setStopTime();
		     Runnable :: run(); 
		     sumUp();}

	// redefine setStopTime()
	void setStopTime();

	// constructor.  We never use plain Wormholes, we always have
	// class SDFWormhole : public Wormhole, public SDFStar
	// here we do
	// SDFWormhole::SDFWormhole(Galaxy& g) : Wormhole(this,g) {...}

	Wormhole(Star& self, Galaxy& g);

	// function to connect inner galaxy ports to outside through
	// event horizons of proper type
	void buildEventHorizons ();

	// Destructor
	~Wormhole ();

protected :
	// get the stopping condition for the inner domain.
	// SHOULD be redefined in the derived class.
	virtual float getStopTime();

	// arrange things after run if necessary
	virtual void sumUp();

	StringList print (int recursive) const;

	// reference to myself as a Star
	Star& selfStar;

private :
	// flag saying whether to zap event horizons in destructor
	int dynamicHorizons;

	// check ready
	int checkReady() const;

};

#endif
