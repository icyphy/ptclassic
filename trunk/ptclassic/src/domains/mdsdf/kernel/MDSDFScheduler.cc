static const char file_id[] = "MDSDFScheduler.cc";

/*******************************************************************
  $Id$

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
    Date of creation:	8 August, 1993

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <strstream.h>
#include "MDSDFScheduler.h"
#include "Fraction.h"
#include "Error.h"

extern const char MDSDFdomainName[];

// Domain identification.
const char* MDSDFScheduler::domain() const { return MDSDFdomainName; }

  ////////////////////////////
  // repetitions
  ////////////////////////////

// Derived from SDFScheduler::repetitions()
int MDSDFScheduler::repetitions()  {
  // Initialize the least common multiple for the rows and the columns,
  // which after all the repetitions have been set, will equal the lcm
  // of all the row and all the column denominators respectively.
  lcmOfRowDenoms = 1;
  lcmOfColDenoms = 1;

  // The following iteration occurs exactly as many times as
  // there are blocks in the universe.
  // Note that for a connected universe, this iteration would be
  // entirely unnecessary.  Is there some way to get rid of it?
  // Should we allow disconnected universes?
  DFGalStarIter next(*galaxy());
  MDSDFStar* s;
  while ((s = (MDSDFStar*)next++) != 0) {
    // Get the next atomic b
    // First check too see whether a repetitions property has
    // been set.  If so, do nothing.  Otherwise, compute the
    // repetitions property.

    // Due to how ANYSIZE dimensioned stars should inherit from the source
    // and not the destination, we want to do repetitions only from source
    // stars.
    if(s->numInputs() == 0) {
      if(s->repetitions.num() == 0) {
	// Set repetitions to 1 and set repetitions for
	// all blocks connected to the current block.
	s->repetitions = 1;
	s->rowRepetitions = 1;
	s->colRepetitions = 1;

	reptConnectedSubgraph(*s);
	if (invalid) return FALSE;
      }
    }
  }

  // Once all the repetitions properties are set, the lcm member
  // contains the least common multiple of all the denominators
  // of all the fractional repetitions.  To convert them to
  // integers, we multiply through by lcm.

  next.reset();
  while ((s = (MDSDFStar*)next++) != 0) {
    // Get the next atomic block:
    s->rowRepetitions *= Fraction(lcmOfRowDenoms);
    s->rowRepetitions.simplify();
    s->colRepetitions *= Fraction(lcmOfColDenoms);
    s->colRepetitions.simplify();
    s->repetitions = s->rowRepetitions * s->colRepetitions;
  }
  return TRUE;
}


  ////////////////////////////
  // reptConnectedSubgraph
  ////////////////////////////

// The following function sets the repetitions property of
// all atomic blocks connected to block, directly or indirectly.
// It is assumed that block has its repetitions property already set.
int MDSDFScheduler :: reptConnectedSubgraph (Block& block) {
  // Step through each portHole
  BlockPortIter nextp(block);
  for(int i = block.numberPorts(); i>0; i--) {
    MDSDFPortHole& nearPort = (MDSDFPortHole&)*nextp++;
    if (nearPort.atBoundary())
      // if the port is at the boundary of the wormhole.
      continue;
    MDSDFPortHole& farPort = (MDSDFPortHole&)*(nearPort.far());
    // recursively call this function on the farSideBlock,
    // having determined that it has not previously been done.

    if(reptArc(nearPort,farPort))
      reptConnectedSubgraph (*(farPort.parent()));
  }
  return TRUE;
}

  ////////////////////////////
  // reptArc
  ////////////////////////////

// This function sets the repetitions property of two blocks
// on opposite sides of a connection.  It is assumed that nearPort
// belongs to a block that has already had its repetitions property set.
// If in addition farPort has had its repetitions property set,
// then consistency is checked, and FALSE is returned.  Otherwise,
// TRUE is returned.

int MDSDFScheduler::reptArc (MDSDFPortHole& nearPort, MDSDFPortHole& farPort){
  MDSDFStar& nearStar = (MDSDFStar&) nearPort.parent()->asStar();
  MDSDFStar& farStar = (MDSDFStar&) farPort.parent()->asStar();
  Fraction& nearStarRowRepetitions = nearStar.rowRepetitions;
  Fraction& nearStarColRepetitions = nearStar.colRepetitions;
  Fraction& farStarRowRepetitions = farStar.rowRepetitions;
  Fraction& farStarColRepetitions = farStar.colRepetitions;
  Fraction& farStarRepetitions = farStar.repetitions;
  Fraction farStarRowShouldBe;
  Fraction farStarColShouldBe;

  // ANYSIZE resolution is based on input ports, ie. if a star has
  // ANYSIZE outputs, it MUST have an ANYSIZE input (the reverse is
  // not necessary).  A cascade of ANYSIZE inputs/ouputs is possible, which
  // necessitates a recursive search for the first ANYSIZE input connected
  // to a non-ANYSIZE output.
  // A star should not have two inputs with ANYSIZE dimensions because
  // they will be set to the same dimensions, unless this was intended.
  //
  // One example is for fork stars.
  if(farPort.numRowXfer() == ANYSIZE) {
    // farStar has ANYSIZE rows, resolve it
    if(farStar.resolveANYSIZErows() == 0) 
      return FALSE;               // continue only if resolution successful 
  }
  if(farPort.numColXfer() == ANYSIZE) {
    // farStar has ANYSIZE columns, resolve it
    if(farStar.resolveANYSIZEcols() == 0)  
      return FALSE;               // continue only if resolution successful
  }

  // compute what the far star repetitions property should be.
  // There are two types of repetitions.  One is how many row and column
  // repetitions are needed for an initial iteration firing, which differs
  // from steady state repetitions due to the fact that there might be column
  // delays on the arcs.

  // Note: total repetitions for a star is equal to the number of row
  // repetitions times the number of column repetitions 

  Fraction rowFactor(nearPort.numRowXfer(),farPort.numRowXfer());
  Fraction colFactor(nearPort.numColXfer(),farPort.numColXfer());
  farStarRowShouldBe = nearStarRowRepetitions * rowFactor;
  farStarColShouldBe = nearStarColRepetitions * colFactor;

  // Simplify the fraction
  farStarRowShouldBe.simplify();
  farStarColShouldBe.simplify();

  if (farStarRepetitions.num() == 0) {
    // farStarRepetitions has not been set, so set it
    farStarRowRepetitions = farStarRowShouldBe;
    farStarColRepetitions = farStarColShouldBe;
    farStarRepetitions = farStarRowShouldBe * farStarColShouldBe;

    // update the least common multiple of the denominators
    lcmOfRowDenoms = lcm(lcmOfRowDenoms,farStarRowShouldBe.den());
    lcmOfColDenoms = lcm(lcmOfColDenoms,farStarColShouldBe.den());
    return TRUE;
  }
  else {
    // farStarRepetitions has been set, so test for equality
    if ((farStarRowRepetitions != farStarRowShouldBe) ||
        (farStarColRepetitions != farStarColShouldBe)) {
      Error::abortRun(farStar,"sample rate inconsistency detected");
      invalid = TRUE;
    }
    return FALSE;
  }
}



	////////////////////////////
	// run
	////////////////////////////

int MDSDFScheduler::run() {
  MDSDFSchedIter nextEntry(mySchedule);
  MDSDFScheduleEntry* entry;

  if(invalid) {
    Error::abortRun(*galaxy(), "Error during setup - can't run");
    return FALSE;
  }
  if(haltRequested()) {
    Error::abortRun(*galaxy(), "Can't continue after run-time error");
    return FALSE;
  }

  while(numItersSoFar < numIters && !haltRequested()) {
    nextEntry.reset();

    // assume the schedule has been set by the setup member
    while((entry = nextEntry++) != 0 && !invalid) {
      for(int row = entry->startRowIndex; row <= entry->endRowIndex; row++) {
	for(int col = entry->startColIndex; col <= entry->endColIndex; col++) {
	  entry->star->rowIndex = row;
	  entry->star->colIndex = col + 
	    numItersSoFar * (int)double(entry->star->colRepetitions);
	  invalid = !entry->star->run();
	  if(invalid) break;
	}
	if(invalid) break;
      }
    }
    currentTime += schedulePeriod;
    numItersSoFar++;
  }
  numIters++;
  return TRUE;
}

	////////////////////////////
	// setup
	////////////////////////////

void MDSDFScheduler :: setup () {
	if (!galaxy()) {
		Error::abortRun("MDSDFScheduler: no galaxy!");
		return;
	}
	numItersSoFar = 0;
	numIters = 1;			// reset the member "numIters"
	clearHalt();
	invalid = FALSE;

        checkConnectivity();            // from SDFScheduler
        if (invalid) return;

        // Initialize only the states of the galaxy.  The PortHoles will
        // be not be initialized until after the repetitions are computed.
        // This is done by changing MDSDFStar::initialize().
        prepareGalaxy();  // from SDFScheduler

	currentTime = 0;

	if (haltRequested()) {
		invalid = TRUE;
		return;
	}

	// force recomputation of repetitions and noTimes.  Also make
	// sure all stars are SDF.
        checkStars();
	if (invalid) return;

	repetitions();
	if (invalid) return;

        // Once repetitions are known, initialize the portHoles and geodesics
        postSchedulingInit();

        // Schedule the graph.

        computeSchedule(*galaxy());
	if (invalid) return;

	adjustSampleRates();
	return;

}

void MDSDFScheduler::postSchedulingInit() {
  // Sort of quirky.  The automatic domain generator created a function
  // prepareForScheduling() in the MDSDFStar class, but this function
  // does not exist in Star or DataFlowStar
  GalTopBlockIter next(*galaxy());
  Block *b;
  while ((b = next++) != 0 && !Scheduler::haltRequested())
    b->initPorts();
}

// Compute the mdsdf schedule, almost exactly the same as 
// SDFScheduler::computeSchedule except that it initializes the mdsdf
// mySchedule instead of the sdf mySchedule.
int MDSDFScheduler::computeSchedule(Galaxy& g) { 
  DFGalStarIter nextStar(g);
  DataFlowStar* s;

  mySchedule.initialize();

  // MAIN LOOP
  // After a scheduler pass, the passValue tells us whether the
  // scheduler is finished.  After the pass, the meaning of the
  // possible values is:
  //      2 - scheduler is finished
  //      1 - scheduler is deadlocked
  //      0 - more passes are required

  do {
    passValue = 2;
    deferredStar = 0;
    int numAddedThisPass;

    numAddedThisPass = 0;
    nextStar.reset();
    while ((s = nextStar++) != 0) {
      int runResult;
      MDSDFStar& ms = *((MDSDFStar*)s);
      int startRow = ms.rowIndex;
      int startCol = ms.colIndex;
      int endRow = 0;
      int endCol = 0;
      int numTimesThisStar = 0;

      do {
	runResult = simRunStar(ms,ms.rowIndex,ms.colIndex,deferredFiring);

	if(runResult == 0) {
	  numAddedThisPass++;
	  numTimesThisStar++;
	  endRow = ms.rowIndex;
	  endCol = ms.colIndex;
	  // update the index for the next firing
	  if(++ms.colIndex >= (int)((double)ms.startColIndex + 
				    (double)ms.colRepetitions)) {
	    // end of column repetitions, goto next row
	    if(++ms.rowIndex >= (int)((double)ms.startRowIndex + 
				      (double)ms.rowRepetitions)) {
	      // end of row repetitions, finished all reps for this iteration
	      ms.startColIndex += (int)((double)ms.colRepetitions);
	      ms.startRowIndex = 0;  // is this always zero?!
	      ms.rowIndex = ms.startRowIndex;
	    }
	    ms.colIndex = ms.startColIndex;
	  }
	  passValue = 0;
	}
	else if(runResult == 1 && passValue != 0 && deferredStar == 0) {
	  passValue = 1;
	}
      } while (runResult == 0);
      
      if(numTimesThisStar > 0)
	mySchedule.append(ms,startRow,startCol,endRow,endCol);
    }

  } while (passValue == 0);
  // END OF MAIN LOOP

  if (passValue == 1) reportDeadlock(nextStar);

  return !invalid;
}

int MDSDFScheduler::simRunStar(MDSDFStar& atom, int rowIndex, int colIndex,
			       int deferFiring) {
  int status = atom.simRunStar(rowIndex,colIndex,deferFiring);
  return status;
}

StringList MDSDFScheduler::displaySchedule() {
  return mySchedule.printVerbose();
}

// display the schedule
StringList MDSDFSchedule::printVerbose() const  {
  ostrstream strm;
  strm << "MDSDF SCHEDULE:\n";
  MDSDFSchedIter next(*this);
  MDSDFScheduleEntry* entry;
  for (int i = size(); i > 0; i--) {
    entry = next++;
    strm << entry->star->fullName();
    strm << ", firing range: ";
    strm << "(" << entry->startRowIndex << "," << entry->startColIndex << ")";
    if(entry->startRowIndex != entry->endRowIndex ||
       entry->startColIndex != entry->endColIndex)
      strm << " - (" << entry->endRowIndex << "," << entry->endColIndex << ")";
    strm << "\n";
  }
  strm << ends;
  return StringList(strm.str());
}

// schedule destructor, free the elements of the list, but not the list itself
// destroying the links of the list is done by ~SequentialList()
void MDSDFSchedule::initialize() {
  while(!empty()) {
    Pointer p = getAndRemove();
    LOG_DEL; delete p;
  }
}





