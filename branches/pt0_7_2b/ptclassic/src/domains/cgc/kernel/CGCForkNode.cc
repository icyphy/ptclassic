static const char file_id[] = "CGCForkNode.cc";
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
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCForkNode.h"

// this type is persistent
int CGCForkNode::isItPersistent () const {
	return TRUE;
}

// make a new source connection
PortHole* CGCForkNode::setSourcePort (GenericPort &sp, int delay) {
	return af.setSource(sp, delay);
}

// make a new destination connection.  We always make a fork star.
PortHole* CGCForkNode::setDestPort (GenericPort &gp) {
	return af.setDest(gp, 1);
}

// class id
ISA_FUNC(CGCForkNode,CGCGeodesic);
