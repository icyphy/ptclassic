static const char file_id[] = "CG56Target.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck and J. Pino

 Base target for Motorola 56000 assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CG56Target.h"
#include "CG56Star.h"
#include "FixState.h"

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
