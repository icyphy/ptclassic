static const char file_id[] = "CGConnect.cc";
#include "Block.h"
#include "CGConnect.h"
#include "CircularBuffer.h"
#include "CGGeodesic.h"

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

int CGPortHole::bufSize() const {
        return cgGeo().bufSize();
}

int CGPortHole::localBufSize() const {
	return cgGeo().localBufSize();
}

// initialize the offset member.  If there is no fork involved, input
// portholes start reading from offset 0, and output portholes start
// writing just after any delay tokens.
// If there are forks, outputs are done the same way, and input pointers
// are backed up to handle the "forkDelay".
int CGPortHole::initOffset() {
	int bsiz = bufSize();
	if (isItOutput()) {
		offset = numTokens();
		// the following error is already reported by bufSize()
	        if (offset > bsiz) return FALSE;
		if (offset == bsiz) offset = 0;
	}
	else {
		offset = -cgGeo().forkDelay();
		if (offset < 0) offset += bsiz;
		// if still < 0, forkDelay is larger than we can handle
		if (offset < 0) {
			Error::abortRun(*this,
				" sorry, forkDelay is larger\n",
				"than the current implementation can handle");
			return FALSE;
		}
	}
	return TRUE;
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
