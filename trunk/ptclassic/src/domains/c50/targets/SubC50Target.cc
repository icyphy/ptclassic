static const char file_id[] = "SubC50Target.cc";
/******************************************************************
Version identification:
@(#)SubC50Target.cc	1.6	8/15/96

@Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer: Andreas Baensch
 Date of creation: 8 May 1995

 Based on code by J. Pino.

 Target for TI 320C5x assembly code generation that generates 
 two subroutines, ptolemyInit, ptolemyMain.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SubC50Target.h"
#include "KnownTarget.h"
#include "TITarget.h"

SubC50Target :: SubC50Target(const char* nam, const char* desc,
			     const char* assocDomain) :
C50Target(nam,desc,assocDomain),TITarget(nam,desc,"C50Star",assocDomain)
{
	initStates();
}

SubC50Target::SubC50Target(const SubC50Target& arg) :
C50Target(arg),TITarget(arg)
{
	initStates();
	copyStates(arg);
}

void SubC50Target :: initStates() {
	bMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
	uMemMap.setAttributes(A_SETTABLE|A_NONCONSTANT);
}

/*virtual*/ void SubC50Target :: mainLoopCode() {
	myCode << "\tret\n" << "ptolemyMain\n";
	scheduler()->compileRun();
	myCode << "\tret\n";
}

void SubC50Target :: headerCode () {
	myCode << ".text\n" << "ptolemyInit\n";
	C50Target :: headerCode();
};

 
Block* SubC50Target::makeNew() const {
	LOG_NEW; return new SubC50Target(*this);
}

ISA_FUNC(SubC50Target, C50Target);

// Register an instance
static SubC50Target proto("sub-C50",
			  "generate subroutines pigiInit and pigiMain");

static KnownTarget entry(proto, "sub-C50");
