#include "DEConnect.h"
#include "Block.h"
#include "StringList.h"
#include "Output.h"
#include "DEStar.h"
#include "PriorityQueue.h"
#include "WormConnect.h"
 
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revisions:

Code for functions declared in DEConnect.h

**************************************************************************/

extern Error errorHandler;
extern PlasmaList plasmaList;

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
// Return a current Particle 
	dataNew = TRUE;
	timeStamp = stamp;
	return *(Particle *) *(myBuffer->here());
}	

void OutDEPort :: sendData ()
{
// Two options are possible. (1) Scheduler calls it, or (2) user calls
// it in go() definition. I choose (1) since it is parallel with SDF case.
// It sends a data to the output Geodesic, and generates an event into the Q
// inside the DE system. At the DEin##_WormHole boundary, no event is signaled.
   if (dataNew) {
	putParticle();

	// If the port lies on the Wormhole boundary, just inform timeStamp.
	// Otherwise, generate an event into the event queue.
	if (farSidePort->isItInput() == TRUE) {
		Star& s = parent()->asStar();
		DEStar* dp = (DEStar*) &s;
		dp->eventQ->levelput(farSidePort, timeStamp);
	} else {
		EventHorizon* p = (EventHorizon *) farSidePort;
		p->timeStamp = timeStamp;
	}
	
	dataNew = FALSE;
   }
}	

PortHole& MultiInDEPort :: newPort () {
        InDEPort* newport = new InDEPort;
        ports.put(*newport);
        parent()->
            addPort(newport->
                        setPort(newName(), parent(), type));
	newport->typePort = typePort;
        return *newport;
}
 
 
PortHole& MultiOutDEPort :: newPort () {
        OutDEPort* newport = new OutDEPort;
        ports.put(*newport);
        parent()->
            addPort(newport->
                        setPort(newName(), parent(), type));
	newport->typePort = typePort;
        return *newport;
}
