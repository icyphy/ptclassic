/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 2/13/92

 This used to be part of AutoForkNode.  It is now a separate class,
 to make it easier to define other types of auto-forking nodes
 (such as AsmForkNode for assembly language stars).

*************************************************************************/

#ifndef _AutoFork_h
#define _AutoFork_h 1

#ifdef __GNUG__
#pragma interface
#endif

class Geodesic;
class Block;
class MultiPortHole;
class GenericPort;
class PortHole;

class AutoFork {
private:
	Geodesic& geo;
	Block* forkStar;
	MultiPortHole* forkOutput;
	// cruft used for generating names for autoforks
	static int nF;
	static const char* autoForkName();
public:
	AutoFork(Geodesic& g) : geo(g), forkStar(0), forkOutput(0) {}
	~AutoFork() {}
	PortHole* setSource(GenericPort&, int delay = 0);
	PortHole* setDest(GenericPort&, int alwaysFork = 0);
};
#endif
