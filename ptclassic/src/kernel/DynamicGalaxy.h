/*******************************************************************
SCCS version identification
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.
 Programmer : Soonhoi Ha
 Date : Jan. 14, 1992

********************************************************************/
#ifndef _DynamicGalaxy_h
#define _DynamicGalaxy_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Galaxy.h"

// This galaxy is the baseclass for a family of dynamically created
// galaxies.  Blocks, portholes, and other objects in the galaxy are
// assumed to be on the heap and are removed by the destructor.

class DynamicGalaxy : public Galaxy {
protected:
	void zero();		// remove contained objects
public:
	~DynamicGalaxy() { zero();}

	// the makeNew function returns an error and a null pointer.
	Block* makeNew() const;

	// class identification
        int isA(const char*) const;

};

#endif
