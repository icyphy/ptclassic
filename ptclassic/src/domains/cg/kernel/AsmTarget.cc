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
// clear the memory
	mem.reset();
// initialize proc ptrs and create schedule
	if (!Target::setup(g)) return FALSE;
	GalStarIter nextStar(g);
// request memory, using the Target, for all stars in the galaxy.
// note that allocReq just posts requests; performAllocation processes
// all the requests.  Note we've already checked that all stars are AsmStar.
	AsmStar* s;
	while ((s = (AsmStar*)nextStar++) != 0) {
		allocReq(*s);
	}
	if (!mem.performAllocation()) return FALSE;
// do all initCode methods.
	nextStar.reset();
	while ((s = (AsmStar*)nextStar++) != 0)
		s->initCode();
	return TRUE;
}

// request memory for all structures in a star
int
AsmTarget::allocReq(AsmStar& star) {
	int sflag = TRUE;
	int pflag = TRUE;
// first, allocate for portholes.
	AsmStarPortIter nextPort(star);
	AsmPortHole* p;
	while ((p = nextPort++) != 0)
		pflag = pflag && mem.allocReq(*p);
	BlockStateIter nextState(star);
	State* s;
	while ((s = nextState++) != 0)
		sflag = sflag && mem.allocReq(*s);
	if(!sflag) {
		Error::abortRun(
	   	    "Cannot find memory for States in ", star.readName());
		return FALSE;
	} else if(!pflag) {
		Error::abortRun(
	   	    "Cannot find memory for PortHoles in ", star.readName());
		return FALSE;
	} else {
		return TRUE;
	}
}

