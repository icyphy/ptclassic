#include "WormConnect.h"
#include "Star.h"
#include "StringList.h"
#include "Output.h"
#include "Wormhole.h"
 
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/31/90
 Date of Revision: 6/13/90
 Revisions:
	make EventHorizon Universal.
        each doamin needs to define In??EventHorizon and Out??EventHorizon
        derived from Universal EventHorizon with domain-specific
  	translation.

Code for functions declared in WormConnect.h

**************************************************************************/

extern Error errorHandler;
extern PlasmaList plasmaList;

/**************************************************************************

	methods for EventHorizon

**************************************************************************/

void 
EventHorizon :: ghostConnect(EventHorizon& to)
{
	// make connection without Geodesic.
        to.ghostPort = this;
        ghostPort = &to;
}
 
PortHole& EventHorizon :: setPort (
			     inOutType inOut,
			     const char* s,
                             Wormhole* parentWormhole,
                             Star* parentStar,
                             dataType t = FLOAT,
			     float timeStamp = 0.0,
			     unsigned numTokens = 1,
			     unsigned delay = 0)
{
	// Initialize PortHole
        PortHole::setPort(s, parentStar,t);

	// Initialize the EventHorizon members
	timeStamp = 0.0;
	dataNew = FALSE;
	numberTokens = 1;
	bufferSize = numberTokens + delay;

	// set which wormhole it is in
	wormhole = parentWormhole;

	// in or out?
	inOrOut = inOut;

        return *this;
}

int EventHorizon :: isItInput() {
	if (inOrOut < 2) return TRUE;
	else		 return FALSE;
}

int EventHorizon :: isItOutput() {
	if (inOrOut > 1) return TRUE;
	else		 return FALSE;
}

/**************************************************************************

	methods for ToEventHorizon

**************************************************************************/

void ToEventHorizon :: getData()
{
	// check whether data exists or not
	if (myGeodesic->size() >= numberTokens) {
		getParticle();
		dataNew = TRUE;
	} else {
		dataNew = FALSE;
	}
}

void ToEventHorizon :: transferData ()
{
// It moves a Particle from the Universal EventHorizon to the ghostPort.

	Pointer* p;

	// check if data in.
	if (dataNew == FALSE) return;

	// Back up in the buffer by numberTokens
	for(int i = numberTokens; i>0; i--)
		p = myBuffer->last();

	// now, transfer the data.
	for(i = numberTokens; i>0; i--) {
	
		// get the buffer pointer
		p = myBuffer->next();

		// Get Particle from Plasma
		Particle* pp = myPlasma->get();

		// Copy from the buffer to this Particle
		// since we need previous values sometimes.
		*pp = *(Particle *)*p;
	
		// ghostPort :: current pointer
		Pointer* q = ghostPort->myBuffer->next();

		// put the current particle to Plasma
		ghostPort->myPlasma->put((Particle*)*q);

		// Get a new Particle (event) from the Geodesic
		*q = pp;
	}

	// set DataNew Value to ghostPort
	ghostPort->dataNew = TRUE;
	dataNew = FALSE;

	// call ghostPort->sendData() for further conversion if it is input.
	if (isItInput())
		ghostPort->sendData();
}

void ToEventHorizon :: initialize()
{
	// call initialization routine for itselt
	PortHole :: initialize();

	// Initialize members
	timeStamp = 0.0;
	dataNew = FALSE;

	// if on the boundary, call ghostPort :: initialize()
	if (isItInput())
		ghostPort->initialize();
}

/**************************************************************************

	methods for FromEventHorizon

**************************************************************************/

void FromEventHorizon :: transferData ()
{
	// call ghostPort->grabData for initial conversion if it is output.
	if (isItOutput())
		ghostPort->grabData();
}

void FromEventHorizon :: initialize()
{
	// call initialization routine for itselt
	PortHole :: initialize();

	// Initialize members
	timeStamp = 0.0;
	dataNew = FALSE;

	// if on the boundary, call ghostPort :: initialize()
	if (isItOutput())
		ghostPort->initialize();
}

