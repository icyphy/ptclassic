#ifndef _Wormhole_h
#define _Wormhole_h 1

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

	Basic Wormhole class has the following state (may need more states).
	FloatState delay : which is the length of simulated execution
			   time of the wormhole.
	start() go() wrapup() functions are virtual since they may be
		redefined in the derived class.

	Let me explain the structure of specific Wormhole using SDFWormhole.
	1. SDFWormhole class realizes ??inSDF_Wormholes. 
	2. It has same members as SDFStar since from the outside, wormhole
	   is indistinguishable from star. The scheduler not user should 
	   take care of the difference.
	3. It may redefine some virtual functions to fit to the outside 
	   world regardless of what is inside.
	   e.g. go() function now set state "samplingPeriod" of inside
	   galaxy as well as running the inside-scheduler.
	4. It may have some states. setBlock() function set defaults for them.
	   e.g. "iterationPeriod" is necessary to calculate the "delay".

	
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

	// State to be set by users how long is the simulated time
	// of the inside Domain.
	// for DEWormhole : simulated delay
	// for SDFWormhole : sampling delay (period)
	double messageProcessingTime;

	// When it starts execution
	double currentTime;

	// Methods to  print out description.
	StringList printVerbose() { return print(0);}
	StringList printRecursive() { return print(1);}

	// virtual star functions
	// "start" may be redefined if we want to adjust the state values
	// after the scheduling is done. "go" is usually redefined for
	// timeStamping business on EventHorizon.
	void setup() { initSched();} 
	//	       checkSDF(); }

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
	StringList print (int recursive);

private :
	// reference to myself as a Star
	Star& selfStar;

	// flag saying whether to zap event horizons in destructor
	int dynamicHorizons;

	// Restriction 1 : SDFDomain should consume one token/arc and
	//		   generate one token/arc as an inside Domain.
	// void checkSDF(); 
};

#endif
