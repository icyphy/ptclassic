/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck

 Declaration for BDFTarget, the default target to be used in the BDF
 domain.  Very similar to SDFTarget.

 If loopScheduler is 1, a clustering scheduler is used (BDFClustSched),
 otherwise a default scheduler, BDFScheduler, is used.

***********************************************************************/
#ifndef _BDFTarget_h
#define _BDFTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "StringState.h"
#include "IntState.h"
#include "FloatState.h"

class BDFTarget : public Target {
protected:
	StringState logFile;
	IntState loopScheduler;

	// schedulePeriod for interface with a timed domain.
	FloatState schedulePeriod;

public:
	BDFTarget();
	void setup();
	Block* makeNew() const;
	~BDFTarget();
};
#endif
