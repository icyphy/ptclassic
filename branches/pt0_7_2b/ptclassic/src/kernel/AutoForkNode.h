#ifndef _AutoForkNode_h
#define _AutoForkNode_h 1
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
#include "Geodesic.h"

class MultiPortHole;
class Block;

class AutoForkNode : public Geodesic {
public:
	int isItPersistent ();
	PortHole* setSourcePort (GenericPort &);
	PortHole* setDestPort (GenericPort &);
	AutoForkNode () : forkStar(0), forkOutput(0) {}
	~AutoForkNode();
private:
	// pointers to the Fork star and its output multiporthole, if it exists
	Block* forkStar;
	MultiPortHole* forkOutput;
	// cruft used for generating names for autoforks
	static int nF;
	const char* autoForkName();
};
#endif
