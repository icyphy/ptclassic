/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Shuvra Bhattacharyya
 Modifier: Soonhoi Ha (5/92)
 Date of creation: 5/9/91

Loop scheduler

*******************************************************************/

#ifndef _LoopScheduler_h
#define _LoopScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

#include "SDFCluster.h"
#include "StringList.h"

/////////////////////////
// class LoopScheduler //
/////////////////////////

class LoopScheduler : public SDFClustSched {
protected:
	//
	// First step: decomposition
	// Second step: for each decomposed galaxy, do main scheduling.
	// Return TRUE and leave the invalid flag clear if and
	// only if everything went OK.
	//
	int computeSchedule(Galaxy& g);

public:
	// constructor
	LoopScheduler(const char* log) : SDFClustSched(log) {}

	// Display the schedule
	StringList displaySchedule();

	// Generate code using the Target to produce the right language.
	void compileRun();
};

#endif
