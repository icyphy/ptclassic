#ifndef _DEUniverse_h
#define _DEUniverse_h 1

#include "Universe.h"
#include "DEScheduler.h"

// SCCS version identification
// $Id$

/*******************************************************************

	Programmer : Soonhoi Ha
	Description : DEUniverse is defined here.
	Modification date : 5/30/90 - 

	Revised to be a type of Universe: J. Buck, 6/6/90
********************************************************************/

	//////////////////////////////
	// DEUniverse
	//////////////////////////////

const char DEstring[] = "DISCRETE EVENT";
	
class DEUniverse : public Universe {
public:
	DEUniverse() : Universe(new DEScheduler,DEstring) {}
};

#endif
