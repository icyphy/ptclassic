/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 Baseclass for all single-processor code generation targets.

*******************************************************************/
#include "CGTarget.h"
#include "Error.h"
#include "SDFScheduler.h"

// constructor
CGTarget::CGTarget(const char* name,const char* starclass,
		   const char* desc) : Target(name,starclass,0,desc) {}

void CGTarget :: initialize() {
	myCode.initialize();
	Target::initialize();
}

void CGTarget :: start() {
	if (!sched) sched = new SDFScheduler;
	headerCode();
}

Block* CGTarget :: clone() const {
	return new CGTarget(readName(),starType(),readDescriptor());
}

void CGTarget :: addCode(const char* code) {
	myCode += code;
}

void CGTarget :: headerCode () {
	addCode("/* generated code */\n");
}

void CGTarget :: wrapup() {
	Target :: wrapup();
	Error::message((const char*)myCode);
}

CGTarget :: ~CGTarget() { delete sched;}
