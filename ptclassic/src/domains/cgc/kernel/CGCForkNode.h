#ifndef _CGCForkNode_h
#define _CGCForkNode_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 This class is an auto-forking version of CGCGeodesic.  It is to
 CGCGeodesic as AutoForkNode is to Geodesic.  A template could be
 used to generate both.

*************************************************************************/
#include "CGCGeodesic.h"
#include "AutoFork.h"

class CGCForkNode : public CGCGeodesic {
public:
	// constructor: make the CGCFork object refer to me.
	CGCForkNode() : af(*this) {}
	int isItPersistent () const;
	PortHole* setSourcePort (GenericPort &, int delay = 0);
	PortHole* setDestPort (GenericPort &);

	// class identification
	int isA(const char*) const;

private:
	AutoFork af;
};
#endif
