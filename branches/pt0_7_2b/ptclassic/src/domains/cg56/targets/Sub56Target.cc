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
#include <ctype.h>

Sub56Target :: Sub56Target(const char* nam, const char* desc,
			 unsigned x_addr, unsigned x_len,
			 unsigned y_addr, unsigned y_len) :
	CG56Target(nam,desc,x_addr,x_len,y_addr,y_len), uname(0)
{
	initStates();
}

void Sub56Target :: initStates() {
	addState(dirName.setState("dirName",this,"~/DSPcode",
                                  "directory for all output files"));
}

Sub56Target :: ~Sub56Target() {
	LOG_DEL; delete dirFullName; dirFullName = 0;
	LOG_DEL; delete uname;
}

int Sub56Target :: run() {
	StringList rts = "\trts\n";
	addCode(rts);
	StringList ptolemyMain = "ptolemyMain\n";
	addCode(ptolemyMain);
	mySched()->setStopTime(1);
	int i = Target::run();
	addCode(rts);
	return i;
}

Sub56Target::Sub56Target(const Sub56Target& arg) :
	CG56Target(arg)
{
	initStates();
	copyStates(arg);
}

int Sub56Target :: setup (Galaxy& g) {
	LOG_DEL; delete dirFullName;
	dirFullName = writeDirectoryName(dirName);
	if (!CG56Target::setup(g)) return FALSE;
	uname = (char*)g.readName();
	return TRUE;
}

void Sub56Target :: headerCode () {
	CG56Target :: headerCode();
	const char* path = expandPathName("~ptolemy/lib/cg56");
	StringList inc = "\tinclude '";
	inc += path;
	inc += "/intequlc.asm'\n\tinclude '";
	inc += path;
	inc += "/ioequlc.asm'\n";
	addCode(inc);
	addCode(
		"	org	p:\n"
		"ptolemyInit\n"
		"	movep	#0,x:m_bcr\n\n");
};

int Sub56Target :: genFile (StringList& stuff, const char* suffix) {
	int status;
	StringList bname = uname;
	bname += suffix;
	char* fullName = writeFileName(bname);
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
	LOG_DEL; delete fullName;
	return status;
}

Block* Sub56Target::clone() const {
	LOG_NEW; return new Sub56Target(*this);
}

void Sub56Target :: wrapup () {
 	inProgSection = TRUE;
	StringList map = mem->printMemMap(";","");
	addCode (map);
	CGTarget::wrapup();
// put the stuff into the files.
	if (!genFile(myCode, ".asm")) return;
// directive to change to the working directory
	StringList cd = "cd "; cd += dirFullName; cd += ";";
}

ISA_FUNC(Sub56Target,CG56Target);

//make an instance
static Sub56Target proto("sub-CG56","generate subroutines pigiInit & pigi Main",0,4096,0,4096);

static KnownTarget entry(proto, "sub-CG56");
