#ifndef _WormConnect_h
#define _WormConnect_h 1

#include "NamedObj.h"
#include "DataStruct.h"
#include "Connect.h"

#define grabData beforeGo
#define sendData afterGo

enum inOutType {IN, in, OUT, out};
class Star;
class Wormhole;

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/31/90
 Date of revision: 6/15/90
 Revisions:
	EventHorizon is defined.
	EventHorizon is PortHole associated with Wormholes.
******************************************************************/

/*********************************************************************

 EventHorizon:

 For Wormholes, we need another class of PortHoles.

 *********************************************************************/

        //////////////////////////////////////////
        // class EventHorizon
        //////////////////////////////////////////

//  This PortHole links two PortHoles inside and outside a Wormhole.
class EventHorizon : public PortHole
{
public:
	// We need another set of connection information
	// between boundary and inside of the wormhole.
	EventHorizon* ghostPort;
	
	// Connect two EventHorizons.
	void ghostConnect(EventHorizon& to );

	// TimeStamp of the current data, which is necessary for interface
	// of two domains. 
	float timeStamp;

	// Is data new?
	int dataNew;

	// Which wormhole it is in.
	Wormhole* wormhole;

	// is it Input or Output? -- depends on the location.
	int isItInput();
	int isItOutput();

	// set ports
	PortHole& setPort(inOutType inOut, const char* portName, 
		Wormhole* parentWormhole, Star* parentStar,
		dataType type = FLOAT, unsigned numTokens = 1 );

private:
	int inOrOut;
};

        //////////////////////////////////////////
        // class ToEventHorizon
        //////////////////////////////////////////

// Input EventHorizon
class ToEventHorizon : public EventHorizon
{
protected:

	// trasfering data from outside to Universal EventHorizon
	void getData() ;

	// transfer data from Universal EventHorizon to ghostPort.
	void transferData();

public:
	// redefine initialize()
	void initialize(); 

	// for timed domain, it returns the next global time of the 
	// outside domain. This value is used to set the stop time
	// of the inside "timed" domain.
	// for untimed domain, it gives the value when to stop the
	// inside "timed" domain.
	virtual float getNextStamp() = 0;

	// flag to check that the inner domain finishes its scheduling
	// before deadlocked. For untimed domain, they are always FALSE.
	// But for timed domains, it varies at runtime.
	int outputBeforeDeadlocked;
			   
};

        //////////////////////////////////////////
        // class FromEventHorizon
        //////////////////////////////////////////

// Output EventHorizon
class FromEventHorizon : public EventHorizon
{
friend class Wormhole;
protected:

	//transfer data from Universal EventHorizon to outside
	void putData() { putParticle();
			 dataNew = FALSE; }

	// fire ghostPort :: grabData to get Data
	void transferData();

	// make sure this port is ready for inside Galaxy if it is an input.
	// if ready, set up the stopping condition for the inner-domain.
	virtual int ready();
			   
public:
	// redefine initialize()
	void initialize(); 

};

#endif
