/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/18/92

 Declaration for the default DE target. It used to be in DEDomain.cc file.

***********************************************************************/
#ifndef _DETarget_h
#define _DETarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "IntState.h"
#include "FloatState.h"

class DETarget : public Target {
protected:
	FloatState timeScale;	// relative time scale for interface with
				// another timed domain.
	IntState   syncMode;	// If TRUE, the inner timed domain can not
				// be ahead of the outer DE domain.

public:
	DETarget();
	void start();
	Block* clone() const;
	~DETarget();
};

#endif
