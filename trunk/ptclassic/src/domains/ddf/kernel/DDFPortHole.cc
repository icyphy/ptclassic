static const char file_id[] = "DDFConnect.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "Block.h"
#include "DDFConnect.h"
#include "Particle.h"
#include "Plasma.h"
#include "CircularBuffer.h"
#include "Geodesic.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/27/90

Code for DDF-type portholes and multiportholes.

**************************************************************************/

int InDDFPort :: isItInput () const { return TRUE;}

void InDDFPort :: grabData () { getParticle();}

int OutDDFPort :: isItOutput () const { return TRUE;}

void OutDDFPort :: grabData () { clearParticle();}

void OutDDFPort :: sendData () { putParticle();}

int MultiInDDFPort :: isItInput () const { return TRUE;}
int MultiOutDDFPort :: isItOutput () const { return TRUE;}

PortHole& DDFPortHole :: setPort (
			     const char* s,
                             Block* parent,
                             DataType t,
                             unsigned numTokens,
			     unsigned delay)
{
	// Initialize PortHole
        PortHole::setPort(s,parent,t);

	// Based on the delay, we allocate a Buffer assuming no
	// past Particles are allocated
	if (numTokens > 0) {
		varying = FALSE;
		numberTokens = numTokens;
	} else {
		varying = TRUE;
		numberTokens = 1;
	}

	// The number of Particles the buffer has to hold is:
	//		numberTokens current and future Particles
	//		delay past Particles
	bufferSize = numberTokens + delay;

        return *this;
}

void DDFPortHole :: imageConnect()
{
	// get real partner
	PortHole& realPort = imagePort->newConnection();

	// connect realPort to itself to set farSidePort member.
	realPort.connect(realPort, 0);
	imageGeo = realPort.myGeodesic;
}

void DDFPortHole :: setDDFParams(unsigned numTokens)
{
	numberTokens = numTokens;
	bufferSize = numberTokens;
	if (myBuffer && myBuffer->size() != bufferSize)
		initialize();
}

void InDDFPort :: moveData() 
{
	Particle** p;

	// Back up in the buffer by numberTokens
	for (int i = numberTokens; i > 0; i--)
		p = myBuffer->last();

	// sending copies to imageGeo
	for (i = numberTokens; i > 0; i--) {
		p = myBuffer->next();
		Particle* pp = myPlasma->get();
		*pp = **p;
		imageGeo->put(pp);
	}
}
	
void OutDDFPort :: moveData() 
{
	// get data from imageGeo
	for (int i = numberTokens; i > 0; i--) {
		Particle** p = myBuffer->next();
		myPlasma->put(*p);
		*p = imageGeo->get();
	}
}

MultiPortHole& MultiDDFPort :: setPort (const char* s,
                             Block* parent,
                             DataType t,
                             unsigned numTokens) {
        MultiPortHole::setPort(s,parent,t);
	numberTokens = numTokens;
        return *this;
}

// common code for making a new DDFPortHole in a DDFMultiPortHole.
// called by MultiInDDFPort::newPort and MultiOutDDFPort::newPort.
PortHole& MultiDDFPort :: finishNewPort(DDFPortHole& p) {
	if (numberTokens == 0) {
		p.numberTokens = 1;
		p.varying = TRUE;
	}
	else {
		p.numberTokens = numberTokens;
		p.varying = FALSE;
	}
	return installPort(p);
}
 
PortHole& MultiInDDFPort :: newPort () {
	LOG_NEW; DDFPortHole& p = *new InDDFPort;
	return finishNewPort(p);
}

PortHole& MultiOutDDFPort :: newPort () {
	LOG_NEW; DDFPortHole& p = *new OutDDFPort;
	return finishNewPort(p);
}


