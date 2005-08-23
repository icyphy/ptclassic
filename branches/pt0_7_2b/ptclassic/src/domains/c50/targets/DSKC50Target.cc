static const char file_id[] = "DSKC50Target.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: A. Baensch
 Date of creation: 30 June 1995

 Target for TI TMS320C5x DSP board.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DSKC50Target.h"
#include "KnownTarget.h"
#include "TITarget.h"

DSKC50Target :: DSKC50Target(const char* nam, const char* desc) :
	C50Target(nam,desc),TITarget(nam,desc,"C50Star") {
	initStates();
}

DSKC50Target::DSKC50Target(const DSKC50Target& arg) :
	C50Target(arg),TITarget(arg) {
	initStates();
	copyStates(arg);
}

void DSKC50Target :: initStates() {

	// ROM tables: Block B1 512 words and User Data (u) 8704 words
	bMemMap.setInitValue("768-1279");
	uMemMap.setInitValue("2432-9999");
	bMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
	uMemMap.setAttributes(A_NONSETTABLE|A_NONCONSTANT);
	runFlag.setInitValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	targetHost.setAttributes(A_SETTABLE|A_NONCONSTANT);
	addStream("shellCmds",&shellCmds);
}

void DSKC50Target :: headerCode () {
	shellCmds.initialize();
	myCode 	<< "\t.ps	00808h\n"
	   	<< "\tB	TINT		; set timer interrupt vector\n"
  	   	<< "\tB	RINT		; set receive interrupt vector\n"
	   	<< "\tB	XINT		; set transmit interrupt vector\n";
	C50Target :: headerCode();
	interruptFlag = TRUE;
}

Block* DSKC50Target::makeNew() const {
	LOG_NEW; return new DSKC50Target(*this);
}

void DSKC50Target :: trailerCode () {
	C50Target::trailerCode();
}

int DSKC50Target :: compileCode() {
	StringList assembleCmds;
	assembleCmds << "assembl " << filePrefix << ".asm";
	if (systemCall(assembleCmds,"Errors in assembly")!=0)
		return FALSE;
	return TRUE;
}

// generate the .asm file (and optionally display it)
void DSKC50Target :: writeCode() {
    C50Target :: writeCode();
}

int DSKC50Target :: runCode() {
	StringList runCmd;
	runCmd << "loader " << filePrefix << ".dsk";
	if ( systemCall(runCmd,
			"Problems running code onto TMS320C5x",
			targetHost) != 0 )
	    return FALSE;
	return TRUE;
}

void DSKC50Target::frameCode() {
    TITarget::frameCode();
}

ISA_FUNC(DSKC50Target, C50Target);

// make an instance
static DSKC50Target proto("DSKC50", "run code on the DSK TMS320C5x card");
