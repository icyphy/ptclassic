static const char file_id[] = "BDFPortHole.cc";
#include "Block.h"
#include "BDFPortHole.h"
#include "CircularBuffer.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck

Code for BDF-type portholes and multiportholes.

**************************************************************************/

// table used by reverse(BDFRelation) function.
BDFRelation
BDFPortHole::reversals[4] = { BDF_TRUE,BDF_FALSE,BDF_COMPLEMENT,BDF_SAME};

int InBDFPort :: isItInput () const { return TRUE;}

// note: grabData and sendData shouldn't be called if, based on
// associated booleans, an arc is not to consume or produce tokens.
void InBDFPort :: receiveData () {
	getParticle();
}

int OutBDFPort :: isItOutput () const { return TRUE;}

void OutBDFPort :: receiveData () {
	clearParticle();
}

void OutBDFPort :: sendData () {
	putParticle();
}

int MultiInBDFPort :: isItInput () const { return TRUE;}
int MultiOutBDFPort :: isItOutput () const { return TRUE;}

PortHole& BDFPortHole :: setPort(const char* portName, Block* parent,
				 DataType type, unsigned numTokens,
				 BDFPortHole* assocBool, BDFRelation relation,
				 int delay) {
	PortHole::setPort(portName,parent,type);
	return setBDFParams(numTokens,assocBool,relation,delay);
}

// Function to alter only BDF parameters
// We re-do porthole initialization if bufferSize changes
PortHole& BDFPortHole :: setBDFParams(unsigned num,BDFPortHole* assoc,
				      BDFRelation sense, int delay) {
	numberTokens = num;
	bufferSize = numberTokens + delay;
	setRelation(sense,assoc);
	if (myBuffer && myBuffer->size() != bufferSize)
		initialize();
	return *this;
}

// return true if I am dynamic
int BDFPortHole :: isDynamic() const {
	return TorF(relType());
}

// mph functions.
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

