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
#include "CGUtilities.h"
#include "miscFuncs.h"

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


