#ifndef _DynamicGalaxy_h
#define _DynamicGalaxy_h 1

#include "Galaxy.h"


/*******************************************************************
SCCS version identification
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.
 Programmer : Soonhoi Ha
 Date : Jan. 14, 1992

********************************************************************/

// This galaxy is created during the auto-wormholization process.
// It can destroy the component blocks.

class DynamicGalaxy : public Galaxy {
public:
	~DynamicGalaxy();

	// class identification
        int isA(const char*) const;

};

#endif
