static const char file_id[] = "AsmTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

 Programmer: J. Buck, J.Pino

 Basic assembly language code Target for a single processor.
 Either an SDFScheduler or a LoopScheduler object can be used as
 the scheduler.  More specialized Targets (for a specific
 processor, etc) can be derived from this.

*******************************************************************/
#include "AsmTarget.h"
#include "GalIter.h"
#include "AsmStar.h"
#include "ProcMemory.h"
#include "CGUtilities.h"
#include "KnownBlock.h"
#include "miscFuncs.h"
#include "pt_fstream.h"


// constructor
AsmTarget :: AsmTarget(const char* nam, const char* desc,
		  const char* stype, ProcMemory* m) :
		CGTarget(nam,stype,desc), mem(m), interruptFlag(FALSE)
{ initStates();}

// all AsmTargets permit AnyAsmStar.
const char* AsmTarget :: auxStarClass() const { return "AnyAsmStar";}

void AsmTarget :: initStates() {
	// change default value of destDirectory
	destDirectory.setInitValue("~/DSPcode");

	targetHost.setAttributes(A_SETTABLE);
	filePrefix.setAttributes(A_SETTABLE);
	displayFlag.setAttributes(A_SETTABLE);
	runFlag.setAttributes(A_SETTABLE);
}

void AsmTarget :: headerCode() {
	CGTarget :: headerCode();
	disableInterrupts();
}

int AsmTarget::allocateMemory() {
// clear the memory
	if (mem == 0) return FALSE;
	mem->reset();

	GalStarIter nextStar(*galaxy());
// request memory, using the Target, for all stars in the galaxy.
// note that allocReq just posts requests; performAllocation processes
// all the requests.  Note we've already checked that all stars are AsmStar.
	AsmStar* s;
	while ((s = (AsmStar*)nextStar++) != 0) {
		if (!allocReq(*s)) return FALSE;
	}
	if (!mem->performAllocation()) return FALSE;

	return TRUE;
}

int AsmTarget :: codeGenInit() {
	// initialize the porthole offsets, and do all initCode methods.
	GalStarIter nextStar(*galaxy());
	AsmStar* s;
	while ((s = (AsmStar*)nextStar++) != 0) {
		BlockPortIter next(*s);
		AsmPortHole* p;
		while ((p = (AsmPortHole*) next++) != 0) {
			if (!p->initOffset()) return FALSE;
		}
		doInitialization(*s);
	}
	if (interruptFlag) enableInterrupts();
	return TRUE;
}

// request memory for all structures in a star
int
AsmTarget::allocReq(AsmStar& star) {
// first, allocate for portholes.  We only allocate memory for input ports;
// output ports share the same buffer as the input port they are connected to.
// (We do, however, allocate memory for output event horizon ports).
	AsmStarPortIter nextPort(star);
	AsmPortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItOutput() && p->far()->isItInput()) continue;
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

// Routines to modify the galaxy to permit use of the loop scheduler.

inline int hasCirc(PortHole* p) {
	return (p->attributes() & PB_CIRC) != 0;
}

inline int wormEdge(PortHole& p) {
	PortHole* f = p.far();
	if (!f) return TRUE;
	else return p.atBoundary();
}

extern int warnIfNotConnected (Galaxy&);

// Here's the main guy.
int AsmTarget::modifyGalaxy() {
	Galaxy& g = *galaxy();
	const char* dom = g.domain();
	if (!int(loopingLevel)) return TRUE;
	// init and call start methods.  We must do this so that
	// the numberTokens values will be correct.
	if (warnIfNotConnected(g)) return FALSE;
	g.initialize();

	// compute repetitions.
	SDFScheduler* sched = (SDFScheduler*) scheduler();
	sched->setGalaxy(g);
	sched->repetitions();

	if (Scheduler::haltRequested()) return FALSE;
	int status = TRUE;
	GalStarIter nextStar(g);
	Star* s;
	while ((s = nextStar++) != 0) {
		BlockPortIter nextPort(*s);
		DFPortHole* p;
		while ((p = (DFPortHole*) nextPort++) != 0 && status) {
			int nread;
			int nwrite;
			int boundaryFlag = 0;
			if (p->atBoundary()) {
				if (p->parentReps() == 0) continue;
				boundaryFlag = 1;
				if (p->isItOutput()) {
					nread = p->numXfer() * 
						p->parentReps();
					nwrite = p->far()->numXfer();
				} else {
					nread = p->far()->numXfer();
					nwrite = p->numXfer() * 
						p->parentReps();
				}
				if (nread == nwrite) continue;
			} else if (p->isItOutput() || 
			    p->numXfer() == p->far()->numXfer()) {
				continue;
			} else {
				nread = p->numXfer();
				nwrite = p->far()->numXfer();
			}

			// we have a rate conversion.
			if (nread < nwrite && nwrite%nread == 0) {
				// I run more often than my writer.
				// must have P_CIRC but writer does not
				// need it.
				if (!hasCirc(p)) {
					Block* newb =
					   spliceStar(p, "CircToLin",0,dom);
					if (!newb) return FALSE;
					PortHole* newP = 
						newb->portWithName("input");
					if (boundaryFlag) {
					    p->aliasFrom()->setAlias(*newP);
					}
				}
			}
			else if (nread > nwrite && nread%nwrite == 0) {
				// My writer runs more often than me.
				// It needs PB_CIRC, I do not.
				if (!hasCirc(p->far())) {
					Block* newb = 
					   spliceStar(p, "LinToCirc",1,dom);
					if (!newb) return FALSE;
					PortHole* newP = 
						newb->portWithName("output");
					if (boundaryFlag) {
					    p->aliasFrom()->setAlias(*newP);
					}
				}
			}
 			else {
				// nonintegral rate conversion, both need
				// PB_CIRC
				Block* newb;
				PortHole* newP = p;
				if (!hasCirc(newP)) {
					newb = spliceStar(newP, "CircToLin", 0,dom);
					newP = newb->portWithName("input");
					if (!newP) return FALSE;
				}
				if (!hasCirc(newP->far())) {
					if (!spliceStar(newP, "LinToCirc", 1,dom))
						return FALSE;
				}
			}
		}
	}
	return status;
}

void AsmTarget::disableInterrupts() {}

void AsmTarget::enableInterrupts() {}

void AsmTarget::saveProgramCounter() {
	myCode << "Save program counter";
}

void AsmTarget::restoreProgramCounter() {
	myCode << "Restore program counter";
}

// dummies to do code generation for wormhole data motion.  These
// produce comments.
void AsmTarget :: wormInputCode(PortHole& p) {
	AsmPortHole *ap = (AsmPortHole*)&p;
	StringList msg = "Xfer ";
	msg << ap->bufSize() << " values from host to location ";
	msg << ap->location();
	myCode << comment(msg);
}

void AsmTarget :: wormOutputCode(PortHole& p) {
	AsmPortHole *ap = (AsmPortHole*)&p;
	StringList msg = "Xfer ";
	msg << ap->bufSize() << " values from location ";
	msg << ap->location() << " to host";
	myCode << comment(msg);
}

/*virtual*/ void AsmTarget::frameCode() {
    if (procedures.numPieces() > 0) {
	StringList heading = "\n\n\nProcedures Begin\n\n";
	StringList tail = "\n\n\nProcedures End\n\n";
	myCode << comment(heading) <<  procedures << comment(tail);
    }
    StringList memMap = mem->printMemMap("","");
    myCode << comment(memMap);
}

/*virtual*/ void AsmTarget :: writeCode() {
    writeFile(myCode,".asm",displayFlag);
}
