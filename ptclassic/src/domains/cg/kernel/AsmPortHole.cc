static const char file_id[] = "AsmConnect.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif
#include "AsmConnect.h"
#include "AsmGeodesic.h"
#include "Error.h"
#include "miscFuncs.h"

// attributes
extern const Attribute P_CIRC(PB_CIRC,0);
extern const Attribute P_SYMMETRIC(PB_SYMMETRIC,0);

// we require circular access either if the PB_CIRC attribute is set
// (indicating a user request for it), or if the number of tokens
// read or written each time doesn't evenly divide the buffer size.

int AsmPortHole::circAccess() const {
	int bsiz = bufSize();
	if ((attributes() & PB_CIRC) != 0) return TRUE;
	if (numberTokens > 1 && bsiz % numberTokens != 0) return TRUE;
	int farTokens = far()->numTokens();
	if (farTokens > 1 && bsiz % farTokens != 0) return TRUE;
	return FALSE;
}

// return a string indicating the address.  This is virtual so it
// could be handled differently by derived classes.
StringList AsmPortHole::location() {
	int n = baseAddr() + offset;
	return n;
}

// allocate an AsmGeodesic.  Use hashstring for the name since we expect
// repeated names to occur (lots of Node_input and Node_output, for example)

Geodesic* AsmPortHole::allocateGeodesic() {
	char nm[80];
	strcpy (nm, "Node_");
	strcat (nm, readName());
	LOG_NEW; Geodesic *g = new AsmGeodesic;
	g->setNameParent(hashstring(nm), parent());
	return g;
}

// initialize the offset member.  If there is no fork involved, input
// portholes start reading from offset 0, and output portholes start
// writing just after any delay tokens.

// if there are forks, outputs are done the same way, and input pointers
// are backed up to handle the "forkDelay".
int AsmPortHole::initOffset() {
	int bsiz = bufSize();
	if (isItOutput()) {
		offset = numTokens();
		// the following error is already reported by bufSize()
	        if (offset > bsiz) return FALSE;
		if (offset == bsiz) offset = 0;
	}
	else {
		offset = -geo().forkDelay();
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

int AsmPortHole::bufSize() const {
	return geo().bufSize();
}

int AsmPortHole::localBufSize() const {
	return geo().localBufSize();
}

void AsmPortHole::assignAddr(ProcMemory& m, unsigned a) {
	geo().assignAddr(m,a);
}

unsigned AsmPortHole::baseAddr() const {
	return geo().address();
}

ProcMemory* AsmPortHole::memory() const {
	return geo().memory();
}

// processing for each port added to a fork buffer
// call this from OutXXX
void MultiAsmPort :: forkProcessing (AsmPortHole& p) {
	if (forkSrc) {
		p.setForkSource(forkSrc);
	}
}

// destructor: remove forklist references.
AsmPortHole :: ~AsmPortHole() {
	ListIter next(forkDests);
	OutAsmPort* p;
	while ((p = (OutAsmPort*)next++) != 0) p->setForkSource(0);
	if (forkSrc)
		forkSrc->forkDests.remove(this);
}

// make me a fork destination; set my source.
void AsmPortHole::setForkSource(AsmPortHole * p) {
	forkSrc = p;
	if (!p) return;
	// add me as one of forkSrc's destinations
	forkSrc->forkDests.put(this);
}

int InAsmPort :: isItInput() const {return TRUE; }
int OutAsmPort :: isItOutput() const {return TRUE; }
int MultiInAsmPort :: isItInput() const {return TRUE; }
int MultiOutAsmPort :: isItOutput() const {return TRUE; }

