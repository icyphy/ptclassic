#ifndef _AsmForkNode_h
#define _AsmForkNode_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 2/13/92

 This glass is an auto-forking version of AsmGeodesic.  It is to
 AsmGeodesic as AutoForkNode is to Geodesic.  A template could be
 used to generate both.

*************************************************************************/
#include "AsmGeodesic.h"
#include "AutoFork.h"

class AsmForkNode : public AsmGeodesic {
public:
	// constructor: make the AsmFork object refer to me.
	AsmForkNode() : af(*this) {}
	int isItPersistent () const;
	PortHole* setSourcePort (GenericPort &, int delay = 0);
	PortHole* setDestPort (GenericPort &);

	// class identification
	int isA(const char*) const;

private:
	AutoFork af;
};
#endif
