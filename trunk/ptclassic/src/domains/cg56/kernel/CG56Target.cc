static const char file_id[] = "CG56Target.cc";
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

 Programmer: J. Buck and J. Pino

 Base target for Motorola 56000 assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CG56Target.h"
#include "CG56Star.h"
#include "FixState.h"

int CG56Target :: compileCode() {
        StringList assembleCmds = "asm56000 -b -l -A -oso ";
        assembleCmds += filePrefix;
        if (systemCall(assembleCmds,"Errors in assembly")!=0)
                return FALSE;
        return TRUE;
}

void CG56Target :: headerCode () {
    MotorolaTarget::headerCode();
    const char* path = expandPathName("$PTOLEMY/lib/cg56");
    myCode << "\tinclude '" << path << "/intequlc.asm'\n\tinclude '"
	   << path << "/ioequlc.asm'\n";
}

void CG56Target :: setup() {
	Galaxy* g = galaxy();
	if (g && (g->stateWithName("ONE") == 0)) {
		LOG_NEW; FixState& ONE = *new FixState;
		g->addState(ONE.setState("ONE",this,"",
					"Max Fix point value",
					A_NONSETTABLE|A_CONSTANT));
		ONE.setInitValue(CG56_ONE);
	}
	MotorolaTarget :: setup();
}

// makeNew
Block* CG56Target :: makeNew () const {
	LOG_NEW; return new CG56Target(*this);
}

void CG56Target::writeFloat(double val) {
	myCode << "; WARNING: the M56000 does not support floating point!\n";
	myCode << "; perhaps this state was meant to be type FIX?\n";
	MotorolaTarget::writeFloat(val);
}

const char* CG56Target::className() const { return "CG56Target";}
