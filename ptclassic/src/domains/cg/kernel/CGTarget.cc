static const char file_id[] = "CGTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

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
#include "CGDisplay.h"

// Return a string for indenting to the given depth
StringList CGTarget::indent(int depth) {
	StringList out;
	out = "";
	for(int i=0; i<depth; i++) {
		out += "    ";
	}
	return out;
}

// code to do initialiation for a star
void CGTarget::doInitialization(CGStar& cgStar) {
	cgStar.initCode();
}

// constructor
CGTarget::CGTarget(const char* name,const char* starclass,
		   const char* desc) : Target(name,starclass,desc) {}

void CGTarget :: initialize() {
	myCode.initialize();
	Target::initialize();
}

int CGTarget::setup(Galaxy& g) {
	// reset the label counter
	numLabels = 0 ;

	if (!Target::setup(g)) return FALSE;

	// BUG ALERT?
	// It is not clear whether the following is correct.
	// AsmTarget and CompileTarget have to bypass it.
	// The cast to CGPortHole clearly creates the problem for
	// CompileTarget.  It's not clear what the problem is for
	// AsmTarget.

	// initialize the porthole offsets, and do all initCode methods.
	GalStarIter nextStar(g);
	CGStar* s;
	while ((s = (CGStar*)nextStar++) != 0) {
		BlockPortIter next(*(Block*)s);
		PortHole* p;
                while ((p = next++) != 0) {
                        if (!((CGPortHole*)p)->initOffset()) return FALSE;
                }
                doInitialization(*s);
        }
        return TRUE;
}

void CGTarget :: start() {
	if (!mySched() && !parent()) {
		LOG_NEW; setSched (new SDFScheduler);
	}
	headerCode();
}

int CGTarget :: run() {
	// Sorry about the following horrible cast.
	// Design of kernel Scheduler makes it very difficult to avoid
	int iters = ((SDFScheduler*)mySched())->getStopTime();
	StringList startIter = beginIteration(iters,1);
	addCode(startIter);
	mySched()->setStopTime(1);
	int i = Target::run();
	StringList endIter = endIteration(iters,1);
	addCode(endIter);
	return i;
}

Block* CGTarget :: clone() const {
	LOG_NEW; return new CGTarget(readName(),starType(),readDescriptor());
}

void CGTarget :: addCode(const char* code) {
	if(code) myCode += code;
}

void CGTarget :: headerCode () {
	StringList code = "/* generated code for target ";
	code += readFullName();
	code += " */\n";
	addCode(code);
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

CGTarget :: ~CGTarget() { delSched();}

void CGTarget :: outputComment (const char* msg) {
	StringList code = "/* ";
        code += msg;
        code += " */\n";
        addCode(code);
}
