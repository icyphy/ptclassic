static const char file_id[] = "CGWormStar.cc";

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

 Programmer: Soonhoi Ha 
 Date of creation: 1/12/93

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGWormBase.h"
#include "CGWormStar.h"
#include "CGPortHole.h"
#include "Domain.h"
#include "ConstIters.h"
#include "Error.h"

/*******************************************************************

	class CGWormStar methods

********************************************************************/

// constructor, clone the portholes from the original wormhole
// And set the profileIndex to indicate which inside processor is 
// associated with this star.
CGWormStar :: CGWormStar(CGStar* s, int pix, int invoc, int flag): 
	profileIndex(pix), invocNum(invoc) {

	if (pix < 0) {
		Error::abortRun(*s, "wrong profile index for CGWormStar");
	}

	myStar = s;
	myWorm = s->myWormhole();

	if (flag) {
	   Domain* dom = Domain::of(*(s->parent()));
	   CBlockPortIter nextp(*s);
	   const PortHole* p;
	   while ((p = nextp++) != 0) {
		EventHorizon* eh;
		if (p->isItInput()) {
			eh = &dom->newTo();
			eh->setEventHorizon(in, p->name(), 0, this);
		} else {
			eh = &dom->newFrom();
			eh->setEventHorizon(out, p->name(), 0, this);
		}
		PortHole* newP = eh->asPort();
		addPort(newP->setPort(p->name(), this, p->resolvedType(),
				      p->numXfer()));
	   }
	}
}

CGWormStar :: ~CGWormStar() {
	deleteAllGenPorts();
}

void CGWormStar :: initialize() {}

// redefine go() method to generate code for the inside wormhole.
void CGWormStar :: go() {
	myWorm->downLoadCode(invocNum, profileIndex);
}

int CGWormStar :: run() {
	int status = Star :: run();
	if (numberPorts() > 0) {
		BlockPortIter nextPort(*myStar);
		CGPortHole* p;
		while ((p = (CGPortHole*) nextPort++) != 0) {
			p->advance();
		}
	}
	return status;
}
