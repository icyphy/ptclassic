static const char file_id[] = "CGMacroStar.cc";

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

 Programmer: Soonhoi Ha 

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGMacroStar.h"
#include "Domain.h"
#include "Error.h"
#include "CGMacroCluster.h"

/*******************************************************************

	class CGMacroStar methods

********************************************************************/

// setProp: clone the portholes from the original cluster
// And set the profileIndex to indicate which inside processor is 
// associated with this star.
void CGMacroStar :: setProp(CGStar* s, int pix, int invoc, int flag) {

	profileIndex = pix;
	invocNum = invoc;

	if (pix < 0) {
		Error::abortRun(*s, "wrong profile index for CGMacroStar");
	}

	myCluster = (CGMacroClusterBag*) s;

	if (flag) {
		input.setPort("input", this, ANYTYPE);
		output.setPort("output", this, ANYTYPE);
		CGClustPortIter nextp(*myCluster);
		CGClustPort* p;
		PortHole * newP;
		while ((p = nextp++) != 0) {
			if (p->isItInput()) {
				newP = &input.newPort();
			} else {
				newP = &output.newPort();
			}
			CGClustPort* inp = p->inPtr();
			while (inp->inPtr()) inp = inp->inPtr();

			DFPortHole& ref = inp->real();
			newP->setPort(p->name(), this, ref.resolvedType(),
				      ref.numXfer());
	   	}
	}
}

void CGMacroStar :: initialize() {}

// redefine go() method to generate code for the inside macro actor
void CGMacroStar :: go() {
	myCluster->downLoadCode(this, invocNum, profileIndex);
}

int CGMacroStar :: run() {
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
