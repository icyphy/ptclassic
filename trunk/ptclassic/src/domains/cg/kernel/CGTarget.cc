static const char file_id[] = "CGTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

 Baseclass for all single-processor code generation targets.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGTarget.h"
#include "CGStar.h"
#include "GalIter.h"
#include "Error.h"
#include "SDFScheduler.h"
#include "SDFCluster.h"
#include "CGDisplay.h"
#include "miscFuncs.h"
#include <stream.h>

// Return a string for indenting to the given depth
StringList CGTarget::indent(int depth) {
	StringList out;
	out = "";
	for(int i=0; i<depth; i++) {
		out += "    ";
	}
	return out;
}

// constructor
CGTarget::CGTarget(const char* name,const char* starclass,
		   const char* desc, char sep)
: Target(name,starclass,desc), schedFileName(0), noSchedule(0)
{
	separator = sep;
	targetNestedSymbol.setSeparator(separator);
	addState(destDirectory.setState("destDirectory",this,"PTOLEMY_SYSTEMS",
			"Directory to write to"));
	addState(loopScheduler.setState("loopScheduler",this,"NO",
			"Specify whether to use loop scheduler."));
	addStream("code",&myCode);
}

// destructor
CGTarget::~CGTarget() {
	delSched();
	LOG_DEL; delete schedFileName;
}

// dummy functions that perform parts of setup.
int CGTarget :: modifyGalaxy() { return TRUE; }
int CGTarget :: allocateMemory() { return TRUE; }
int CGTarget :: codeGenInit() { return TRUE; }

// the main guy.
// If there is no galaxy yet, we just create a scheduler and return.
// This is not to my liking; the whole way we do this needs to be
// rethought.  It also means that any derived classes of CGTarget that
// are intended to be used as child targets of CGMultiTarget must
// behave the same way.

void CGTarget::setup() {
	myCode.initialize();
	writeDirectoryName(destDirectory);
	if (!scheduler()) {
		if(int(loopScheduler)) {
			schedFileName = writeFileName("schedule.log");
			LOG_NEW; setSched(new SDFClustSched(schedFileName));
		} else {
			LOG_NEW; setSched(new SDFScheduler);
		}
	}
	if (!galaxy()) return;
// We have a galaxy; continue the setup.
	headerCode();
	// reset the label counter
	BaseSymbolList::reset() ;

	targetNestedSymbol.initialize();

	if (!noSchedule) {
		if (!modifyGalaxy()) return;
		Target::setup();
		if (Scheduler::haltRequested()) return;
	}
	noSchedule = 0;		// reset for next setup.

	// choose sizes for buffers and allocate memory, if needed
	if(!allocateMemory() || !codeGenInit()) return;

	// if inside a wormhole, proceed to generate and download
	// code now.
	if(galaxy()->parent()) wormCodeGenerate();
}

// default code generation for a wormhole.  This produces an infinite
// loop that reads from the inputs, processes the schedule, and generates
// the outputs.
int CGTarget :: wormCodeGenerate() {
	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	beginIteration(-1,0);
	// generate wormhole inputs
	while ((p = nextPort++) != 0) {
		if (p->isItInput()) wormInputCode(p->newConnection());
	}
	scheduler()->compileRun();
	nextPort.reset();
	// generate wormhole outputs
	while ((p = nextPort++) != 0) {
		if (p->isItOutput()) wormOutputCode(p->newConnection());
	}
	endIteration(-1,0);
	frameCode();
	return wormLoadCode();
}

void CGTarget :: addStream(const char* name, StringList* slist)
{
	int flag = codeStringLists.add(name,slist);
	if (flag == FALSE)
	{
		StringList message;
		message << "addStream: " << name << " already exists";
		Error::abortRun(*this,message);
	}
}

StringList* CGTarget :: getStream(const char* name)
{
	return codeStringLists.get(name);
}

