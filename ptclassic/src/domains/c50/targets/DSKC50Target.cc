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

DSKC50Target :: DSKC50Target(const char* nam, const char* desc,
			     const char* assocDomain) :
C50Target(nam,desc,assocDomain),
TITarget(nam,desc,"C50Star", assocDomain) {
	initStates();
	addStream("shellCmds",&shellCmds);
}

DSKC50Target::DSKC50Target(const DSKC50Target& arg) :
C50Target(arg),
TITarget(arg) {
	initStates();
	copyStates(arg);
	addStream("shellCmds",&shellCmds);
}

void DSKC50Target :: initStates() {
	// ROM tables: Block B1 512 words and User Data (u) 8704 words
	bMemMap.setInitValue("768-1279");
	uMemMap.setInitValue("2432-6848");
	// the u and b memory maps should be settable since
	// u/b memory are the same as far as teh assembler is
	// concerned so if there's not enough room in one
	// the program can use the other
	// FIXME: Should this be automated?
	bMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
	uMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
	runFlag.setInitValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	compileFlag.setInitValue("YES");
	compileFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
	targetHost.setAttributes(A_SETTABLE|A_NONCONSTANT);
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
  	assembleCmds << "assembl " << filePrefix << "DSK.asm";
  	if (systemCall(assembleCmds,"Errors in assembly")!=0)
  		return FALSE;
	return TRUE;
}

// generate the C50 assembler .asm file and truncate its lines to 80
// characters due to limitations in the DSK assembler
void DSKC50Target :: writeCode() {
	C50Target :: writeCode();

	// DSK assembler doesn't seem to like lines longer than 80 characters
	// so make a new file that's the original file with each line truncated
	// to 80 characters.
	StringList postProcessCmd;
	postProcessCmd.initialize();
	postProcessCmd << "cut -c 1-79 " << filePrefix<< ".asm > "
		       << filePrefix << "DSK.asm";
	systemCall(postProcessCmd,
		   "Post-processing of the C50 assembler file failed,\nbut the code might still compile");

	// Compile the code if requested. Note that even if compilation is not
	// wanted the user might want to keep *DSK.asm file to use it later so
	// the compile method should be called but the compilation not executed
}

int DSKC50Target :: runCode() {
	StringList runCmd;
	runCmd << "loader " << filePrefix << "DSK.dsk";
	return ( systemCall(runCmd,
			    "Problems running code onto TMS320C5x",
			    targetHost) == 0 );
}

void DSKC50Target::frameCode() {
	TITarget::frameCode();
}

ISA_FUNC(DSKC50Target, C50Target);

// Register an instance
static DSKC50Target proto("DSKC50", "run code on the DSK TMS320C5x card");

static KnownTarget entry(proto, "DSKC50");
