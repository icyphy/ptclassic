static const char file_id[] = "CGPortHole.cc";
#include "Block.h"
#include "CGPortHole.h"
#include "CircularBuffer.h"
#include "CGGeodesic.h"
#include "SDFStar.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck, E. A. Lee

Code for CG-type portholes and multiportholes.

**************************************************************************/

// constructor
CGPortHole :: CGPortHole() : offset(0), forkSrc(0), embeddedPort(0) {}

// destructor: remove forklist references.
CGPortHole :: ~CGPortHole() {
	ListIter next(forkDests);
	OutCGPort* p;
	while ((p = (OutCGPort*)next++) != 0) p->setForkSource(0);
	if (forkSrc)
		forkSrc->forkDests.remove(this);
}

// Advance the offset by the number of tokens produced or
// consumed in this PortHole when the Star fires.
void CGPortHole::advance() {
	offset += numberTokens;
	int sz = bufSize();
	if (offset >= sz) offset -= sz;
}

// allocate a CGGeodesic.  Use hashstring for the name since we expect
// repeated names to occur (lots of Node_input and Node_output, for example)
Geodesic* CGPortHole::allocateGeodesic() {
        char nm[80];
        strcpy (nm, "Node_");
        strcat (nm, name());
        LOG_NEW; Geodesic *g = new CGGeodesic;
        g->setNameParent(hashstring(nm), parent());
        return g;
}

// make me a fork destination; set my source.
void CGPortHole::setForkSource(CGPortHole * p) {
	forkSrc = p;
	if (!p) return;
	// add me as one of forkSrc's destinations
	forkSrc->forkDests.put(this);
}


// processing for each port added to a fork buffer
// call this from OutXXX
void MultiCGPort :: forkProcessing (CGPortHole& p) {
	if (forkSrc) {
		p.setForkSource(forkSrc);
	}
}

// this avoids having cfront generate many copies of the destructor
MultiCGPort :: ~MultiCGPort() {}

// ug-lee!!!!
static int parentReps(const PortHole* p) {
	SDFStar* s = (SDFStar*)(p->parent());
	return s->reps();
}

int CGPortHole :: bufSize() const {
	if (atBoundary())
		return parentReps(this)*numberTokens;
	else return cgGeo().bufSize();
}

int CGPortHole :: localBufSize() const {
	if (atBoundary())
		return parentReps(this)*numberTokens;
	else return cgGeo().localBufSize();
}

int InCGPort :: isItInput () const { return TRUE;}
int OutCGPort :: isItOutput () const { return TRUE;}
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
