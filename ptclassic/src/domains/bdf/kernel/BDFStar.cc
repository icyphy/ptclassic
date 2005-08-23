static const char file_id[] = "BDFStar.cc";
/******************************************************************
Version identification:
@(#)BDFStar.cc	2.14	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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

 Programmer: J. T. Buck
 Date of creation: 1/8/91

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "BDFStar.h"
#include "BDFPortHole.h"
#include "SimControl.h"

// an aid for debugging: make animation behavior look like DDF stars
// in which partial evaluation occurs.  Problem: this file now "knows"
// the format of the animation info.

#include "textAnimate.h"
#include <iostream.h>

/*******************************************************************

	class BDFStar methods

********************************************************************/

BDFStar :: BDFStar() : sdfCtx(0), dynExec(0) {}

int BDFStar :: isSDFinContext() const { return sdfCtx;}

// enable dynamic execution
int BDFStar :: setDynamicExecution(int val) {
	dynExec = val ? TRUE : FALSE;
	return TRUE;
}

// in addition to baseclass initialize, set waitPort if we have a
// dynamic input.
void BDFStar :: initialize() {
	DynDFStar::initialize();
	if (!isSDF() && !isSink() && dynamicExec()) {
		// find waitPort
		BDFStarPortIter nextp(*this);
		BDFPortHole *p;

		while ((p = nextp++) != 0) {
			p->clearReadFlag();
			if (p->isDynamic() && p->isItInput()) {
				waitFor(*p->assoc());
			}
		}
	}
	sdfCtx = isSDF() ? TRUE : dataIndependent();
}

// the baseclass has no additional smarts for determining data-independence.
int BDFStar::dataIndependent() { return isSDF();}

// The following is defined in BDFDomain.cc -- this forces that module
// to be included if any BDF stars are linked in.
extern const char BDFdomainName[];

const char* BDFStar :: domain () const { return BDFdomainName;}

ISA_FUNC(BDFStar,DynDFStar);

// method called when an input port does not have enough data.
// If we are being executed dynamically, we must make sure that there
// is enough data (since the DDF scheduler will execute us based on
// the waitPort).  Otherwise it is an error if there is not enough data.

int BDFStar :: handleWait(BDFPortHole& p) {
	if (dynamicExec()) {
		if (textAnimationState()) {
			cout << "PARTIAL: " << fullName() << ": wait on "
			     << p.name() << "\n";
		}
		waitFor(p,p.numXfer());
		return TRUE;
	}
	else {
		Error::abortRun(p, "BDF scheduler error: too few tokens");
		return FALSE;
	}
}

// run function: This does any required receiveData or sendData calls.
// if SDF, we can use the simpler routine in DataFlowStar::run.

int BDFStar :: run () {
	if (isSDF()) return DataFlowStar::run();
	// not SDF.
	if (SimControl::haltRequested()) return FALSE;

	BDFStarPortIter nextPort(*this);
	BDFPortHole *p;

	// step 1: process unconditional inputs that haven't already
	// been read.

	while ((p = nextPort++) != 0) {
		if (p->isItOutput() || p->isDynamic() ||
		    p->alreadyRead()) continue;
		if (p->far() && p->numTokens() < p->numXfer())
			return handleWait(*p);
		else
			p->receiveData();
	}

	// step 2: process conditional inputs.  Assumption: control
	// ports always move one integer, and previous processing
	// in step 1 has fetched the control token.
	// again, we skip ports that have already been read.

	DFPortHole* ctlp = 0;
	nextPort.reset();
	while ((p = nextPort++) != 0) {
		if (p->isItOutput() || !p->isDynamic() ||
		    p->alreadyRead()) continue;
		ctlp = p->assoc();
		int ctl_is_t = (*ctlp)%0;
		int read_on_t = (p->assocRelation() == BDF_TRUE);
		// data are moved if control token matches the
		// assocRelation.
		if (ctl_is_t == read_on_t) {
			if (p->far() && p->numTokens() < p->numXfer())
				return handleWait(*p);
			else
				p->receiveData();
		}
	}

	// leave a waitPort for next time if there was a control port.
	if (ctlp) waitFor(*ctlp);

	// step 3: execute the star
	if (!Star::run()) return FALSE;

	// step 4: do sendData calls for conditional outputs.
	// we do conditionals first so that we can still access any
	// output control booleans.
	// while we are at it, the read flags of input ph's are cleared.

	nextPort.reset();
	while ((p = nextPort++) != 0) {
		if (p->isItInput() || !p->isDynamic()) {
			p->clearReadFlag();
			continue;
		}
		ctlp = p->assoc();
		int ctl_is_t = (*ctlp)%0;
		int write_on_t = (p->assocRelation() == BDF_TRUE);
		if (ctl_is_t == write_on_t)
			p->sendData();
	}

	// step 5: do sendData calls for unconditional outputs.

	nextPort.reset();
	while ((p = nextPort++) != 0) {
		if (p->isItOutput() && !p->isDynamic())
			p->sendData();
	}
	return TRUE;
}

int BDFStar :: notRunnable () {
	if (isSDFinContext()) {
		return DataFlowStar::notRunnable();
	}
	else {
		Error::abortRun(*this, "fn notRunnable, not SDFinContext");
		return 0;
	}
}

int BDFStar :: simRunStar (int deferFiring) {
	if (isSDFinContext()) {
		return DataFlowStar::simRunStar(deferFiring);
	}
	else {
		Error::abortRun(*this, "fn simRunStar, not SDFinContext");
		return 0;
	}
}

	///////////////////////////
	// deferrable
	///////////////////////////

// return TRUE if we can defer the star, FALSE if we cannot.
// Postpone any execution of a star feeding data to another
// star that is runnable.  Also postpone if each output port
// has enough data on it to satisfy destination stars.

int BDFStar :: deferrable () {
	if (isSDFinContext()) {
		return DataFlowStar::deferrable();
	}
	else {
		Error::abortRun(*this, "fn deferrable: not SDFinContext");
		return 0;
	}
}
