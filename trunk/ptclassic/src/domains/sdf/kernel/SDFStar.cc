static const char file_id[] = "SDFStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee, J. Buck, D. G. Messerschmitt
 Date of creation: 5/29/90

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SDFStar.h"
#include "SDFPortHole.h"


/*******************************************************************

	class DataFlowStar/SDFStar methods

The DataFlowStar thingies do SDF-specific methods, with a check
of the isSDF fn to see if that is OK.

********************************************************************/

DataFlowStar::DataFlowStar() : noTimes(0), repetitions(0,1), nInP(0),
master(0) {}

// default runCost is 0
unsigned DataFlowStar::runCost() { return 0;}

// return execution time: define a default time of 5 (why?)
int DataFlowStar :: myExecTime() { return 5;}

int DataFlowStar::isSDF() const { return FALSE;}
int DataFlowStar::isSDFinContext() const { return FALSE;}

int SDFStar::isSDF() const { return TRUE;}
int SDFStar::isSDFinContext() const { return TRUE;}

void DataFlowStar :: initialize() {
	Star :: initialize();
	repetitions = 0;
	noTimes = 0;
	BlockPortIter nextp(*this);
	PortHole* p;
	while ((p = nextp++) != 0)
		if (p->isItInput()) nInP++;
}

// execute SDF star -- OVERRIDE if not SDF!
int DataFlowStar :: run() {
	BlockPortIter next(*this);
	for(int i = numberPorts(); i > 0; i--)
		(next++)->receiveData();
	int status = Star::run();
	// we send the data even on error
	next.reset();
	for(i = numberPorts(); i > 0; i--)
		(next++)->sendData();
	return status;
}

// The following is defined in SDFDomain.cc -- this forces that module
// to be included if any SDF stars are linked in.
extern const char SDFdomainName[];

const char* SDFStar :: domain () const { return SDFdomainName;}

// isA functions
ISA_FUNC(DataFlowStar,Star);
ISA_FUNC(SDFStar,DataFlowStar);

inline int wormEdge(PortHole& p) {
	PortHole* f = p.far();
	if (!f) return TRUE;
	else return p.atBoundary();
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

int DataFlowStar :: notRunnable () {
	DFStarPortIter nextp(*this);
	DFPortHole *p;
	// Check to see whether the requisite repetitions have been met.
	if (repetitions.num() <= int(noTimes))
		return 2;

	// Step through all the input ports, checking to see whether
	// there is enough data on the inputs
	while ((p = nextp++) != 0) {
		// worm edges always have enough data
		if (wormEdge(*p)) continue;
		if (p->isItInput() && p->numTokens() < p->numXfer())
			// not enough data
			return 1;
	}
	return 0;
}

	////////////////////////////
	// simRunStar
	////////////////////////////

int DataFlowStar :: simRunStar (int deferFiring) {

	int test = notRunnable();

	if(test) return test;	// return if the star cannot be run

	// If we get to this point without returning, then the star can be run.

	int i;
	DFPortHole* port;

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

	DFStarPortIter nextp(*this);
	for(i = numberPorts(); i>0; i--) {

		// Look at the next port in the list
		port = nextp++;

		// On the wormhole boundary, skip.
		if (wormEdge(*port)) continue;

		if( port->isItInput() )
			// OK to update size for input PortHole
			port->decCount(port->numXfer());
		else 
			// OK to update size for output PortHole
			port->incCount(port->numXfer());
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
// star that is runnable.  Also postpone if any output port
// has enough data on it to satisfy destination stars.

int DataFlowStar :: deferrable () {

	int i;
	DFPortHole* port;
	DFStarPortIter nextp(*this);
	// Check to see whether any destination blocks are runnable
	for(i = numberPorts(); i>0; i--) {
		// Look at the next port in the list
		port = nextp++;
		// cannot be deferred if on the boundary of wormhole.
		if (wormEdge(*port)) return FALSE;

		SDFStar& dest = (SDFStar&) port->far()->parent()->asStar();

		// If not an output, or destination is not
		// runnable, skip to the next porthole

		if(!port->isItOutput() || dest.notRunnable() != 0)
			continue;
		
		return TRUE;
	}

	// Alternatively, the block might be deferred if its output
	// ports all have enough data to satisfy destination
	// stars, even if the destination stars are not runnable.
	// if deferCondition is 2, we defer if ANY output port has
	// enough data.

	nextp.reset();	// restart iterator
	for(i = numberPorts(); i>0; i--) {

		// Look at the farSidePort of the next port in the list
		port = nextp++;
		// Skip if it is not output or not connected
		if (!port->isItOutput())
			continue;
		
		port = (SDFPortHole*)(port->far());

		// The farside port is an input.  Check Particle supply
		// if not enough, atom cannot be deferred.
		// (unless minimum-memory case)
		if (port->numTokens() >= port->numXfer())
			return TRUE;
	}

	return FALSE;
}


