static const char file_id[] = "AsmTargetInit.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 This is the star/state initialization function for AsmTarget.
 It is kept separate because lots of include files not needed for
 the rest of AsmTarget are referenced here.

*******************************************************************/

#include "AsmTarget.h"
#include "AsmStar.h"
#include "IntState.h"
#include "FloatState.h"
#include "FixState.h"
#include "IntArrayState.h"
#include "FloatArrayState.h"
#include "FixArrayState.h"
#include "ProcMemory.h"

// code to do initialiation for a star

void AsmTarget::doInitialization(CGStar& cgStar) {
	// Cast to AsmStar
	AsmStar &star = (AsmStar&)cgStar;
	BlockStateIter nextState(star);
	State* s;
        StringList comment = "initialization code from star ";
        comment += star.readFullName();
        comment += " (class ";
        comment += star.readClassName();
        comment += ")\n";
        outputComment(comment);
	while ((s = nextState++) != 0) {
		if ((s->attributes() & AB_MEMORY) == 0 ||
		    (s->attributes() & AB_NOINIT) != 0) continue;
		// too bad -- switching on state types
		StringList smsg = "initialization for state ";
		smsg += s->readFullName();
		outputComment (smsg);
		unsigned addr;
		ProcMemory *mem = star.lookupEntry(s->readName(),addr);
		orgDirective(mem->readName(), addr);

		// handle A_REVERSE attribute, which makes arrays
		// write out backwards.

		int sIdx = 0;
		int limit = s->size();
		int step = 1;

		if ((s->attributes() & AB_REVERSE) != 0) {
			sIdx = limit - 1;
			limit = -1;
			step = -1;
		}
		if (strcmp(s->type(), "INT") == 0) {
			IntState* i = (IntState*)s;
			writeInt(int(*i));
		}
		else if (strcmp(s->type(), "INTARRAY") == 0) {
			IntArrayState* i = (IntArrayState*)s;
			for (int j = sIdx; j != limit; j += step)
				writeInt((*i)[j]);
		}
		else if (strcmp(s->type(), "FIX") == 0) {
			FixState* i = (FixState*)s;
			writeFix(double(*i));
		}
		else if (strcmp(s->type(), "FIXARRAY") == 0) {
			FixArrayState* i = (FixArrayState*)s;
			for (int j = sIdx; j != limit; j += step)
				writeFix((*i)[j]);
		}
		else if (strcmp(s->type(), "FLOAT") == 0) {
			FloatState* i = (FloatState*)s;
			writeFloat(double(*i));
		}
		else if (strcmp(s->type(), "FLOATARRAY") == 0) {
			FloatArrayState* i = (FloatArrayState*)s;
			for (int j = sIdx; j != limit; j += step)
				writeFloat((*i)[j]);
		}
		else {
			Error::abortRun (*this, "can't write state of type ",
					 s->type());
		}
	}
	codeSection();
	cgStar.initCode();
}

