static const char file_id[] = "S56XTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino

 Target for Ariel S-56X DSP board.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "S56XTarget.h"
#include "KnownTarget.h"
#include "MotorolaTarget.h"

S56XTarget :: S56XTarget(const char* nam, const char* desc) :
	CG56Target(nam,desc),MotorolaTarget(nam,desc,"CG56Star")
{
	initStates();
}

S56XTarget::S56XTarget(const S56XTarget& arg) :
	CG56Target(arg),MotorolaTarget(arg)
{
	initStates();
	copyStates(arg);
}

void S56XTarget :: initStates() {
	xMemMap.setValue("0-255 8192-16383");
	yMemMap.setValue("0-16383");
	xMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
	yMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
	runFlag.setValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	targetHost.setAttributes(A_SETTABLE|A_NONCONSTANT);
}

void S56XTarget :: headerCode () {
	CG56Target :: headerCode();
	const char *path = expandPathName("~ptolemy/lib/cg56/s56x.asm");
	StringList inc;
	inc << "\tinclude '" << path << "'\n";
	addCode(inc);
	interruptFlag = TRUE;
};

Block* S56XTarget::clone() const {
	LOG_NEW; return new S56XTarget(*this);
}

void S56XTarget :: wrapup () {
	addCode (
		 "MAINEND equ	*\n"
		 "	jmp	ERROR\n"
		 "	org	p:$1ff0\n"
		 "	nop\n"
		 "	stop\n"
		 "	org	p:MAINEND\n");
		
	inProgSection = TRUE;
	CG56Target::wrapup();
}

int S56XTarget :: compileCode() {
	StringList assembleCmds = "asm56000 -b -l -A -oso ";
	assembleCmds += uname;
	if (!genFile(miscCmds , uname,".aio")) return FALSE;
	return !systemCall(assembleCmds,"Errors in assembly");
}

int S56XTarget :: loadCode() {
	StringList downloadCmds = "load_s56x ";
	downloadCmds += fullFileName(uname,".lod\n");
	return !hostSystemCall(downloadCmds,
		"Problems loading code onto S56X");
}

int S56XTarget :: runCode() {
	if (!genFile(runCmds,uname)) return FALSE;
	chmod(fullFileName(uname),0755);	//make executable
	StringList runCmd(uname);
	runCmd << " &";
	return !hostSystemCall(runCmd,"Problems running code onto S56X");
}


ISA_FUNC(S56XTarget,CG56Target);

// make an instance
static S56XTarget proto("S-56X","run code on the S-56X card");

static KnownTarget entry(proto, "S-56X");

