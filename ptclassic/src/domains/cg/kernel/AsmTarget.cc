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
// clear the memory
	if (mem == 0) return FALSE;
	mem->reset();
// initialize proc ptrs and create schedule
	if (!Target::setup(g)) return FALSE;
	GalStarIter nextStar(g);
// request memory, using the Target, for all stars in the galaxy.
// note that allocReq just posts requests; performAllocation processes
// all the requests.  Note we've already checked that all stars are AsmStar.
	AsmStar* s;
	while ((s = (AsmStar*)nextStar++) != 0) {
		if (!allocReq(*s)) return FALSE;
	}
	if (!mem->performAllocation()) return FALSE;
// initialize the porthole offsets, and do all initCode methods.
	nextStar.reset();
	while ((s = (AsmStar*)nextStar++) != 0) {
		AsmStarPortIter next(*s);
		AsmPortHole* p;
		while ((p = next++) != 0) {
			if (!p->initOffset()) return FALSE;
		}
		doInitialization(*s);
	}
	return TRUE;
}

// request memory for all structures in a star
int
AsmTarget::allocReq(AsmStar& star) {
// first, allocate for portholes.  We only allocate memory for input ports;
// output ports share the same buffer as the input port they are connected to.
	AsmStarPortIter nextPort(star);
	AsmPortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItOutput()) continue;
		if (!mem->allocReq(*p)) {
			Error::abortRun(*p,
			  "memory allocation failed for porthole buffer:",
					"attribute mismatch");
			return FALSE;
		}
	}
	BlockStateIter nextState(star);
	State* s;
	while ((s = nextState++) != 0) {
		// skip states that are not in memory
		if ((s->attributes() & AB_MEMORY) == 0) continue;
		if (!mem->allocReq(*s)) {
			Error::abortRun(*s,
			      "memory allocation failed for state buffer:",
					"attribute mismatch");
			return FALSE;
		}
	}
	return TRUE;
}

void AsmTarget :: outputComment (const char* msg) {
	outputLineOrientedComment ("; ", msg, 80);
}

void AsmTarget :: outputLineOrientedComment(const char* prefix,
					    const char* msg,
					    int lineLen) {
	int plen = strlen(prefix);
	LOG_NEW; char* line = new char[lineLen-plen+1];
	strcpy (line, prefix);
	char* p = line + plen;
	int curlen = plen;
	while (*msg) {
		char c = *msg++;
		*p++ = c;
		if (c == '\t') curlen = ((curlen/8)+1)*8;
		else curlen++;
		if (curlen >= lineLen || c == '\n') {
			*p = 0;
			addCode(line);
			curlen = plen;
			p = line + plen;
		}
	}
	*p++ = '\n';
	*p = 0;
	addCode(line);
	LOG_DEL; delete line;
}

