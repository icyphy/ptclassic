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
	// of two domains. If a EventHorizon has more than one buffer,
	// timeStamp should be associated with each buffer. (TimeStamp
	// becomes a circular buffer? or we derive another circular buffer
	// with timeStamp?)
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
		dataType type = FLOAT, float timeStamp = 0.0,
		unsigned numTokens = 1, unsigned delay = 0);

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

	// redefine initialize()
	void initialize(); 
			   
};

        //////////////////////////////////////////
        // class FromEventHorizon
        //////////////////////////////////////////

// Output EventHorizon
class FromEventHorizon : public EventHorizon
{
protected:

	//transfer data from Universal EventHorizon to outside
	void putData() { putParticle();
			 dataNew = FALSE; }

	// fire ghostPort :: grabData to get Data
	void transferData();

	// redefine initialize()
	void initialize(); 
			   
	// number of Data collected
	int numData;
};

#endif
