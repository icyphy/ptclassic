#ifndef _VHDLBForkNode_h
#define _VHDLBForkNode_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 This class is an auto-forking version of VHDLBGeodesic.  It is to
 VHDLBGeodesic as AutoForkNode is to Geodesic.  A template could be
 used to generate both.

*************************************************************************/
#include "VHDLBGeodesic.h"
#include "AutoFork.h"

class VHDLBForkNode : public VHDLBGeodesic {
public:
	// constructor: make the VHDLBFork object refer to me.
	VHDLBForkNode() : af(*this) {}
	int isItPersistent () const;
	PortHole* setSourcePort (GenericPort &, int delay = 0);
	PortHole* setDestPort (GenericPort &);

	// class identification
	int isA(const char*) const;

private:
	AutoFork af;
};
#endif
