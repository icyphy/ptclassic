static const char file_id[] = "DataFlowStar.cc";
/******************************************************************
Version identification:
@(#)DataFlowStar.cc	1.10	03/27/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  J. T. Buck

DataFlowStar is the baseclass for stars in the various dataflow domains.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DataFlowStar.h"
#include "DFPortHole.h"

// The methods in DataFlowStar, for the most part, assume SDF semantics,
// with a check of the isSDF function to make sure this is OK.  non-SDF
// stars should override these methods or avoid calling them; if not,
// errors will be reported.

DataFlowStar::DataFlowStar() : repetitions(0,1),
noTimes(0), nInP(0), master(0) {}

// default runCost is 0
unsigned DataFlowStar::runCost() { return 0;}

// return execution time: define a default time of 5 (why?)
int DataFlowStar :: myExecTime() { return 5;}

// by default we assume the star is NOT SDF which means the fns below
// will generate errors.

int DataFlowStar::isSDF() const { return FALSE;}
int DataFlowStar::isSDFinContext() const { return isSDF();}

// functions for dynamic execution
int DataFlowStar::setDynamicExecution(int) { return TRUE;}
DFPortHole* DataFlowStar::waitPort() const { return 0;}
int DataFlowStar::waitTokens() const { return 0;}

// prepare star for execution.
void DataFlowStar :: initialize() {
	Star :: initialize();
	repetitions = 0;
	noTimes = 0;
	initPortCounts();
}

// count # of ports.
void DataFlowStar :: initPortCounts() {
	BlockPortIter nextp(*this);
	PortHole* p;
	nInP = 0;
	while ((p = nextp++) != 0)
		if (p->isItInput()) nInP++;
}

// check to see if using the scheduling fns below is valid.
void DataFlowStar :: schedError() const {
	Error::abortRun(*this, "SDF scheduling may not be used");
}

// execute SDF star -- OVERRIDE if not SDF!
int DataFlowStar :: run() {
	BlockPortIter next(*this);
	PortHole* nextPort;
	int i;
	for(i = numberPorts(); i > 0; i--) {
	  nextPort = next.next();
	  if (nextPort->isItInput()) {
	    nextPort->receiveData();
	  }
	}
	int status = Star::run();
	// we send the data even on error
	next.reset();
	for(i = numberPorts(); i > 0; i--) {
	  nextPort = next.next();
	  if (nextPort->isItOutput()) {
	    nextPort->sendData();
	  }
	}
	return status;
}

// functions for use with the loop scheduler
void DataFlowStar::beginLoop(int,DFPortHole*,int) {}
void DataFlowStar::endLoop() {}

// isA function
ISA_FUNC(DataFlowStar,Star);

// make a duplicate DataFlowStar.  This will call Star::clone 
// and then set DataFlowStar specific data members such as
// repetitions.
/* virtual */ Block* DataFlowStar::clone () const {
	DataFlowStar* star = (DataFlowStar*)Star::clone();
	if (star) {
	  star->repetitions = repetitions;
	}
	return star;
}

// scheduling functions below assume SDF semantics can be used.

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
	if (reps() <= int(noTimes))
		return 2;

	// Step through all the input ports, checking to see whether
	// there is enough data on the inputs
	while ((p = nextp++) != 0) {
		// worm edges always have enough data
		if (p->atBoundary()) continue;
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
	if (!okSDF()) return FALSE;

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
		if (port->atBoundary()) continue;

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
		if (port->atBoundary()) return FALSE;

		DataFlowStar& dest = (DataFlowStar&) port->far()->parent()->asStar();

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
		
		port = port->far();

		// The farside port is an input.  Check Particle supply
		// if not enough, atom cannot be deferred.
		// (unless minimum-memory case)
		if (port->numTokens() >= port->numXfer())
			return TRUE;
	}

	return FALSE;
}
