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

// we require circular access either if the PB_CIRC attribute is set
// (indicating a user request for it), or if the number of tokens
// read each time doesn't evenly divide the buffer size.

int AsmPortHole::circAccess() const {
	if (attributes() | PB_CIRC) return TRUE;
	return (bufferSize % numberTokens != 0);
}

// return a string indicating the address.  This is virtual so it
// could be handled differently by derived classes.
StringList AsmPortHole::location(int update) {
	int n = baseAddr() + offset;
	if (update) advance();
	return n;
}

Geodesic* AsmPortHole::allocateGeodesic() {
	char* nm = new char[strlen(readName())+6];
	strcpy (nm, "Node_");
	strcat (nm, readName());
	Geodesic *g = new AsmGeodesic;
	g->setNameParent(nm, parent());
	return g;
}

int AsmPortHole::bufSize() const {
	return geo().bufSize();
}

void AsmPortHole::assignAddr(ProcMemory& m, unsigned a) {
	geo().assignAddr(m,a);
}

unsigned AsmPortHole::baseAddr() const {
	return geo().addr();
}

