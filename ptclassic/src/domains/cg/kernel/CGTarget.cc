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
	addState(destDirectory.setState("destDirectory",this,"PTOLEMY_SYSTEMS",
			"Directory to write to"));
	addState(loopScheduler.setState("loopScheduler",this,"NO",

			"Specify whether to use loop scheduler."));
}

// destructor
CGTarget::~CGTarget() {
	delSched();
	LOG_DEL; delete schedFileName;
}

// dummy functions that perform parts of setup.
int CGTarget :: modifyGalaxy(Galaxy&) { return TRUE; }
int CGTarget :: allocateMemory(Galaxy&) { return TRUE; }
int CGTarget :: codeGenInit(Galaxy&) { return TRUE; }

void CGTarget :: initStars(Galaxy& g) {
	CGStar* s;
	GalStarIter next(g);
	while ((s = (CGStar*)next++) != 0 ) {
		s->codeblockSymbol.setTarget(this);
		s->starSymbol.setTarget(this);
	}
}

// the main guy.
int CGTarget::setup(Galaxy& g) {
	// reset the label counter
	numLabels = 0 ;

	gal = &g;

	targetNestedSymbol.initialize();
	targetNestedSymbol.setTarget(this);

	if (!modifyGalaxy(g)) return FALSE;

	if (!noSchedule) 
		if (!Target::setup(g)) return FALSE;
	else
		g.initialize();

	initStars(g);

	// choose sizes for buffers and allocate memory, if needed
	if(!allocateMemory(g)) return FALSE;

	if(!codeGenInit(g)) return FALSE;

	// if inside a wormhole, proceed to generate and download
	// code now.
	if(g.parent()) return wormCodeGenerate(g);
	else return TRUE;
}

// default code generation for a wormhole.  This produces an infinite
// loop that reads from the inputs, processes the schedule, and generates
// the outputs.
int CGTarget :: wormCodeGenerate(Galaxy&g) {
	BlockPortIter nextPort(g);
	PortHole* p;
	beginIteration(-1,0);
	// generate wormhole inputs
	while ((p = nextPort++) != 0) {
		if (p->isItInput()) wormInputCode(p->newConnection());
	}
	mySched()->compileRun();
	nextPort.reset();
	// generate wormhole outputs
	while ((p = nextPort++) != 0) {
		if (p->isItOutput()) wormOutputCode(p->newConnection());
	}
	endIteration(-1,0);
	frameCode();
	return wormLoadCode();
}

void CGTarget :: start() {
	noSchedule = 0;
	myCode.initialize();
	LOG_DEL; delete dirFullName;
	dirFullName = writeDirectoryName(destDirectory);
	if (!mySched()) {
		if(int(loopScheduler)) {
			schedFileName = writeFileName("schedule.log");
			LOG_NEW; setSched(new SDFClustSched(schedFileName));
		} else {
			LOG_NEW; setSched(new SDFScheduler);
		}
	}
	headerCode();
}

int CGTarget :: run() {
	// if a wormhole, setup already generated code.
	// We must do the transfer of data to and from the target.
	if(gal->parent()) {
		BlockPortIter nextPort(*gal);
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
		int iters = (int)mySched()->getStopTime();
		beginIteration(iters,0);
		mySched()->compileRun();
		endIteration(iters,0);
		frameCode();
	}
	return !Scheduler::haltRequested();
}

void CGTarget :: frameCode() {}

void CGTarget :: copySchedule(SDFSchedule& s) {
	SDFScheduler* sched = (SDFScheduler*) mySched();
	sched->copySchedule(s);
	// indicate multiprocessor scheduler already generated schedule
	noSchedule = TRUE;
	// make this processor runs forever
	sched->setStopTime(-1);	
}

StringList CGTarget :: generateCode(Galaxy& g) {
	setup(g);
	run();
	return myCode;
}

Block* CGTarget :: clone() const {
	LOG_NEW; CGTarget*t= new CGTarget(readName(),starType(),readDescriptor());
	t->copyStates(*this);
	return t;
}

void CGTarget :: addCode(const char* code) {
	if(code) myCode += code;
}

void CGTarget :: headerCode () {
	StringList code = "generated code for target ";
	code += readFullName();
	outputComment(code);
}

void CGTarget :: beginIteration(int reps, int depth) {
	myCode << "REPEAT " << reps << " TIMES { /* depth " << depth << "*/\n";
}

void CGTarget :: endIteration(int reps, int depth) {
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
	myCode << "/* READ from wormhole port " << p.readFullName() << " */\n";
}

void CGTarget :: wormOutputCode(PortHole& p) {
	myCode << "/* WRITE to wormhole port " << p.readFullName() << " */\n";
}

int CGTarget :: sendWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cout << "sending " << cp.bufSize() << " values to worm port " << cp.readFullName() << "\n";
	cout.flush();
	// data are discarded
	cp.forceGrabData();
	return TRUE;
}

int CGTarget :: receiveWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	int size = cp.bufSize();
	cout << "receiving " << size <<
		" values from worm port " << p.readFullName() << "\n";
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
	s.fire();
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
	cmd += dirFullName;
	cmd += ";";
	cmd += command;
	int i = system(cmd);
	if(i != 0 && error != NULL) Error::abortRun(error);
	return i;
}
