static const char file_id[] = "DDFPortHole.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "Block.h"
#include "DDFPortHole.h"
#include "Particle.h"
#include "Plasma.h"
#include "CircularBuffer.h"
#include "Geodesic.h"

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha
 Date of creation: 8/27/90

Code for DDF-type portholes and multiportholes.

**************************************************************************/

void DDFPortHole :: moveData() {}

int InDDFPort :: isItInput () const { return TRUE;}

void InDDFPort :: receiveData () { getParticle();}

int OutDDFPort :: isItOutput () const { return TRUE;}

void OutDDFPort :: receiveData () { clearParticle();}

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
	imageGeo = realPort.geo();
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
		(*p)->die();
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


