static const char file_id[] = "CGCMacroStar.cc";

/******************************************************************
Version identification:
@(#)CGCMacroStar.cc	1.3 04/07/97

@Copyright (c) 1995-1997 The Regents of the University of California.
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

 Programmer: Soonhoi Ha 

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCMacroStar.h"
#include "CGPortHole.h"
#include "Domain.h"
#include "ConstIters.h"
#include "Error.h"
#include "CGMacroCluster.h"

/*******************************************************************

	class CGCMacroStar methods

********************************************************************/

// constructor, clone the portholes from the original cluster
// And set the profileIndex to indicate which inside processor is 
// associated with this star.
void CGCMacroStar :: setProp(CGStar* s, int pix, int invoc, int flag) {
	profileIndex = pix;
	invocNum = invoc;

	if (pix < 0) {
		Error::abortRun(*s, "wrong profile index for CGCMacroStar");
	}

	myCluster = (CGMacroClusterBag*) s;

	if (flag) {
		input.setPort("input", this, FLOAT);
		output.setPort("output", this, FLOAT);
		CGMacroClustPortIter nextp(*myCluster);
		CGMacroClustPort* p;
		PortHole * newP;
		while ((p = nextp++) != 0) {
			if (p->isItInput()) {
				newP = &input.newPort();
			} else {
				newP = &output.newPort();
			}
			DFPortHole& ref = p->inPtr()->real();
			DataType dt = ref.resolvedType();
			if (!dt) dt = ANYTYPE;
			newP->setPort(p->name(), this, dt, ref.numXfer());
	   	}
	}
}

// void CGCMacroStar :: initialize() {}

// redefine go() method to generate code for the inside wormhole.
void CGCMacroStar :: go() {
	myCluster->downLoadCode(this, invocNum, profileIndex);
}

int CGCMacroStar :: run() {
	int status = Star :: run();
	if (numberPorts() > 0) {
		BlockPortIter nextPort(*this);
		CGPortHole* p;
		while ((p = (CGPortHole*) nextPort++) != 0) {
			p->advance();
		}
	}
	return status;
}
