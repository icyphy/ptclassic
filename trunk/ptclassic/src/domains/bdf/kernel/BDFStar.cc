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

/*******************************************************************

	class BDFStar methods

********************************************************************/

BDFStar :: BDFStar() 
: sdf(0), sdfCtx(0) {}

int BDFStar :: isSDF() const { return sdf;}
int BDFStar :: isSDFinContext() const { return sdfCtx;}

void BDFStar :: initialize() {
	DataFlowStar::initialize();
	// determine SDF-ness
	BDFStarPortIter nextp(*this);
	BDFPortHole *p;
	sdf = TRUE;
	while ((p = nextp++) != 0) {
		if (TorF(p->relType())) sdf = FALSE;
	}
	sdfCtx = sdf ? TRUE : dataIndependent();
}

// the baseclass has no additional smarts for determining data-independence.
int BDFStar::dataIndependent() { return sdf;}

// The following is defined in BDFDomain.cc -- this forces that module
// to be included if any BDF stars are linked in.
extern const char BDFdomainName[];

const char* BDFStar :: domain () const { return BDFdomainName;}

ISA_FUNC(BDFStar,DataFlowStar);

inline int wormEdge(PortHole& p) {
	PortHole* f = p.far();
	if (!f) return TRUE;
	else return (p.isItInput() == f->isItInput());
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



