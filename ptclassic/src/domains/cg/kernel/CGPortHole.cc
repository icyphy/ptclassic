static const char file_id[] = "CGConnect.cc";
#include "Block.h"
#include "CGConnect.h"
#include "CircularBuffer.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

Code for CG-type portholes and multiportholes.

**************************************************************************/

int InCGPort :: isItInput () const { return TRUE;}

// void InCGPort :: grabData () { getParticle();}

int OutCGPort :: isItOutput () const { return TRUE;}

// void OutCGPort :: grabData () { clearParticle();}

// void OutCGPort :: sendData () { putParticle();}

int MultiInCGPort :: isItInput () const { return TRUE;}
int MultiOutCGPort :: isItOutput () const { return TRUE;}

PortHole& MultiInCGPort :: newPort () {
	LOG_NEW; PortHole& p = *new InCGPort;
	p.numberTokens = numberTokens;
	return installPort(p);
}
 
PortHole& MultiOutCGPort :: newPort () {
	LOG_NEW; PortHole& p = *new OutCGPort;
	p.numberTokens = numberTokens;
	return installPort(p);
}
