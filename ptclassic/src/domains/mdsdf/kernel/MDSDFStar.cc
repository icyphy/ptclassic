static const char file_id[] = "MDSDFStar.cc";

/*******************************************************************
  Version $Id$

Copyright (c) 1990-1994
   The Regents of the University of California. All rights reserved.

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

    Programmer:		Mike J. Chen
    Date of creation:	13 August 1993

********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MDSDFStar.h"
#include "SimControl.h"

extern const char MDSDFdomainName[];

// Class identification.
ISA_FUNC(MDSDFStar,DataFlowStar);

// Domain identification.
const char* MDSDFStar::domain() const { return MDSDFdomainName; }

// Constructor
MDSDFStar::MDSDFStar() : rowIndex(0), colIndex(0),
			 rowRepetitions(0,1), colRepetitions(0,1),
			 startRowIndex(0), startColIndex(0) {}

// Domain-specific initialization.  This sets up only the states of
// each star in the galaxy and calls the user setup() routine.
void MDSDFStar::initialize() { 
  initState();       // defined in Block
  // call user-specified initialization (unless error occurred)
  if (!SimControl::haltRequested())
    setup();
  repetitions = 0;
  rowRepetitions = 0;
  colRepetitions = 0;
  rowIndex = 0;
  colIndex = 0;
  startRowIndex = 0;
  startColIndex = 0;
  noTimes = 0;
  initPortCounts();  // defined in DataFlowStar
}

int MDSDFStar::simRunStar(int rIndex, int cIndex, int deferFiring) {
  if(!okSDF()) return FALSE;  // might want to change to MDSDF specific check
  rowIndex = rIndex;
  colIndex = cIndex;

  int test = notRunnable();
 
  if(test) return test; // return if the star cannot be run

  int i;

  MDSDFPortHole* port;

  // skip deferring for now.  doesn't appear to be needed given that
  // the buffer space is completely allocated for this iteration
  
  // Iterate over the ports to adjust the number of tokens on each arc.
  // Note that this should work equally well if there are no inputs.
  DFStarPortIter nextp(*this);
  for(i = numberPorts(); i > 0; i--) {
    // Look at the next port in the list
    port = (MDSDFPortHole*)nextp++;
    
    // On the wormhole boundary, skip.
    if(port->atBoundary()) continue;
    
    if(port->isItInput()) {}
      // OK to update size for input PortHole
    else
      // Ok to update Geodesic buffer size for output PortHole
      port->setGeodesicValid((rowIndex+1)*(port->numRowXfer())-1,
			     (colIndex+1)*(port->numColXfer())-1);
  }
  
  // Indicate that the block was successfully run
  return 0;
}

// For Solaris2.3 CC, functions declared to return an int must return
// something.
int MDSDFStar::deferrable() {return 0;}


// Result codes when testing if a star is runnable:
// 0 - star can be run now
// 1 - star can't be run now, but should be runnable later
// 2 - star has been run as many times as needed for the given iterations
int MDSDFStar::notRunnable() {
  DFStarPortIter nextp(*this);
  MDSDFPortHole *p;

  // Check to see whether the required number of row and column repetitions
  //   has been met
  if(int(double(rowRepetitions)) <= rowIndex ||
     int(double(colRepetitions)) <= colIndex)
    return 2;

  // Step through all the input ports, checking to see whether the required
  // inputs are on the input Geodesics
  while((p = (MDSDFPortHole*)nextp++) != 0) {
    // worm edges always have enough data
    if(p->atBoundary()) continue;
    if(p->isItInput()) {
      if(p->lastValidColOnGeo() < p->neededValidCol(colIndex) ||
	 p->lastValidRowOnGeo() < p->neededValidRow(rowIndex))
	return 1;     // required tokens unavailable on Geodesic
    }
  }
  return 0; // star is runnable
}

// Resolve ANYSIZE inputs and outputs.  Function is called during
// scheduling.  Returns 0 if there is an error, such as when
// there are ANYSIZE outputs but no ANYSIZE input, or if there are
// multiple inputs with ANYSIZE specifications.  Otherwise, returns
// the number of rows that all ANYSIZE inputs and outputs are set to.
int MDSDFStar::resolveANYSIZErows() {
  if(numInputs()) {
    BlockPortIter nextp(*this);
    PortHole* p;
    MDSDFPortHole* input = 0;
    int resolvedSize;

    // find the single input that is of type ANYSIZE, if multiple
    while((p = nextp++) != 0) {
      MDSDFPortHole* mp = (MDSDFPortHole*)p;
      if(mp->numRowXfer() == ANYSIZE && mp->isItInput()) {
	if(input == 0)
	  input = mp;
        else return 0;  // multiple inputs with ANYSIZE rows
      }
    }
    if(input == 0)
      return 0;         // no input with ANYSIZE rows
    // at this point, we should have only one input with ANYSIZE rows
    resolvedSize = ((MDSDFPortHole*)(input->far()))->numRowXfer();
    if(resolvedSize == ANYSIZE) {
      resolvedSize = ((MDSDFStar*)(input->parent()))->resolveANYSIZErows();
      if(resolvedSize == 0)
	return 0;
    }
    // at this point, we have a valid resolved row size, set the input
    // and all outputs with ANYSIZE rows to be this size;
    nextp.reset();
    while((p = nextp++) != 0) {
      MDSDFPortHole* mp = (MDSDFPortHole*)p;
      if(mp->numRowXfer() == ANYSIZE)
	mp->setRowXfer(resolvedSize);
    }
    return resolvedSize;
  }
  return 0;
}

// Resolve ANYSIZE inputs and outputs.  Function is called during
// scheduling.  Returns 0 if there is an error, such as when
// there are ANYSIZE outputs but no ANYSIZE input, or if there are
// multiple inputs with ANYSIZE specifications.  Otherwise, returns
// the number of columns that all ANYSIZE inputs and outputs are set to.
int MDSDFStar::resolveANYSIZEcols() {
  if(numInputs()) {
    BlockPortIter nextp(*this);
    PortHole* p;
    MDSDFPortHole* input = 0;
    int resolvedSize;

    // find the single input that is of type ANYSIZE, if multiple
    while((p = nextp++) != 0) {
      MDSDFPortHole* mp = (MDSDFPortHole*)p;
      if(mp->numColXfer() == ANYSIZE && mp->isItInput()) {
	if(input == 0)
	  input = mp;
        else return 0;  // multiple inputs with ANYSIZE columnss
      }
    }
    if(input == 0)
      return 0;         // no input with ANYSIZE columns
    // at this point, we should have only one input with ANYSIZE columnss
    resolvedSize = ((MDSDFPortHole*)(input->far()))->numColXfer();
    if(resolvedSize == ANYSIZE) {
      resolvedSize = ((MDSDFStar*)(input->parent()))->resolveANYSIZEcols();
      if(resolvedSize == 0)
	return 0;
    }
    // at this point, we have a valid resolved column size, set the input
    // and all outputs with ANYSIZE columns to be this size;
    nextp.reset();
    while((p = nextp++) != 0) {
      MDSDFPortHole* mp = (MDSDFPortHole*)p;
      if(mp->numColXfer() == ANYSIZE)
	mp->setColXfer(resolvedSize);
    }
    return resolvedSize;
  }
  return 0;
}


