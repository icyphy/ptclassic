#ifndef _MDSDFScheduler_h
#define _MDSDFScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

/**************************************************************************
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
    Date of creation:	August 3, 1993

**************************************************************************/

#include "SDFScheduler.h"
#include "MDSDFPortHole.h"
#include "MDSDFStar.h"

class MDSDFScheduleEntry {
 public:
  MDSDFScheduleEntry(MDSDFStar* s, int startRow, int startCol,
		     int endRow, int endCol) {
    star = s; startRowIndex = startRow; startColIndex = startCol;
    endRowIndex = endRow; endColIndex = endCol; }
  MDSDFStar* star;
  int startRowIndex;
  int startColIndex;
  int endRowIndex;
  int endColIndex;
};

// Not jus ta simply list of pointers to DataFlowStars like it is in
// SDFSchedule.  We need to keep index numbers with the scheduled
// firing of each star.
class MDSDFSchedule : private SequentialList {
 friend class MDSDFSchedIter;
 public:
  // constructor
  MDSDFSchedule() {};

  // destructor
  ~MDSDFSchedule() { initialize(); }

  // Add an element to the end of the list
  void append(MDSDFStar& s,int startRow,int startCol,int endRow,int endCol) {
    LOG_NEW; SequentialList::put(new MDSDFScheduleEntry(&s,startRow,startCol,
							endRow,endCol));
  }

  void initialize();

  // Return the number of elements on the list
  SequentialList::size;

  // Display the schedule
  StringList printVerbose() const;
};

class MDSDFScheduler : public SDFScheduler
{
 friend class MDSDFSchedIter;
 public:
    // Domain identification.
    virtual const char* domain() const;

    // ***** These functions replace those for the 1-D SDFScheduler *****

    // Set the repetition propert of each atomic block in the galaxy, and
    // recursively call the repetitions function for each non-atomic block
    // in the galaxy.
    virtual int repetitions();

    // Display the schedule
    /*virtual*/ StringList displaySchedule();

    // ***** functions used from the SDFScheduler class *****
    /*virtual*/ int run();

protected:
    // Initialize only the states of the galaxy.  The PortHoles will
    // be not be initialized until after the schedule is computed.
    // This is done by changing MDSDFStar::initialize() to only call
    // setup() unless its repetition numbers have been set.
    void postSchedulingInit();

    int lcmOfRowDenoms;
    int lcmOfColDenoms;
    
    // ***** These functions replace those for the 1-D SDFScheduler *****
    /* virtual */ void setup();  
//    /* virtual */ int addIfWeCan(DataFlowStar& star, int defer = FALSE);

    // Schedule the multidimensional synchronous dataflow graph
    /*virtual*/ int computeSchedule(Galaxy& g);

    int simRunStar(MDSDFStar& star, int rowIndex, int colIndex,
		   int deferFiring = FALSE);

    // ***** functions used from the SDFScheduler class *****

    // Check that all stars belong to the right domain.
    // virtual int checkStars();

    // Initialize the galaxy for running, executes after scheduler
    // has completed.  Although this is the same command as in SDFScheduler,
    // different things result because this function calls 
    // MDSDFStar::initialize() rather than DataFlowStar::initialize().
    // Result is that only Geodesics and PortHoles are initialized.
    // virtual int prepareGalaxy();

    // Verify connectivity for the galaxy.
    // virtual int checkConnectivity();

    // Function to adjust sample rates on even horizon ports after schedule
    // is computer.
    // virtual void adjustSampleRates();

private:
    /****** functions that hide and replace those of SDFScheduler *****/

    // set the repetitions property of each atomic block is
    // the subgraph (which may be the whole graph) connected
    // to block.  Called by repetitions().
    int reptConnectedSubgraph(Block& block);

    // Set the repetitions property of blocks on each side of a connection.
    int reptArc(MDSDFPortHole& nearPort, MDSDFPortHole& farPort);
 protected:
    MDSDFSchedule mySchedule;

};

class MDSDFSchedIter:private ListIter {
 public:
  MDSDFSchedIter(const MDSDFSchedule& s) : ListIter(s) {}
  MDSDFSchedIter(MDSDFScheduler& sch) : ListIter(sch.mySchedule) {}
  MDSDFScheduleEntry* next() { return (MDSDFScheduleEntry*)ListIter::next(); }
  MDSDFScheduleEntry* operator++(POSTFIX_OP) { return next(); }
  ListIter::reset;
};
#endif

