static const char file_id[] = "WormConnect.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "WormConnect.h"
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
 Date of Revision: 6/13/90
 Revisions:
	make EventHorizon Universal.
        each doamin needs to define In??EventHorizon and Out??EventHorizon
        derived from Universal EventHorizon with domain-specific
  	translation.

Code for functions declared in WormConnect.h

**************************************************************************/

/**************************************************************************

	methods for EventHorizon

**************************************************************************/

inline Particle** EventHorizon :: nextInBuffer() { return myBuffer->next(); }

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
                             DataType t,
			     unsigned numTokens)
{
	// Initialize PortHole
        PortHole::setPort(s, parentStar,t);

	// Initialize the EventHorizon members
	timeStamp = 0.0;
	dataNew = FALSE;
	numberTokens = numTokens;
	bufferSize = numberTokens;

	// set which wormhole it is in
	wormhole = parentWormhole;

	// in or out?
	inOrOut = inOut;

        return *this;
}

int EventHorizon :: isItInput() const {
	if (inOrOut < 2) return TRUE;
	else		 return FALSE;
}

int EventHorizon :: isItOutput() const {
	if (inOrOut > 1) return TRUE;
	else		 return FALSE;
}

/**************************************************************************

	methods for ToEventHorizon

**************************************************************************/

void ToEventHorizon :: getData()
{
	// check whether data exists or not
	if (numTokens() >= numberTokens) {
		getParticle();
		dataNew = TRUE;
	} else {
		dataNew = FALSE;
	}
}

void ToEventHorizon :: transferData ()
{
// It moves a Particle from the Universal EventHorizon to the ghostPort.

	// check if data in.
	if (dataNew == FALSE) return;

	// Back up in the buffer by numberTokens
	myBuffer->backup(numberTokens);

	// now, transfer the data.
	for(int i = numberTokens; i>0; i--) {

		// get pointers to each of the CircularBuffer objects.
		Particle** p = myBuffer->next();
		Particle** q = ghostPort->nextInBuffer();

		// In the following code, particles aren't copied, only
		// pointers to Particles.
		// get a reference to the old particle from the ghostport.
		Particle* tmp = *q;

		// Transfer the particle from one buffer to the other
		*q = *p;

		// zero the old particle and put it in the source buffer.
		tmp->initialize();
		*p = tmp;
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
	// call initialization routine for myself
	PortHole :: initialize();

	// Initialize members
	timeStamp = 0.0;
	dataNew = FALSE;

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
        DataType type = realP.myType();
        int numToken = realP.numXfer();
// separate rules for connecting inputs and outputs.
        if (galp->isItInput()) {
                EventHorizon& to = outSideDomain->newTo();
                EventHorizon& from = inSideDomain->newFrom();
                to.setPort(in, galp->readName(), worm, (Star*) parent(),
                	type, numToken);
                parent()->addPort(to);
		ports.put(to);
                from.setPort(in, ghostName(*galp), worm, (Star*) parent(),
                	type, numToken);
                to.ghostConnect (from);
                from.inheritTypeFrom (realP);
                to.inheritTypeFrom (from);
                from.connect(realP,0);
		return to;
        } else {
                EventHorizon& to = inSideDomain->newTo();
                EventHorizon& from = outSideDomain->newFrom();
                from.setPort(out, galp->readName(), worm, (Star*) parent(),
                                     type, numToken);
                parent()->addPort(from);
		ports.put(from);
                to.setPort(out, ghostName(*galp), worm, (Star*) parent(),
                                   type, numToken);
                to.ghostConnect (from);
                to.inheritTypeFrom (realP);
                from.inheritTypeFrom (to);
                realP.connect(to,0);
		return from;
        }
}

