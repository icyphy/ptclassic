 static const char file_id[] = "C50Target.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

Programmer: Andreas Baensch
Date of creation: 30 June 1995

Modeled after code by J. Buck and J. Pino.

Base target for TI 320C5x assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "C50Target.h"
#include "C50Star.h"
#include "FixState.h"

int C50Target :: compileCode() {
        StringList assembleCmds;
        assembleCmds << "assembl " << filePrefix << ".asm";
        if (systemCall(assembleCmds,"Errors in assembly")!=0)
                return FALSE;
        return TRUE;
}

void C50Target :: headerCode () {
    TITarget::headerCode();
    myCode << "\t.mmregs\t		; Include memory map reg\n"
	   << "\t.ps	00a00h		; initialize PC\n"
	   << "\t.entry\n"
	   << "\t.ds	0030h\n"
	   << "COUNT	.set	30h\n";
    TITarget::disableInterrupts();	   
    myCode<< "START	setc	SXM		; set sign extension mode\n"
	   << "\tldp	#0		; set data page pointer\n"
	   << "\tsplk	#830h,PMST	; 9K on.chip RAM as Data, no ROM\n"
	   << "\tlacl	#0\n"
	   << "\tsamm	CWSR		; set Wait State Control Register\n"
	   << "\tsamm	PDWSR		; for 0 waits in pgm & data memory\n"
	   << "\tclrc	OVM		\n";
}


void C50Target :: setup() {
	Galaxy* g = galaxy();
	addC50One(this,g);
	TITarget :: setup();
}

void addC50One(Target* target,Galaxy* g) {
	if (g && (g->stateWithName("ONE") == 0)) {
		LOG_NEW; FixState& ONE = *new FixState;
		g->addState(ONE.setState("ONE",target,"",
					"Max Fix point value",
					A_NONSETTABLE|A_CONSTANT));
		ONE.setInitValue(C50_ONE);
	}
}

// makeNew
Block* C50Target :: makeNew () const {
	LOG_NEW; return new C50Target(*this);
}

void C50Target::writeFloat(double val) {
	myCode << "* WARNING: the TMS320C5x does not support floating point!\n";
	myCode << "* perhaps this state was meant to be type FIX?\n";
	TITarget::writeFloat(val);
}

const char* C50Target::className() const { return "C50Target";}

ISA_FUNC(C50Target,TITarget);
