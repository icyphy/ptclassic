static const char file_id[] = "DLScheduler.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                        All Rights Reserved.

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

