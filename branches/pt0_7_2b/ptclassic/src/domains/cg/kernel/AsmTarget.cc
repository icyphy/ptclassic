/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 Basic assembly language code architecture for a single processor.
 Either an SDFScheduler or a LoopScheduler object can be used as
 the scheduler.  More specialized architectures (for a specific
 processor, etc) can be derived from this.

*******************************************************************/
#include "AsmArch.h"
#include "GalIter.h"
#include "AsmStar.h"
#include "ProcMemory.h"

int
AsmArchSched::setup(Block& b) {
	Galaxy& g = b.asGalaxy();
	GalStarIter nextStar(g);
	Star* s;
	const char* typ = asmArch()->starType();
	while ((s = nextStar++) != 0) {
		if (!s->isA(typ)) {
			Error::abortRun(*s,"is not of type ", typ);
			return FALSE;
		}
		s->setTarget(asmArch());
	}
// compute schedule
	if (!sched->setup(g)) return FALSE;
// allocate memory, using the architecture, for all stars in the galaxy.
// the cast is safe because of the test above.
// note that allocMem just posts requests; performAllocation processes
// all the requests.
	nextStar.reset();
	while ((s = nextStar++) != 0) {
		AsmStar& star = *(AsmStar*)s;
		asmArch()->allocMem(star);
	}
	if (!asmArch()->performAllocation()) return FALSE;
	asmArch()->headerCode();
}

// allocate memory for all structures in a star
int
AsmArch::allocMem(AsmStar& star) {
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

int AsmArch::performAllocation() { return mem.performAllocation();}
