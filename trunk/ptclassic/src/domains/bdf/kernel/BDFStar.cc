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
	int sdf = TRUE;
	int sdfCtx = TRUE;
	while ((p = nextp++) != 0) {
		if (TorF(p->relType())) {
			sdf = FALSE;
			if (p->far() && !p->atBoundary()) sdfCtx = FALSE;
		}
	}
}

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

	////////////////////////////
	// notRunnable
	////////////////////////////

// Return 0 if the star is can be run now,
// 1 if it can't be run now, but should be runnable later (it has not
// reached its designated number of iterations), and 2 if it
// has been run as often as it needs to be.
// It is assumed that the denominator of the
// repetitions member is unity (as it should be).

// FIXME

int BDFStar :: notRunnable () {

	BlockPortIter nextp(*this);
	BDFPortHole *p;
	// Check to see whether the requisite repetitions have been met.
	if (reps() <= int(noTimes))
		return 2;

	// Step through all the input ports, checking to see whether
	// there is enough data on the inputs
	while ((p = (BDFPortHole*)nextp++) != 0) {
		// worm edges always have enough data
		if (wormEdge(*p)) continue;
		if (p->isItInput() && p->numTokens() < p->numberTokens)
			// not enough data
			return 1;
	}
	return 0;
}

	////////////////////////////
	// simRunStar
	////////////////////////////

// FIXME
int BDFStar :: simRunStar (int deferFiring) {

	int test = notRunnable();
	if(test) return test;	// return if the star cannot be run

	// If we get to this point without returning, then the star can be run.

	int i;
	BDFPortHole* port;

	// An important optimization for code generation:
	// Postpone any execution of a star feeding data to another
	// star that is runnable.  Also postpone if each output port
	// has enough data on it to satisfy destination stars.
	// This is optional because it slows down the scheduling.

	if(deferFiring && deferrable())
		return 3;

	// Iterate over the ports again to adjust
	// the number of tokens on each arc.
	// Note that this should work equally well if there are no inputs.

	BlockPortIter nextp(*this);
	for(i = numberPorts(); i>0; i--) {

		// Look at the next port in the list
		port = (BDFPortHole*)nextp++;

		// On the wormhole boundary, skip.
		if (wormEdge(*port)) continue;

		if( port->isItInput() )
			// OK to update size for input PortHole
			port->decCount(port->numberTokens);
		else 
			// OK to update size for output PortHole
			port->incCount(port->numberTokens);
	}

	// Increment noTimes
	noTimes += 1;

	// Indicate that the block was successfully run
	return 0;
}

	///////////////////////////
	// deferrable
	///////////////////////////

// return TRUE if we can defer the star, FALSE if we cannot.
// Postpone any execution of a star feeding data to another
// star that is runnable.  Also postpone if each output port
// has enough data on it to satisfy destination stars.

// FIXME

int BDFStar :: deferrable () {

	int i;
	BDFPortHole* port;
	BDFStarPortIter nextp(*this);
	// Check to see whether any destination blocks are runnable
	for(i = numberPorts(); i>0; i--) {
		// Look at the next port in the list
		port = nextp++;
		// cannot be deferred if on the boundary of wormhole.
		if (wormEdge(*port)) return FALSE;

		BDFStar& dest = (BDFStar&) port->far()->parent()->asStar();

		// If not an output, or destination is not
		// runnable, skip to the next porthole

		if(!port->isItOutput() || dest.notRunnable() != 0)
			continue;
		
		return TRUE;
	}

	// Alternatively, the block might be deferred if its output
	// ports all have enough data to satisfy destination
	// stars, even if the destination stars are not runnable.

	// We must detect the case that there are no output ports
	int outputPorts = FALSE;
	nextp.reset();	// restart iterator
	for(i = numberPorts(); i>0; i--) {

		// Look at the farSidePort of the next port in the list
		port = nextp++;
		// Skip if it is not output or not connected
		if (!port->isItOutput())
			continue;
		
		port = (BDFPortHole*)(port->far());

		// The farside port is an input.  Check Particle supply
		// if not enough, atom cannot be deferred.
		if(port->numTokens() < port->numberTokens)
			return FALSE;
			
		// Since the farside port is an input, the nearside is
		// an output
		outputPorts = TRUE;
	}

	// If we get here, all destinations have enough data,
	// and the block can be deferred, assuming there are output ports.

	return outputPorts;
}


