/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 4/22/92

 Declaration for DDFTarget, the default target to be used in the DDF
 domain.  This used to be declared elsewhere.

 If restructure is 1, auto-wormholization is performed (default).

***********************************************************************/
#ifndef _DDFTarget_h
#define _DDFTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "StringState.h"
#include "IntState.h"
#include "FloatState.h"

class DDFTarget : public Target {
protected:
	IntState restructure;

	// For the original DDF scheduler, it defines the number of
	// execution cycles to be overlapped in execution.
	IntState numOverlapped;

	// The user can specify the maximum buffer size on each arc.
	// Unbounded arc is detected at runtime by comparing the arc size
	// and this limit.
	IntState maxBufferSize;

	// schedulePeriod for interface with a timed domain.
	FloatState schedulePeriod;

public:
	DDFTarget();
	Block* makeNew() const;
	void setup();
	~DDFTarget();
};
#endif
