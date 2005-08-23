static const char file_id[] = "S56XTarget.cc";
/******************************************************************
Version identification:
@(#)S56XTarget.cc	1.40 07/30/96

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
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

#include "CGCXSynchComm.h"
#include "CG56XCSynchComm.h"
#include "CG56XCReceive.h"
#include "CG56XCSend.h"
#include "CGCXSend.h"
#include "CGCXReceive.h"
#include "CGCXPeek.h"
#include "CGCXPoke.h"
#include "CG56XCPeek.h"
#include "CG56XCPoke.h"

S56XTarget :: S56XTarget(const char* nam, const char* desc,
			 const char* assocDomain) :
CG56Target(nam, desc, assocDomain),
MotorolaTarget(nam, desc, "CG56Star", assocDomain),
pairNumber(0) {
	initStates();
}

S56XTarget::S56XTarget(const S56XTarget& arg) :
CG56Target(arg),MotorolaTarget(arg),pairNumber(0) {
	initStates();
	copyStates(arg);
}

void S56XTarget::setup() {
    // Keeps track of how many send/receive communication pairs we create
    CG56Target::setup();
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
	char *path = expandPathName("$PTOLEMY/lib/cg56/s56x.asm");
	myCode << "\tinclude '" << path << "'\n";
	interruptFlag = TRUE;
	delete [] path;
};

Block* S56XTarget::makeNew() const {
	LOG_NEW; return new S56XTarget(*this);
}

void S56XTarget :: trailerCode () {
	CG56Target::trailerCode();
	trailer << "\tjmp\tERROR\n";
}

void S56XTarget :: writeCode() {
    /*
     * generate .aio data file
     */
    if (!parent() && aioCmds.numPieces() > 0) {
	if (!writeFile(aioCmds,".aio")) {
	    Error::abortRun(*this,"Aio data file write failed");
	    return;
	}
    }
    /*
     * generate shell-cmd file (/bin/sh)
     */
    if (!parent()) {
	const char *monprog = monitorProg;
    	StringList realcmds = "#!/bin/sh\n";
    	realcmds << headerComment("# ");
    	if ( monprog==NULL || *monprog=='\0' ) {
	    realcmds << "load_s56x" << " '" << filePrefix << ".lod'\n"
		     << shellCmds;
    	}
	else {		
	    realcmds << monprog << " '" << filePrefix << ".lod'\n"
		     << shellCmds;
	}	
    	if (!writeFile(realcmds,"",FALSE,0755)) {
	    Error::abortRun(*this,"Shell command file write failed");
		return;
	}
    }
    /*
     * generate the .asm file (and optionally display it)
     */
    CG56Target :: writeCode();
}

int S56XTarget :: runCode() {
	StringList runCmd;
	runCmd << "./" << filePrefix << " &";
	if (systemCall(runCmd,"Problems running code onto S56X",targetHost)!=0)
	    return FALSE;
	return TRUE;
}


ISA_FUNC(S56XTarget,CG56Target);

void S56XTarget::configureCommPair(CommPair& pair) {
    CGCXSynchComm* cgcSide = (CGCXSynchComm*) pair.cgcStar;
    CG56XCSynchComm* s56xSide = (CG56XCSynchComm*) pair.cgStar;
    cgcSide->s56xSide = s56xSide;
    s56xSide->cgcSide = cgcSide;
    s56xSide->commCount = cgcSide->commCount = &pairNumber;
    s56xSide->pairNumber = cgcSide->pairNumber = pairNumber++;

    cgcSide->S56XFilePrefix.
	setInitValue(hashstring(filePrefix.currentValue()));
    cgcSide->S56XFilePrefix.initialize();

}

CommPair S56XTarget::fromCGC(PortHole&) {
    CommPair pair(new CGCXSend,new CG56XCReceive);
    configureCommPair(pair);
    return pair;
}

CommPair S56XTarget::toCGC(PortHole&) {
    CommPair pair(new CGCXReceive,new CG56XCSend);
    configureCommPair(pair);
    return pair;
}

AsynchCommPair S56XTarget::createPeekPoke(CGTarget& peekTarget,
					  CGTarget& pokeTarget) {
    
    AsynchCommPair pair;
    CGCXBase* cgcSide;
    if (peekTarget.isA("CGCTarget")) {
	cgcSide = new CGCXPeek;
	pair.set(cgcSide, new CG56XCPoke);
    }
    else if (pokeTarget.isA("CGCTarget")) {
	cgcSide = new CGCXPoke;
	pair.set(new CG56XCPeek, cgcSide);
    }
    else {
	StringList message;
	message << "Unable to make a peek/poke pair for " << peekTarget.name()
		<< " to " << pokeTarget.name();
	Error::abortRun(*this,message);
	return pair;
    }

    cgcSide->S56XFilePrefix.
	setInitValue(hashstring(filePrefix.currentValue()));
    cgcSide->S56XFilePrefix.initialize();

    StringList aioVariable;
    aioVariable << symbol("peekPoke");
    setAsynchCommState(pair,"VariableName",aioVariable);
    return pair;
}

// make an instance
static S56XTarget proto("S-56X","run code on the S-56X card");

static KnownTarget entry(proto, "S-56X");
