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
#include "CGDisplay.h"
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
	uname = 0;
	StringList hostPrompt,hostDes,runPrompt,runDes;
	hostPrompt << "Host for " << fullName();
	hostDes    << "Host on which " << fullName() << " is installed";
	runPrompt  << "Run " << fullName() << "?";
	runDes     << "Download and run on " << fullName();
 	addState(displayFlag.setState("Display code?",this,"YES",
		"display code if YES."));
	addState(runFlag.setState(savestring(runPrompt),this,"NO",
		savestring(runDes), A_NONSETTABLE|A_NONCONSTANT));
	addState(targetHost.setState(savestring(hostPrompt),this, 
		"localhost", savestring(hostDes), 
		A_NONSETTABLE|A_NONCONSTANT));
	// change default value of destDirectory
	destDirectory.setInitValue("~/DSPcode");
}

void AsmTarget :: setup() {
	if (galaxy()) {
		LOG_DEL; delete uname;
		uname = makeLower(galaxy()->name());
	}
	CGTarget::setup();
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
	if (!int(loopingLevel)) return TRUE;
	// init and call start methods.  We must do this so that
	// the numberTokens values will be correct.
	if (warnIfNotConnected(g)) return FALSE;
	g.initialize();
	if (Scheduler::haltRequested()) return FALSE;
	int status = TRUE;
	GalStarIter nextStar(g);
	Star* s;
	while ((s = nextStar++) != 0) {
		BlockPortIter nextPort(*s);
		PortHole* p;
		while ((p = nextPort++) != 0 && status) {
			if (p->isItOutput() || wormEdge(*p) ||
			    p->numberTokens == p->far()->numberTokens)
				continue;
			// we have a rate conversion.
			int nread = p->numberTokens;
			int nwrite = p->far()->numberTokens;
			if (nread < nwrite && nwrite%nread == 0) {
				// I run more often than my writer.
				// must have P_CIRC but writer does not
				// need it.
				if (!hasCirc(p)) {
					if (!spliceStar(p, "CircToLin",0))
						return FALSE;
				}
			}
			else if (nread > nwrite && nread%nwrite == 0) {
				// My writer runs more often than me.
				// It needs PB_CIRC, I do not.
				if (!hasCirc(p->far())) {
					if (!spliceStar(p, "LinToCirc",1))
						return FALSE;
				}
			}
			else {
				// nonintegral rate conversion, both need
				// PB_CIRC
				if (!hasCirc(p)) {
					p = spliceStar(p, "CircToLin", 0);
					if (!p) return FALSE;
				}
				if (!hasCirc(p->far())) {
					if (!spliceStar(p, "LinToCirc", 1))
						return FALSE;
				}
			}
		}
	}
	return status;
}

PortHole* AsmTarget::spliceStar(PortHole* p, const char* name,
				int delayBefore)
{
	PortHole* pfar = p->far();
	int ndelay = p->numTokens();
	p->disconnect();
	if (p->isItOutput()) {
		PortHole* t = p;
		p = pfar;
		pfar = t;
	}
	
	// p is now an input port.
	Block* newb = KnownBlock::clone(name);
	if (newb == 0) {
		Error::abortRun("failed to clone a ", name, "!");
		return 0;
	}
	PortHole* ip = newb->portWithName("input");
	PortHole* op = newb->portWithName("output");
	if (ip == 0 || op == 0) {
		Error::abortRun(name, " has the wrong interface for splicing");
		LOG_DEL; delete newb;
		return 0;
	}

	// connect up the new star
	pfar->connect(*ip,delayBefore ? ndelay : 0);
	p->connect(*op,delayBefore ? 0 : ndelay);

	// save in the list of spliced stars
	spliceList.put(newb);

	// initialize the new star.  Name it and add it to the galaxy.
	// using size of splice list in name forces unique names.
	StringList newname("splice-");
	newname << newb->className() << "-" << spliceList.size();
	galaxy()->addBlock(*newb,hashstring(newname));
	newb->initialize();

	// check errors in initialization
	if (Scheduler::haltRequested()) return 0;
	return ip;
}

AsmTarget::~AsmTarget() {
	LOG_DEL; delete uname; uname = 0;
	ListIter next(spliceList);
	Block* b;
	while ((b = (Block*)next++) != 0) {
		// prevent some galaxy types from deleting b twice.
		galaxy()->removeBlock(*b);
		LOG_DEL; delete b;
	}
}
/*
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
*/
char* AsmTarget :: fullFileName(const char* base, const char* suffix)
{
	StringList bname = base;
	if (suffix != NULL) bname << suffix;
	char* fullName = writeFileName(bname);
	return fullName;
}

int AsmTarget::genDisFile(const char* stuff,const char* base,const char* suffix)
{
	char* name = fullFileName(base,suffix);
	int status = display(stuff,name);
	LOG_DEL; delete name;
	return status;
}

int AsmTarget :: genFile(const char* stuff,const char* base,const char* suffix)
{
	int status;
	if (stuff == NULL) return TRUE;
	char* fullName = fullFileName(base, suffix);
	pt_ofstream o(fullName);
	if (!o) status = FALSE;
	else {
		const char* stuff_str = stuff;	// hack
		o << stuff_str;
		o.flush();
		o.close();			// hack
		status = TRUE;
	}
	LOG_DEL; delete fullName;
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
    myCode << comment(mem->printMemMap("",""));
}

void AsmTarget :: writeCode(const char* name) {
	if (name==NULL) name=uname;
	if (haltRequested()) return;
        if (int(displayFlag)) {
                if (!genDisFile(myCode,name,asmSuffix())) return;
	}
        else {
                if (!genFile(myCode,name,asmSuffix())) return;
	}
}

void AsmTarget :: wrapup() {
	if (int(runFlag) && !haltRequested()) {
		if (compileCode() && !haltRequested()) {
			if (loadCode() && !haltRequested()) runCode();
		}
	}
}
