static const char file_id[] = "AsmTarget.cc";
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
#include "AsmTarget.h"
#include "GalIter.h"
#include "AsmStar.h"
#include "ProcMemory.h"

int
AsmTarget::setup(Galaxy& g) {
// initialize proc ptrs and create schedule
	if (!Target::setup(g)) return FALSE;
	GalStarIter nextStar(g);
// allocate memory, using the Target, for all stars in the galaxy.
// note that allocMem just posts requests; performAllocation processes
// all the requests.  Note we've already checked that all stars are AsmStar.
	AsmStar* s;
	while ((s = (AsmStar*)nextStar++) != 0) {
		allocMem(*s);
	}
	if (!mem.performAllocation()) return FALSE;
// do all initCode methods.
	nextStar.reset();
	while ((s = (AsmStar*)nextStar++) != 0)
		s->initCode();
	return TRUE;
}

// allocate memory for all structures in a star
int
AsmTarget::allocMem(AsmStar& star) {
// first, allocate for portholes.
	AsmStarPortIter nextPort(star);
	AsmPortHole* p;
	while ((p = nextPort++) != 0)
		mem.allocReq(*p);
	BlockStateIter nextState(star);
	State* s;
	while ((s = nextState++) != 0)
		mem.allocReq(*s);
	return mem.performAllocation();
}

