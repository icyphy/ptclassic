static const char file_id[] = "EventHorizon.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "EventHorizon.h"
#include "Star.h"
#include "StringList.h"
#include "Error.h"
#include "Wormhole.h"
#include "Particle.h"
#include "Plasma.h"
#include "CircularBuffer.h"
#include "Domain.h"
 
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/31/90
 Date of Revision: 9/16/92
 Revisions:
	make EventHorizon Universal.
        each doamin needs to define In??EventHorizon and Out??EventHorizon
        derived from Universal EventHorizon and domain-specific PortHoles.

Code for functions declared in EventHorizon.h

**************************************************************************/

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
 
// destructor
EventHorizon :: ~EventHorizon() {
	if (ghostPort) { 
		ghostPort->ghostPort = 0;
		LOG_DEL; delete ghostPort;
		ghostPort = 0;
	}
}

void EventHorizon :: setEventHorizon (
			     inOutType inOut,
			     const char* s,
                             Wormhole* parentWormhole,
                             Star* parentStar,
                             DataType t,
			     unsigned numTokens)
{
	// Initialize PortHole
        asPort()->setPort(s, parentStar,t);

	// Initialize the EventHorizon members
	timeMark = 0.0;
	tokenNew = FALSE;
	asPort()->numberTokens = numTokens;
	asPort()->setMaxDelay(0);

	// set which wormhole it is in
	wormhole = parentWormhole;

        // in or out?
        inOrOut = inOut;
}

int EventHorizon :: isItInput() const { return (inOrOut < 2); }
int EventHorizon :: isItOutput() const { return (inOrOut > 1); }

void EventHorizon :: initialize() {}

// This method moves num particles from the EventHorizon "from"
// to me.

void EventHorizon::moveFromGhost(EventHorizon& from, int num) {
	CircularBuffer* toBuffer = buffer();
	CircularBuffer* fromBuffer = from.buffer();
	toBuffer->backup(num);
	for (int i = num; i > 0; i--) {
		Particle** p = toBuffer->next();
		Particle** q = fromBuffer->next();

		// In the following code, particles aren't copied, only
		// pointers to Particles.
		// get a reference to the old particle from the fromBuffer.
		Particle* tmp = *q;
		// Transfer the particle from one buffer to the other
		*q = *p;

		// zero the old particle and put it in the source buffer.
		tmp->initialize();
		*p = tmp;
	}
}


/**************************************************************************

	methods for ToEventHorizon

**************************************************************************/

void ToEventHorizon :: getData()
{
	// check whether data exists or not
	if (asPort()->numTokens() >= asPort()->numXfer()) {
		asPort()->getParticle();
		tokenNew = TRUE;
	} else {
		tokenNew = FALSE;
	}
}

void ToEventHorizon :: transferData ()
{
// It moves a Particle from the Universal EventHorizon to the ghostPort.

	// check if data in.
	if (tokenNew == FALSE) return;

	// move data.
	moveFromGhost(*ghostPort, asPort()->numberTokens);

	// set DataNew Value to ghostPort
	ghostPort->tokenNew = TRUE;
	tokenNew = FALSE;

	// call ghostPort->sendData() for further conversion if it is input.
	if (isItInput())
		ghostPort->asPort()->sendData();
}

void ToEventHorizon :: initialize()
{
	// Initialize members
	timeMark = 0.0;
	tokenNew = FALSE;

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
		ghostPort->asPort()->receiveData();
}

void FromEventHorizon :: initialize()
{
	// Initialize members
	setTimeMark(0.0);
	tokenNew = FALSE;

	// if on the boundary, call ghostPort :: initialize()
	if (isItOutput())
		ghostPort->initialize();
}

int FromEventHorizon :: ready() { return TRUE ;}

/**************************************************************************

	methods for WormMultiPort

**************************************************************************/

int WormMultiPort :: isItInput() const { return alias()->isItInput(); }
int WormMultiPort :: isItOutput() const { return alias()->isItOutput(); }

extern const char* ghostName(const GenericPort&);

PortHole& WormMultiPort :: newPort() {

	// identify domain
	Domain* inSideDomain = Domain::named(worm->insideDomain());
	Domain* outSideDomain = Domain::named(parent()->domain());

	// make real porthole
	GalMultiPort* galp = (GalMultiPort*) alias();
	PortHole& realP = galp->newConnection();

	// build eventHorizon
        DataType type = realP.type();
        int numToken = realP.numXfer();
// separate rules for connecting inputs and outputs.
        if (galp->isItInput()) {
                EventHorizon& to = outSideDomain->newTo();
                EventHorizon& from = inSideDomain->newFrom();
                to.setEventHorizon(in, galp->name(), worm, 
			(Star*) parent(), type, numToken);
                parent()->addPort(*(to.asPort()));
		ports.put(*(to.asPort()));
                from.setEventHorizon(in, ghostName(*galp), worm, 
			(Star*) parent(), type, numToken);
                to.ghostConnect (from);
                from.asPort()->inheritTypeFrom (realP);
                to.asPort()->inheritTypeFrom (*(from.asPort()));
                from.asPort()->connect(realP,0);
		return *(to.asPort());
        } else {
                EventHorizon& to = inSideDomain->newTo();
                EventHorizon& from = outSideDomain->newFrom();
                from.setEventHorizon(out, galp->name(), worm, 
			(Star*) parent(), type, numToken);
                parent()->addPort(*(from.asPort()));
		ports.put(*(from.asPort()));
                to.setEventHorizon(out, ghostName(*galp), worm, 
			(Star*) parent(), type, numToken);
                to.ghostConnect (from);
                to.asPort()->inheritTypeFrom (realP);
                from.asPort()->inheritTypeFrom (*(to.asPort()));
                realP.connect(*(to.asPort()),0);
		return *(from.asPort());
        }
}

