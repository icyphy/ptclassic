static const char file_id[] = "AsmTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

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
#include "UserOutput.h"
#include "CGDisplay.h"
#include "CGUtilities.h"
#include "KnownBlock.h"
#include "miscFuncs.h"

void AsmTarget :: initStates() {
	uname = 0;
	StringList hostPrompt,hostDes,runPrompt,runDes;
	hostPrompt = "Host for "; hostPrompt += readClassName();
	hostDes= "Host on which "; hostDes+=readClassName();
	hostDes+=" is installed";
	runPrompt = "Run "; runPrompt += readClassName(); runPrompt += "?";
	runDes = "Download and run on "; runDes += readClassName();
 	addState(displayFlag.setState("Display code?",this,"YES",
		"display code if YES."));
	addState(runFlag.setState(savestring(runPrompt),this,"NO",
		savestring(runDes), A_NONSETTABLE|A_NONCONSTANT));
	addState(targetHost.setState(savestring(hostPrompt),this, 
		"localhost", savestring(hostDes), 
		A_NONSETTABLE|A_NONCONSTANT));
	// change default value of destDirectory
	destDirectory.setValue("~/DSPcode");
}

int AsmTarget :: setup(Galaxy& g) {
	runCmds.initialize();
 	miscCmds.initialize();
	LOG_DEL; delete uname; uname = 0;
	uname = makeLower(g.readName());
	return CGTarget::setup(g);
}

int AsmTarget :: compileTarget() {
	Error::
	  abortRun("No compileTarget method defined for current target");
	return FALSE;
}

int AsmTarget :: runTarget() {
	Error::
	  abortRun("No runTarget method defined for current target");
	return FALSE;
}

int AsmTarget :: hostSystemCall(const char* cmd, const char* err=NULL) {
	int val = rshSystem(targetHost,cmd,dirFullName);
	if (err != NULL && val != 0) Error::abortRun(err);
	return val;
}

void AsmTarget :: headerCode() {
	StringList code = "generated code for target ";
        code += readFullName();
	outputComment (code);
	disableInterrupts();
}

int AsmTarget::allocateMemory(Galaxy& g) {
// clear the memory
	if (mem == 0) return FALSE;
	mem->reset();

	GalStarIter nextStar(g);
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

int AsmTarget :: codeGenInit(Galaxy& g) {
	// initialize the porthole offsets, and do all initCode methods.
	GalStarIter nextStar(g);
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

// Routines to modify the galaxy to permit use of the loop scheduler.

inline int hasCirc(PortHole* p) {
	return (p->attributes() & PB_CIRC) != 0;
}

inline int wormEdge(PortHole& p) {
	PortHole* f = p.far();
	if (!f) return TRUE;
	else return (p.isItInput() == f->isItInput());
}

extern int warnIfNotConnected (Galaxy&);

// Here's the main guy.
AsmTarget::modifyGalaxy(Galaxy& g) {
	if (!int(loopScheduler)) return TRUE;
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
				int code;
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
	newname << newb->readClassName() << "-" << spliceList.size();
	gal->addBlock(*newb,hashstring(newname));
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
		gal->removeBlock(*b);
		LOG_DEL; delete b;
	}
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

char* AsmTarget :: fullFileName(char* base, const char* suffix=NULL)
{
	StringList bname = fileName(base,suffix);
	char* fullName = writeFileName(bname);
	return fullName;
}

char* AsmTarget :: fileName(char* base, const char* suffix=NULL)
{
	StringList bname = base;
	if (suffix != NULL) bname += suffix;
	return bname;
}

int AsmTarget::genDisFile(StringList& stuff,char* base,const char* suffix=NULL, const char* mode=NULL)
{
	char* name = fullFileName(base,suffix);
	int status = display(stuff,name);
	if (mode != NULL) {
		StringList command = "chmod ";
		command += mode;
		command += " ";
		command += name;
		system(command);
	}
	LOG_DEL; delete name;
	return status;
}

int AsmTarget :: genFile(StringList& stuff,char* base,const char* suffix=NULL, const char* mode=NULL)
{
	int status;
	char* fullName = fullFileName(base, suffix);
	UserOutput o;
	if (!o.fileName(fullName)) {
		Error::abortRun(*this, "can't open file for writing: ",
				fullName);
		status = FALSE;
	}
	else {
		o << stuff;
		o.flush();
		status = TRUE;
	}
	if (mode != NULL) {
		StringList command = "chmod ";
		command += mode;
		command += " ";
		command += fullName;
		system(command);
	}
	LOG_DEL; delete fullName;
	return status;
}

void AsmTarget::disableInterrupts() {
	outputComment("Disable Interrupts");
}

void AsmTarget::enableInterrupts() {
	outputComment("Enable Interrupts");
}

void AsmTarget::saveProgramCounter() {
	outputComment("Save program counter");
}

void AsmTarget::restoreProgramCounter() {
	outputComment("Restore program counter");
}

void AsmTarget :: wrapup() {
	if (int(displayFlag))
		if (!genDisFile(myCode,uname,asmSuffix())) return;
	else
		if (!genFile(myCode,uname,asmSuffix())) return;
	if (int(runFlag))
		if (!compileTarget()) runTarget();
}
