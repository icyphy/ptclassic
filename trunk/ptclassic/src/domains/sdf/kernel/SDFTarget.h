/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/22/92

 Declaration for SDFTarget, the default target to be used in the SDF
 domain.  This used to be declared elsewhere.

 If loopScheduler is 1, a clustering scheduler is used (SDFClustSched),
 otherwise a default scheduler, SDFScheduler, is used.

***********************************************************************/
#ifndef _SDFTarget_h
#define _SDFTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "StringState.h"
#include "IntState.h"
#include "FloatState.h"

class SDFTarget : public Target {
protected:
	StringState logFile;
	IntState loopScheduler;

	// schedulePeriod for interface with a timed domain.
	FloatState schedulePeriod;

public:
	SDFTarget();
	void start();
	int setup(Galaxy&);
	Block* clone() const;
	~SDFTarget();
};
#endif
