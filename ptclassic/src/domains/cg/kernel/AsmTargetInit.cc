static const char file_id[] = "AsmTargetInit.cc";
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
        StringList msg = "initialization code from star ";
	msg << star.fullName() << " (class " << star.className() << ")\n";
        myCode << comment(msg);
	while ((s = nextState++) != 0) {
		if ((s->attributes() & AB_MEMORY) == 0 ||
		    (s->attributes() & AB_NOINIT) != 0) continue;
		// too bad -- switching on state types
		StringList smsg;
		smsg << "initialization for state " << s->fullName();
		myCode << comment(smsg);
		unsigned addr;
		ProcMemory *mem = star.lookupEntry(s->name(),addr);
		orgDirective(mem->name(), addr);

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

