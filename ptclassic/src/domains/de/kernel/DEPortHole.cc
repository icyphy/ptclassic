#include "DEConnect.h"
#include "DEScheduler.h"
#include "Block.h"
#include "StringList.h"
#include "Output.h"
#include "DEStar.h"
#include "PriorityQueue.h"
#include "WormConnect.h"
#include "CircularBuffer.h"
 
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions: If the galaxy inside a Wormhole has a output data, stop the
	DE scheduler if "stopAfterOutput" flag is set(10/24/90).

Code for functions declared in DEConnect.h

**************************************************************************/

extern Error errorHandler;

/**********************************************************

 Member functions for DEPortHoles..

 **********************************************************/

PortHole& DEPortHole :: setPort (
			     const char* s,
                             Block* parent,
                             dataType t = FLOAT)
{
	// Initialize PortHole
        PortHole::setPort(s,parent,t);

	// Initialize the DE-specific members
	timeStamp = 0.0;
	dataNew = FALSE;

        return *this;
}

Particle& InDEPort :: get() 
{
// Return a current Particle 
	dataNew = FALSE;
	return *(Particle *) *(myBuffer->here());
}	

void InDEPort :: grabData ()
{
	getParticle();
	dataNew = TRUE;
	timeStamp = ((DEStar*) parent())->arrivalTime;
}

Particle& OutDEPort :: put(float stamp)
{
// if there is a new Particle, send it
	if (dataNew) sendData();

// Return a current Particle 
	dataNew = TRUE;
	timeStamp = stamp;
	return *(Particle *) *(myBuffer->here());
}	

void OutDEPort :: sendData ()
{
// It sends a data to the output Geodesic, and generates an event into the Q
// inside the DE system. At the DEin##_WormHole boundary, no event is signaled.
   if (dataNew) {
	putParticle();

	// If the port lies on the Wormhole boundary, just inform timeStamp.
	// Otherwise, generate an event into the event queue.
	if (farSidePort->isItInput() == TRUE) {
		Star& s = far()->parent()->asStar();
		DEStar* dp = (DEStar*) &s;
		dp->eventQ->levelput(farSidePort, timeStamp, float(dp->depth));
	} else {
		EventHorizon* p = (EventHorizon *) farSidePort;
		DEScheduler* sr = (DEScheduler*) parent()->mySched();
		p->timeStamp = timeStamp / sr->relTimeScale;
	}
	
	dataNew = FALSE;
   }
}	

PortHole& MultiInDEPort :: newPort () {
	InDEPort& p = *new InDEPort;
	// DE-specific
	p.dataNew = FALSE;
	p.timeStamp = 0.0;
	return installPort(p);
}
 
 
PortHole& MultiOutDEPort :: newPort () {
	OutDEPort& p = *new OutDEPort;
	// DE-specific
	p.dataNew = FALSE;
	p.timeStamp = 0.0;
	return installPort(p);
}
