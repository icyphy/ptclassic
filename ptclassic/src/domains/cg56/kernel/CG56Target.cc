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

StringList CG56ONE = double(1.0 - 1.0/double(1<<23));

void CG56Target :: headerCode () {
        MotorolaTarget::headerCode();
	const char* path = expandPathName("~ptolemy/lib/cg56");
	StringList inc = "\tinclude '";
	inc += path;
	inc += "/intequlc.asm'\n\tinclude '";
	inc += path;
	inc += "/ioequlc.asm'\n";
	addCode(inc);
}

int CG56Target :: setup(Galaxy& g) {
	g.addState(ONE.setState("ONE",this,CG56ONE,
		"Max Fix point value",A_NONSETTABLE|A_CONSTANT));
	return MotorolaTarget :: setup(g);
}

// clone
Block* CG56Target :: clone () const {
	LOG_NEW; return new CG56Target(*this);
}

void CG56Target::writeFloat(double val) {
	StringList out = "; WARNING: the M56000 does not support floating point!\n";
	out += "; perhaps this state was meant to be type FIX?\n";
	addCode(out);
	MotorolaTarget::writeFloat(val);
}


