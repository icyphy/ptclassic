static const char file_id[] = "BDFStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck
 Date of creation: 1/8/91

*******************************************************************/

#include "BDFStar.h"
#include "BDFPortHole.h"
#include "SimControl.h"

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

	// step 1: process unconditional inputs.

	while ((p = nextPort++) != 0) {
		if (p->isItOutput() || p->isDynamic()) continue;
		if (p->far() && p->numTokens() < p->numXfer())
			return handleWait(*p);
		else p->receiveData();
	}

	// step 2: process conditional inputs.  Assumption: control
	// ports always move one integer, and previous processing
	// in step 1 has fetched the control token.

	DFPortHole* ctlp = 0;
	nextPort.reset();
	while ((p = nextPort++) != 0) {
		if (p->isItOutput() || !p->isDynamic()) continue;
		ctlp = p->assoc();
		int ctl_is_t = (*ctlp)%0;
		int read_on_t = (p->assocRelation() == BDF_TRUE);
		// data are moved if control token matches the
		// assocRelation.
		if (ctl_is_t == read_on_t) {
			if (p->far() && p->numTokens() < p->numXfer())
				return handleWait(*p);
			else p->receiveData();
		}
	}

	// leave a waitPort for next time if there was a control port.
	if (ctlp) waitFor(*ctlp);

	// step 3: execute the star
	if (!Star::run()) return FALSE;

	// step 4: do sendData calls for conditional outputs.
	// we do conditionals first so that we can still access any
	// output control booleans.

	nextPort.reset();
	while ((p = nextPort++) != 0) {
		if (p->isItInput() || !p->isDynamic()) continue;
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
