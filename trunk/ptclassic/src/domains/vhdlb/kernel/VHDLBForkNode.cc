static const char file_id[] = "VHDLBForkNode.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Edward A. Lee

 This class is an auto-forking version of VHDLBGeodesic.  It is to
 VHDLBGeodesic as AutoForkNode is to Geodesic.  A template could be
 used to generate both.

*************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLBForkNode.h"

// this type is persistent
int VHDLBForkNode::isItPersistent () const {
	return TRUE;
}

// make a new source connection
PortHole* VHDLBForkNode::setSourcePort (GenericPort &sp, int delay) {
	return af.setSource(sp, delay);
}

// make a new destination connection.  We always make a fork star.
PortHole* VHDLBForkNode::setDestPort (GenericPort &gp) {
	return af.setDest(gp, 1);
}

// class id
ISA_FUNC(VHDLBForkNode,VHDLBGeodesic);
