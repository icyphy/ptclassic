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
#include "Error.h"
#include "UserOutput.h"
#include "SDFScheduler.h"

// constructor
CGTarget::CGTarget(const char* name,const char* starclass,
		   const char* desc) : Target(name,starclass,desc) {}

void CGTarget :: initialize() {
	myCode.initialize();
	Target::initialize();
}

void CGTarget :: start() {
	if (!sched && !parent()) sched = new SDFScheduler;
	headerCode();
}

Block* CGTarget :: clone() const {
	return new CGTarget(readName(),starType(),readDescriptor());
}

void CGTarget :: addCode(const char* code) {
	myCode += code;
}

void CGTarget :: headerCode () {
	StringList code = "/* generated code for target ";
	code += readFullName();
	code += " */\n";
	addCode(code);
}

void CGTarget :: wrapup() {
	Target :: wrapup();
	Error::message((const char*)myCode);
}

void CGTarget :: writeCode(UserOutput& o) {
	o << myCode;
	o.flush();
}

CGTarget :: ~CGTarget() { delete sched;}
