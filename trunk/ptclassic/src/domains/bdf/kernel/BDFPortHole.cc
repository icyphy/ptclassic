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

// table used by reverse(BDFRelation) function.
BDFRelation
BDFPortHole::reversals[4] = { BDF_TRUE,BDF_FALSE,BDF_COMPLEMENT,BDF_SAME};

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

// setRelation sets up relationship arcs.  BDF_SAME and BDF_COMPLEMENT
// arcs always form a circle.
void BDFPortHole::setRelation(BDFRelation rel, BDFPortHole* assocBool) {
	if (rel == relation && pAssocPort == assocBool)
		return;
	if (SorC(relation)) removeRelation();
	relation = rel;
	pAssocPort = assocBool;
	if (!SorC(rel)) return;
	// if my peer is not part of a loop yet, create a two-port loop
	if (!SorC(assocBool->relation)) {
		assocBool->relation = relation;
		assocBool->pAssocPort = this;
		return;
	}
	// it is already part of a loop.  Insert myself between
	// assoc and assoc->pAssocPort.
	pAssocPort = assocBool->pAssocPort;
	relation = (assocBool->relation == rel) ? BDF_SAME : BDF_COMPLEMENT;
	assocBool->pAssocPort = this;
	assocBool->relation = rel;
}

BDFPortHole::~BDFPortHole() { removeRelation();}

// change my relation to BDF_NONE, and delete me from any relationship
// loop.
void BDFPortHole::removeRelation() {
	if (pAssocPort && SorC(relation)) {
		if (pAssocPort->pAssocPort == this) {
			pAssocPort->pAssocPort = 0;
			pAssocPort->relation = BDF_NONE;
		}
		else {
			BDFPortHole* p = pAssocPort;
			// go around until we get to the one that points
			// to me
			while (p->pAssocPort != this) p = p->pAssocPort;
			// make it point to the next in line instead.
			p->pAssocPort = pAssocPort;
			if (relation == BDF_COMPLEMENT)
				p->relation = reverse(p->relation);
		}
	}
	relation = BDF_NONE;
	pAssocPort = 0;
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

