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
#include "miscFuncs.h"

// attributes
extern const Attribute P_CIRC(PB_CIRC,0);

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

// initialize.  Main job here is to set up the offset.
void AsmPortHole::initialize() {
	PortHole::initialize();
	if (isItOutput()) {
		offset = numTokens();
		while (offset > numberTokens) offset -= numberTokens;
	}
	else {
		offset = -geo().forkDelay();
		while (offset < 0) offset += numberTokens;
	}
}

int AsmPortHole::bufSize() const {
	return geo().bufSize();
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
		forkSrc->remEntry(this);
}

int InAsmPort :: isItInput() const {return TRUE; }
int OutAsmPort :: isItOutput() const {return TRUE; }
int MultiInAsmPort :: isItInput() const {return TRUE; }
int MultiOutAsmPort :: isItOutput() const {return TRUE; }

