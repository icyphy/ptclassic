static const char file_id[] = "CGConnect.cc";
#include "Block.h"
#include "CGConnect.h"
#include "CircularBuffer.h"
#include "CGGeodesic.h"
#include "SDFStar.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

Code for CG-type portholes and multiportholes.

**************************************************************************/

// destructor: remove forklist references.
CGPortHole :: ~CGPortHole() {
	ListIter next(forkDests);
	OutCGPort* p;
	while ((p = (OutCGPort*)next++) != 0) p->setForkSource(0);
	if (forkSrc)
		forkSrc->forkDests.remove(this);
}
// allocate a CGGeodesic.  Use hashstring for the name since we expect
// repeated names to occur (lots of Node_input and Node_output, for example)
Geodesic* CGPortHole::allocateGeodesic() {
        char nm[80];
        strcpy (nm, "Node_");
        strcat (nm, readName());
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

// ug-lee!!!!
static int parentReps(const PortHole* p) {
	SDFStar* s = (SDFStar*)(p->parent());
	return int(s->repetitions.numerator);
}

int CGPortHole :: bufSize() const {
	if (isItInput() == far()->isItInput())
		return parentReps(this)*numberTokens;
	else return cgGeo().bufSize();
}

int CGPortHole :: localBufSize() const {
	if (isItInput() == far()->isItInput())
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
