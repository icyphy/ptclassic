static const char file_id[] = "AsmTargetInit.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
#include "AsmPortHole.h"

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
                StringList smsg;
		if ((s->attributes() & AB_SHARED) != 0) {
			if (lookupSharedState(*s) == s) {
				smsg << "initialization for global state "
				     << s->name();
			}
			else {
				continue; // global state only initialize once
			}
		}
		else {
			smsg << "initialization for state " << s->fullName();
		}
		myCode << comment(smsg);
		unsigned addr;
		ProcMemory *mem = star.lookupEntry(s->name(),addr);
		if (mem==NULL) {
			Error::abortRun(*this,"No memory allocated for ",
					s->fullName());
			return;
		}
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
			writeInt(*i);
		}
		else if (strcmp(s->type(), "INTARRAY") == 0) {
			IntArrayState* i = (IntArrayState*)s;
			for (int j = sIdx; j != limit; j += step)
				writeInt((*i)[j]);
		}
		else if (strcmp(s->type(), "FIX") == 0) {
			FixState* i = (FixState*)s;
			writeFix(i->asDouble());
		}
		else if (strcmp(s->type(), "FIXARRAY") == 0) {
			FixArrayState* i = (FixArrayState*)s;
			for (int j = sIdx; j != limit; j += step)
				writeFix((*i)[j]);
		}
		else if (strcmp(s->type(), "FLOAT") == 0) {
			FloatState* i = (FloatState*)s;
			writeFloat(*i);
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
	AsmPortHole* p;
	AsmStarPortIter nextPort(star);
	while ((p = nextPort++) != 0) {
		StringList pmsg;
		pmsg.initialize();
                if (p->isItOutput() && p->far()->isItInput()) continue;
		if (!p->bufSize() ) continue;
		if ( (p->attributes() & PB_NOINIT) || 
		     (p->far()->attributes() & PB_NOINIT)) continue;
		pmsg << "initialization for porthole " << p->fullName();
		myCode << comment(pmsg);
                orgDirective(p->memory()->name(), p->baseAddr());
		for (int i=0 ; i < p->bufSize() ; i++ ) {
			DataType pType = p->setResolvedType();
			if (!pType) return;
			if (strcmp(pType,INT)==0)  writeInt(0);
			else if (strcmp(pType,FIX)==0) writeFix(0);
			else writeFloat(0);
		}
	}
	codeSection();
	cgStar.initCode();
}

