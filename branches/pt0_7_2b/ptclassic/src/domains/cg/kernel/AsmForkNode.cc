static const char file_id[] = "AsmForkNode.cc";
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
#ifdef __GNUG__
#pragma implementation
#endif

#include "AsmForkNode.h"

// this type is persistent
int AsmForkNode::isItPersistent () const {
	return TRUE;
}

// make a new source connection
PortHole* AsmForkNode::setSourcePort (GenericPort &sp, int delay) {
	return af.setSource(sp, delay);
}

// make a new destination connection.  We always make a fork star.
PortHole* AsmForkNode::setDestPort (GenericPort &gp) {
	return af.setDest(gp, 1);
}

// class id
ISA_FUNC(AsmForkNode,AsmGeodesic);
