#include "Block.h"
#include "DDFConnect.h"
#include "Particle.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/27/90

Code for DDF-type portholes and multiportholes.

**************************************************************************/

int InDDFPort :: isItInput () { return TRUE;}

void InDDFPort :: grabData () { getParticle();}

int OutDDFPort :: isItOutput () { return TRUE;}

void OutDDFPort :: grabData () { clearParticle();}

void OutDDFPort :: sendData () { putParticle();}

int MultiInDDFPort :: isItInput () { return TRUE;}
int MultiOutDDFPort :: isItOutput () { return TRUE;}

PortHole& DDFPortHole :: setPort (
			     const char* s,
                             Block* parent,
                             dataType t = FLOAT,
                             unsigned numTokens = 1,
			     unsigned delay = 0)
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

void DDFPortHole :: setDDFParams(unsigned numTokens = 1)
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
                             dataType t = FLOAT,
                             unsigned numTokens = 1) {
        MultiPortHole::setPort(s,parent,t);
	numberTokens = numTokens;
        return *this;
}

PortHole& MultiInDDFPort :: newPort () {
	PortHole& p = *new InDDFPort;
	p.numberTokens = numberTokens;
	return installPort(p);
}
 
 
PortHole& MultiOutDDFPort :: newPort () {
	PortHole& p = *new OutDDFPort;
	p.numberTokens = numberTokens;
	return installPort(p);
}

