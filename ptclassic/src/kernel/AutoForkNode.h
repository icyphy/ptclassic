#ifndef _AutoForkNode_h
#define _AutoForkNode_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif
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

 Change: the auto-forking is moved into a separate class called AutoFork,
 so we can make other types of auto-forking geodesics with greater ease.
*************************************************************************/
#include "Geodesic.h"
#include "AutoFork.h"

class AutoForkNode : public Geodesic {
public:
	// constructor: make the AutoFork object refer to me.
	AutoForkNode() : af(*this) {}
	int isItPersistent () const;
	PortHole* setSourcePort (GenericPort &, int delay = 0);
	PortHole* setDestPort (GenericPort &);

	// class identification
	int isA(const char*) const;

private:
	AutoFork af;
};
#endif
