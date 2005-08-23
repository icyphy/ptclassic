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
#include "Scheduler.h"
#include "Geodesic.h"
 
/**************************************************************************
Version identification:
@(#)EventHorizon.cc	2.28	01/27/97

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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

// predicate saying whether the eventhorizon is retricted to one token
// at a time.  This is the default implementation.
int EventHorizon :: onlyOne() const { return FALSE; }

void EventHorizon::setEventHorizon (inOutType inOut, Wormhole* parentWormhole,
			     	unsigned numTokens) {
	// Initialize the EventHorizon members
	timeMark = 0.0;
	tokenNew = FALSE;
	if (numTokens > 1 && onlyOne())
		Error::abortRun(*asPort(),"cannot support numTokens > 1");
	else {
		asPort()->numberTokens = numTokens;
		asPort()->bufferSize = numTokens;
	}
	// set which wormhole it is in
	wormhole = parentWormhole;

        // in or out?
        inOrOut = inOut;
}

// This method should be called from the inside EH of the wormhole.
void EventHorizon :: setParams(int numTok) {
	// no effect if either end can transfer only one token and we
	// ask for more.
	if (numTok > 1) {
		if (onlyOne()) return;
		if (ghostPort && ghostPort->onlyOne()) return;
	}
	asPort()->numberTokens = numTok;
	asPort()->bufferSize = numTok;
	ghostAsPort()->numberTokens = numTok;
	ghostAsPort()->bufferSize = numTok;

	// note that ghost port is the visible EH of the wormhole.
	ghostAsPort()->initialize();
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

// return the scheduler of the inner domain.
Scheduler* EventHorizon :: innerSched() 
	{ return selfPort->parent()->scheduler(); }
// return the scheduler of the outer domain.
Scheduler* EventHorizon :: outerSched()
	{ return selfPort->parent()->parent()->scheduler(); }


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
		ghostAsPort()->sendData();
}

void ToEventHorizon :: initialize()
{
	// Initialize members
	timeMark = 0.0;
	tokenNew = FALSE;

	// if on the boundary, call ghostPort :: initialize()
	if (isItInput())
		ghostAsPort()->initialize();
}

// Allocate a geodesic appropriate for the inner or outer domain.
Geodesic* ToEventHorizon::allocateGeodesic()
{
    StringList nm;
    nm << "Node_" << asPort()->name();
    const char* dName = isItOutput() ? wormhole->insideDomain()
	: asPort()->parent()->parent()->domain();
    Domain* d = Domain::named(dName);
    Geodesic* g = NULL;
    if (d != NULL) g = &d->newGeo();
    if (g != NULL) g->setNameParent(hashstring(nm),asPort()->parent());
    return g;
}

/**************************************************************************

	methods for FromEventHorizon

**************************************************************************/

void FromEventHorizon :: transferData ()
{
	// call ghostPort->grabData for initial conversion if it is output.
	if (isItOutput())
		ghostAsPort()->receiveData();
}

void FromEventHorizon :: initialize()
{
	// Initialize members
	setTimeMark(0.0);
	tokenNew = FALSE;

	// if on the boundary, call ghostPort :: initialize()
	if (isItOutput())
		ghostAsPort()->initialize();
}

int FromEventHorizon :: ready() { return TRUE ;}

// Allocate a geodesic appropriate for the inner or outer domain.
Geodesic* FromEventHorizon::allocateGeodesic()
{
    StringList nm;
    nm << "Node_" << asPort()->name();
    const char* dName = isItInput() ? wormhole->insideDomain()
	: asPort()->parent()->domain();
    Domain* d = Domain::named(dName);
    Geodesic* g = NULL;
    if (d != NULL) g = &d->newGeo();
    if (g != NULL) g->setNameParent(hashstring(nm),asPort()->parent());
    return g;
}

/**************************************************************************

	methods for WormMultiPort

**************************************************************************/

int WormMultiPort :: isItInput() const { return myGMPH->isItInput(); }
int WormMultiPort :: isItOutput() const { return myGMPH->isItOutput(); }

extern const char* ghostName(const GenericPort&);

PortHole& WormMultiPort::newPort() {

	// identify domain
	Domain* inSideDomain = Domain::named(worm->insideDomain());
	Domain* outSideDomain = Domain::named(parent()->domain());

	// make real porthole
	GalMultiPort* galp = myGMPH;
	PortHole& realP = galp->newConnection();

	// build eventHorizon
        DataType type = realP.type();
        int numToken = realP.numXfer();
	
// separate rules for connecting inputs and outputs.
        if (galp->isItInput()) {
                EventHorizon& to = outSideDomain->newTo();
		PortHole& toPort = *to.asPort();

                to.setEventHorizon(in, worm, numToken);
		installPort(toPort);

                EventHorizon& from = inSideDomain->newFrom();
		PortHole& fromPort = *from.asPort();

                from.setEventHorizon(in, worm, numToken);
		fromPort.setPort(ghostName(toPort), parent(), type);
		
                to.ghostConnect(from);

                fromPort.connect(realP,0);
		return toPort;
        }
	else {
                EventHorizon& from = outSideDomain->newFrom();
		PortHole& fromPort = *from.asPort();
		
                from.setEventHorizon(out, worm, numToken);
		installPort(fromPort);
		
                EventHorizon& to = inSideDomain->newTo();
		PortHole& toPort = *to.asPort();
		
                to.setEventHorizon(out, worm, numToken);
		toPort.setPort(ghostName(fromPort), parent(), type);
		
                to.ghostConnect(from);
		
                realP.connect(toPort,0);
		return fromPort;
        }
}

PortHole& WormMultiPort :: newConnection() {
	// always make a new connection.
	return newPort();
}
