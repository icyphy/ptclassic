static const char file_id[] = "S56XTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

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
	addState(monitorProg.setState("monitor",this,"","Loader/Monitor/Debugger"));
	monitorProg.setAttributes(A_SETTABLE|A_NONCONSTANT);

	// ROM tables in l:256-511, and p: external mem overlaps x:512-8191
	xMemMap.setInitValue("0-255,8192-16383");
	yMemMap.setInitValue("0-255,512-16383");
	xMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
	yMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
	runFlag.setInitValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	targetHost.setAttributes(A_SETTABLE|A_NONCONSTANT);
	addStream("aioCmds",&aioCmds);
	addStream("shellCmds",&shellCmds);
}

void S56XTarget :: headerCode () {
	aioCmds.initialize();
	shellCmds.initialize();
	CG56Target :: headerCode();
	const char *path = expandPathName("$PTOLEMY/lib/cg56/s56x.asm");
	myCode << "\tinclude '" << path << "'\n";
	interruptFlag = TRUE;
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
	StringList assembleCmds = "asm56000 -b -l -A -oso ";
	assembleCmds += filePrefix;
	if (systemCall(assembleCmds,"Errors in assembly")!=0)
		return FALSE;
	return TRUE;
}

void S56XTarget :: writeCode(const char* name) {
	if (name == NULL) name = filePrefix;
	/*
	 * generate .aio data file
	 */
	if (!genFile(aioCmds, name, ".aio")) {
	    Error::abortRun(*this,"Aio data file write failed");
	    return;
	}

	/*
	 * generate shell-cmd file (/bin/sh)
	 */
	const char *monprog = monitorProg;
	StringList realcmds = "#!/bin/sh\n";
	realcmds << headerComment("# ");
	if ( monprog==NULL || *monprog=='\0' ) {
	    realcmds << "load_s56x" << " '" << name << ".lod'\n" << shellCmds;
	} else {
	    realcmds << monprog << " '" << name << ".lod'\n" << shellCmds;
	}
	if (!genFile(realcmds,name)) {
	    Error::abortRun(*this,"Shell command file write failed");
	    return;
	}
	// make script executable
	// The chmod is bogus.  Should pass a mode to genFile when creating
	chmod(fullFileName(name),0755);

	/*
	 * generate the .asm file (and optionally display it)
	 */
	CG56Target :: writeCode(name);
}

int S56XTarget :: runCode() {
	StringList runCmd;
	runCmd << filePrefix << " &";
	if (systemCall(runCmd,"Problems running code onto S56X",targetHost)!=0)
	    return FALSE;
	return TRUE;
}


ISA_FUNC(S56XTarget,CG56Target);

// make an instance
static S56XTarget proto("S-56X","run code on the S-56X card");

static KnownTarget entry(proto, "S-56X");
