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
	xMemMap.setValue("0-255,8192-16383");
	yMemMap.setValue("0-16383");
	xMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
	yMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
	runFlag.setValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	targetHost.setAttributes(A_SETTABLE|A_NONCONSTANT);
	addStream("aioCmds",&aioCmds);
	addStream("shellCmds",&shellCmds);
}

void S56XTarget :: headerCode () {
	aioCmds.initialize();
	shellCmds.initialize();
	CG56Target :: headerCode();
	const char *path = expandPathName("~ptolemy/lib/cg56/s56x.asm");
	myCode << "\tinclude '" << path << "'\n";
	interruptFlag = TRUE;
	shellCmds << headerComment("# ");
};

Block* S56XTarget::makeNew() const {
	LOG_NEW; return new S56XTarget(*this);
}

void S56XTarget :: trailerCode () {
	myCode << "	jmp	ERROR\n";
	inProgSection = TRUE;
	CG56Target::trailerCode();
}

int S56XTarget :: compileCode() {
	StringList assembleCmds;
	assembleCmds << "asm56000 -b -l -A -oso " << uname;
	return !systemCall(assembleCmds,"Errors in assembly");
}

void S56XTarget :: writeCode() {
	// write aio file
	genFile(aioCmds , uname,".aio");

	// write shell script file
	char* file = fullFileName(uname);
	shellCmds << "load_s56x " << file << ".lod\n";
	genFile(shellCmds,uname);
	chmod(file,0755);	//make executable
	LOG_DEL; delete(file);
	CG56Target :: writeCode();
}

int S56XTarget :: runCode() {
	StringList runCmd;
	runCmd << uname << " &";
	return !systemCall(runCmd,"Problems running code onto S56X");
}


ISA_FUNC(S56XTarget,CG56Target);

// make an instance
static S56XTarget proto("S-56X","run code on the S-56X card");

static KnownTarget entry(proto, "S-56X");

