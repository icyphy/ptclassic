static const char file_id[] = "BDFConnect.cc";
#include "Block.h"
#include "BDFConnect.h"
#include "CircularBuffer.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck

Code for BDF-type portholes and multiportholes.

**************************************************************************/

int InBDFPort :: isItInput () const { return TRUE;}

// note: grabData and sendData shouldn't be called if, based on
// associated booleans, an arc is not to consume or produce tokens.
void InBDFPort :: grabData () {
	getParticle();
}

int OutBDFPort :: isItOutput () const { return TRUE;}

void OutBDFPort :: grabData () {
	clearParticle();
}

void OutBDFPort :: sendData () {
	putParticle();
}

int MultiInBDFPort :: isItInput () const { return TRUE;}
int MultiOutBDFPort :: isItOutput () const { return TRUE;}

PortHole& BDFPortHole :: setPort(const char* portName, Block* parent,
		       DataType type, unsigned numTokens,
		       PortHole* assocBool, BDFRelation relation, int delay) {
	PortHole::setPort(portName,parent,type);
	return setBDFParams(numTokens,assocBool,relation,delay);
}

// Function to alter only BDF parameters
// We re-do porthole initialization if bufferSize changes
PortHole& BDFPortHole :: setBDFParams(unsigned num,PortHole* assoc,
				      BDFRelation sense, int delay) {
	numberTokens = num;
	bufferSize = numberTokens + delay;
	relation = sense;
	pAssocPort = assoc;
	if (myBuffer && myBuffer->size() != bufferSize)
		initialize();
	return *this;
}

PortHole& MultiInBDFPort :: newPort () {
	BDFPortHole& p = *new InBDFPort;
	p.setBDFParams(numberTokens,assocBoolean,relation,delay);
	return installPort(p);
}


PortHole& MultiOutBDFPort :: newPort () {
	BDFPortHole& p = *new OutBDFPort;
	p.setBDFParams(numberTokens,assocBoolean,relation,delay);
	return installPort(p);
}

