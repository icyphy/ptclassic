static const char file_id[] = "AutoForkNode.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 11/7/90

 This glass is a Geodesic that automatically generates auto-forks
 when attempts are made to connect multiple outputs to it.  It forbids
 the specification of multiple inputs.
*************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "AutoForkNode.h"

// this type is persistent
int AutoForkNode::isItPersistent () const {
	return TRUE;
}

// make a new source connection
PortHole* AutoForkNode::setSourcePort (GenericPort &sp, int delay) {
	return af.setSource(sp, delay);
}

// make a new destination connection, possibly autoforking
PortHole* AutoForkNode::setDestPort (GenericPort &gp) {
	return af.setDest(gp);
}

// class id
ISA_FUNC(AutoForkNode,Geodesic);
