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
#include "UserOutput.h"
#include "SDFScheduler.h"
#include "SDFCluster.h"
#include "CGDisplay.h"
#include "miscFuncs.h"

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
		   const char* desc, char sep = '_')
: Target(name,starclass,desc), schedFileName(0)
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

	if (!Target::setup(g)) return FALSE;

	initStars(g);

	// choose sizes for buffers and allocate memory, if needed
	if(!allocateMemory(g)) return FALSE;

	return codeGenInit(g);
}

void CGTarget :: start() {
	myCode.initialize();
	LOG_DEL; delete dirFullName;
	dirFullName = writeDirectoryName(destDirectory);
	if (!mySched() && !parent()) {
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
	// Note that stopTime in an SDF scheduler is always integral
	int iters = (int)mySched()->getStopTime();
	beginIteration(iters,0);
	mySched()->compileRun();
	endIteration(iters,0);
	return !Scheduler::haltRequested();
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

void CGTarget :: writeFiring(Star& s,int) { // depth parameter ignored
	s.fire();
}

void CGTarget :: wrapup() {
	Target :: wrapup();
	display(myCode);
}

void CGTarget :: writeCode(UserOutput& o) {
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

int CGTarget :: systemCall(const char* command, const char* error=NULL) {
	StringList cmd = "cd ";
	cmd += dirFullName;
	cmd += ";";
	cmd += command;
	int i = system(cmd);
	if(i != 0 && error != NULL) Error::abortRun(error);
	return i;
}
