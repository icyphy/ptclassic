#ifndef _AsmTarget_h
#define _AsmTarget_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 Basic assembly language code Target for a single processor.
 Either an SDFScheduler or a LoopScheduler object can be used as
 the scheduler.  More specialized Targets (for a specific
 processor, etc) can be derived from this.

*******************************************************************/

#include "CGTarget.h"
#include "SDFScheduler.h"

class ProcMemory;
class AsmStar;

class AsmTarget : public CGTarget {
private:
	const char* starClass;
protected:
	ProcMemory& mem;

	// Request memory for all structures in a Star
	int allocReq(AsmStar&);
public:
	AsmTarget(const char* nam, const char* desc,
		  const char* stype, ProcMemory& m) :
		CGTarget(nam,stype,desc), mem(m) {}

	Block* clone() const = 0;

	// Initialize proc. pointers and create the schedule.
	// Then allocate memory for all stars.
	int setup(Galaxy&);

};

#endif
