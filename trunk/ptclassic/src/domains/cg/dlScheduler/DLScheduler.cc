static const char file_id[] = "DLScheduler.cc";

/*****************************************************************
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
Date of last revision:

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DLScheduler.h"
#include "Error.h"

/////////////////////////////
// setUpProcs
/////////////////////////////

void DLScheduler :: setUpProcs(int num) {
	ParScheduler :: setUpProcs(num);
	LOG_DEL; if (parSched) delete parSched;
	LOG_NEW; parSched = new DLParProcs(numProcs, mtarget);
	parProcs = parSched;
}

DLScheduler :: ~DLScheduler() {
	LOG_DEL; if (parSched) delete parSched;
	LOG_DEL; delete myGraph;
}

/////////////////////////////
// scheduleIt
/////////////////////////////

int DLScheduler :: scheduleIt()
{
  	parSched->initialize(myGraph);

  	// reset the graph for the new parallel schedule
  	myGraph->resetGraph();

	// reset Target
	mtarget->clearCommPattern();

  	// schedule the runnable nodes until there is no more unscheduled node
  	// or the graph is deadlocked.
  	DLNode* node;
	while ((node = myGraph->fetchNode()) != 0) {

	   // read the star
	   CGStar* obj = node->myStar();

	   if (obj->isParallel()) {
		// this version of scheduler could not handle
		// data parallel star. sorry!
		Error::abortRun("Sorry, this scheduler can not handle ",
			"parallel tasks. Try macroScheduler");
		return FALSE;
	   } else {
	   	// schedule the object star.
	   	parSched->scheduleSmall(node);	
   	   }
           if (haltRequested()) {
                Error::abortRun("schedule error");
                return FALSE;
           }
  	}

	if(myGraph->numUnschedNodes()) {
		// deadlock condition is met
		Error::abortRun("Parallel scheduler is deadlocked!");
		return FALSE;
	}
  
  // We do additional list scheduling at the end
  // based on the processor assignment determined by the dynamic level
  // scheduling algorithm.
  mtarget->clearCommPattern();
  myGraph->resetGraph();
  parSched->initialize(myGraph);
  if (parSched->listSchedule(myGraph) < 0) return FALSE;
  mtarget->saveCommPattern();
	
  return TRUE;
}

/////////////////////////////
// displaySchedule
/////////////////////////////

StringList DLScheduler :: displaySchedule() {

	Galaxy* galaxy = myGraph->myGalaxy();

	StringList out;
	out += parSched->display(galaxy);

	return out;
}

