#ifndef _SDFUniverse_h
#define _SDFUniverse_h 1

#include "Universe.h"
#include "SDFScheduler.h"

// SCCS version identification
// $Id$

/*******************************************************************

	An SDF Universe is a Star with a pointer to a component Galaxy,
	and an SDF Scheduler to fire the blocks in the Galaxy.
	From the outside, it looks like a Star with no PortHoles.

	Revised to be a type of Universe: J. Buck, 6/6/90
********************************************************************/

	//////////////////////////////
	// SDFUniverse
	//////////////////////////////

const char SDFstring[] = "SYNCHRONOUS DATAFLOW";
	
class SDFUniverse : public Universe {
public:
	SDFUniverse() : Universe(new SDFScheduler,SDFstring) {}
};

#endif
