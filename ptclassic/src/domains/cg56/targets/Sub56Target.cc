static const char file_id[] = "Sub56Target.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino

 Target for Motorola 56000 assembly code generation that generates 
 two subroutines, ptolemyInit, ptolemyMain.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Sub56Target.h"
#include "UserOutput.h"
#include "CG56Star.h"
#include "KnownTarget.h"

int Sub56Target :: run() {
	addCode("	rts\n");
	addCode("ptolemyMain\n");
	mySched()->setStopTime(1);
	int i = Target::run();
	addCode("	rts\n");
	return i;
}

void Sub56Target :: headerCode () {
	CG56Target :: headerCode();
	addCode(
		"	org	p:\n"
		"ptolemyInit\n");
};

Block* Sub56Target::clone() const {
	LOG_NEW; return new Sub56Target(*this);
}

ISA_FUNC(Sub56Target,CG56Target);

//make an instance
static Sub56Target proto("sub-CG56","generate subroutines pigiInit and pigiMain");

static KnownTarget entry(proto, "sub-CG56");
