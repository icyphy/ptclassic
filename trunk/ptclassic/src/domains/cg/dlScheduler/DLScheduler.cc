static const char file_id[] = "DLScheduler.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
#include "CGWormhole.h"
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
	// temporal hack
	wormFlag = FALSE;

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

	   // check the atomicity of the star
	   if (obj->isItWormhole()) {

		// hack
		wormFlag = TRUE;

	   	CGWormhole* worm = obj->myWormhole();

	   	// determine the pattern of processor availability.
		// And return the schedule time.
	   	int when = parSched->determinePPA(node, avail);

	   	// compute the work-residual which can be scheduled in 
	   	// parallel with this construct.  
	   	// If the residual work is too small, we may 
	   	// want to devote more processors to the construct. 
	   	// If the residual work is large, we use the optimal value.
	   	int resWork = myGraph->sizeUnschedWork() - 
			myGraph->workAfterMe(node);

		// Possible future revision: decide optimal number of 
		// processors by an iterative procedure.
	   	// calculate the optimal number of processors taking 
		// the "front-idle-time" into account.
	   	worm->computeProfile(numProcs, resWork, &avail);

	   	parSched->scheduleBig(node, numProcs, when, avail);

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
  
  // By dynamic level scheduling, we obtain a schedule for a system
  // which overlaps communication with computation.
  // In case there is no wormhole inside, and we set the flag saying
  // NO overlapComm, then we do additional list scheduling at the end
  // based on the processor assignment determined by the dynamic level
  // scheduling algorithm.
  if (!wormFlag) {
	mtarget->clearCommPattern();
	myGraph->resetGraph();
	parSched->initialize(myGraph);
	parSched->listSchedule(myGraph);
  }
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
	out += ParScheduler :: displaySchedule();

	return out;
}

int DLScheduler :: createSubGals() {
	if ((!wormFlag) && noOverlap)
		 return ParScheduler :: createSubGals();
	else return FALSE;
}
