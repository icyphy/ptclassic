static const char file_id[] = "Sub56Target.cc";
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

 Target for Motorola 56000 assembly code generation that generates 
 two subroutines, ptolemyInit, ptolemyMain.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Sub56Target.h"
#include "KnownTarget.h"
#include "MotorolaTarget.h"

Sub56Target :: Sub56Target(const char* nam, const char* desc) :
	CG56Target(nam,desc),MotorolaTarget(nam,desc,"CG56Star")
{
	initStates();
}

Sub56Target::Sub56Target(const Sub56Target& arg) :
	CG56Target(arg),MotorolaTarget(arg)
{
	initStates();
	copyStates(arg);
}

void Sub56Target :: initStates() {
	xMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
	yMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
}

/*virtual*/ void Sub56Target :: mainLoopCode() {
	myCode << "	rts\n" << "ptolemyMain\n";
	scheduler()->compileRun();
	myCode << "	rts\n";
}

void Sub56Target :: headerCode () {
	myCode << "	org p:\n" << "ptolemyInit\n";
	CG56Target :: headerCode();
};

 
Block* Sub56Target::makeNew() const {
	LOG_NEW; return new Sub56Target(*this);
}

ISA_FUNC(Sub56Target,CG56Target);

//make an instance
static Sub56Target proto("sub-CG56","generate subroutines pigiInit and pigiMain");

static KnownTarget entry(proto, "sub-CG56");