int CGTarget :: run() {
	// if a wormhole, setup already generated code.
	// We must do the transfer of data to and from the target.
	if(galaxy()->parent()) {
		BlockPortIter nextPort(*galaxy());
		PortHole* p;
		while ((p = nextPort++) != 0) {
			if (p->isItInput() &&
			    !sendWormData(p->newConnection())) return FALSE;
		}
		nextPort.reset();
		while ((p = nextPort++) != 0) {
			if (p->isItOutput() &&
			    !receiveWormData(p->newConnection())) return FALSE;
		}
	}
	else {
		// Note that stopTime in an SDF scheduler is always integral
		int iters = (int)scheduler()->getStopTime();
		beginIteration(iters,0);
		scheduler()->compileRun();
		endIteration(iters,0);
		frameCode();
	}
	return !Scheduler::haltRequested();
}

void CGTarget :: frameCode() {}

void CGTarget :: copySchedule(SDFSchedule& s) {
	SDFScheduler* sched = (SDFScheduler*) scheduler();
	sched->copySchedule(s);
	// indicate multiprocessor scheduler already generated schedule
	noSchedule = TRUE;
}

StringList CGTarget :: generateCode() {
	setup();
	if (galaxy()->parent() == 0) {
		run();
		Target::wrapup();
	}
	return myCode;
}

Block* CGTarget :: makeNew() const {
	LOG_NEW; return new CGTarget(name(),starType(),descriptor());
}

void CGTarget :: addCode(const char* code) {
	if(code) myCode += code;
}

void CGTarget :: headerCode () {
	StringList code = "generated code for target ";
	code += fullName();
	outputComment(code);
}

void CGTarget :: beginIteration(int reps, int depth) {
	myCode << "REPEAT " << reps << " TIMES { /* depth " << depth << "*/\n";
}

void CGTarget :: endIteration(int /*reps*/, int depth) {
	myCode << "} /* end repeat, depth " << depth << "*/\n";
}

int CGTarget :: compileCode() {
	Error::abortRun("No compileCode method defined for current target");
	return FALSE;
}

int CGTarget :: loadCode() {
	Error::abortRun("No loadCode method defined for current target");
	return FALSE;
}

int CGTarget :: runCode() {
	Error::abortRun("No runCode method defined for current target");
	return FALSE;
}

void CGTarget :: wormInputCode(PortHole& p) {
	myCode << "/* READ from wormhole port " << p.fullName() << " */\n";
}

void CGTarget :: wormOutputCode(PortHole& p) {
	myCode << "/* WRITE to wormhole port " << p.fullName() << " */\n";
}

int CGTarget :: sendWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cout << "sending " << cp.bufSize() << " values to worm port " << cp.fullName() << "\n";
	cout.flush();
	// data are discarded
	cp.forceGrabData();
	return TRUE;
}

int CGTarget :: receiveWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	int size = cp.bufSize();
	cout << "receiving " << size <<
		" values from worm port " << p.fullName() << "\n";
	cout.flush();
	// insert zero-valued particles onto cp's geodesic
	for (int i = 0; i < size; i++)
		cp%i << 0;
	cp.forceSendData();
	return TRUE;
}

int CGTarget :: wormLoadCode() {
	display(myCode);
	return TRUE;
}

void CGTarget :: writeFiring(Star& s,int) { // depth parameter ignored
	s.run();
}

void CGTarget :: wrapup() {
	Target :: wrapup();
	display(myCode);
}

void CGTarget :: writeCode(ostream& o) {
	o << myCode;
	o.flush();
}

ISA_FUNC(CGTarget,Target);

void CGTarget :: outputComment (const char* msg) {
	StringList code = "/* ";
        code += msg;
        code += " */\n";
        addCode(code);
}

int CGTarget :: systemCall(const char* command, const char* error) {
	StringList cmd = "cd ";
	cmd += workingDirectory();
	cmd += ";";
	cmd += command;
	int i = system(cmd);
	if(i != 0 && error != NULL) Error::abortRun(error);
	return i;
}
