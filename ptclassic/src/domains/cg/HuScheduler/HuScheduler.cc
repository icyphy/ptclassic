static const char file_id[] = "HuScheduler.cc";

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

#include "HuScheduler.h"
#include "CGWormhole.h"
#include "Error.h"

/////////////////////////////
// setUpProcs
/////////////////////////////

void HuScheduler :: setUpProcs(int num) {
	ParScheduler :: setUpProcs(num);
	LOG_DEL; if (parSched) delete parSched;
	LOG_NEW; parSched = new HuParProcs(numProcs, mtarget);
	parProcs = parSched;
}

HuScheduler :: ~HuScheduler() {
	LOG_DEL; if (parSched) delete parSched;
	LOG_DEL; delete myGraph;
}

/////////////////////////////
// scheduleIt
/////////////////////////////

int HuScheduler :: scheduleIt()
{
	wormFlag = FALSE;
	parSched->initialize((HuGraph*) myGraph);

	// reset the graph for the new parallel schedule
	myGraph->resetGraph();

	// schedule the runnable nodes until there is no more unscheduled node
	// or the graph is deadlocked.
	while (myGraph->numUnschedNodes() > 0) {
	   HuNode* node;
	   while ((node = (HuNode*) myGraph->fetchNode()) != 0) {

		// read the star
		CGStar* obj = node->myStar();

		// check the atomicity of the star
		if (obj->isItWormhole()) {
			wormFlag = TRUE;
			CGWormhole* worm = obj->myWormhole();

			// determine the pattern of processor availability.
			parSched->determinePPA(avail);

			// compute the work-residual which can be scheduled in 
			// parallel with this construct.  
			// If the residual work is too small, we may 
			// want to devote more processors to the construct. 
			// If the residual work is big, use the optimal value.
			int resWork = myGraph->sizeUnschedWork() - 
				myGraph->workAfterMe(node);

			// Possible future revision: decide optimal number of 
			// processors by an iterative procedure.
			// calculate the optimal number of processors taking 
			// the "front-idle-time" into account.
			worm->computeProfile(numProcs, resWork, &avail);

			parSched->scheduleBig(node, numProcs, avail);

		} else {
			// schedule the object star.
			parSched->scheduleSmall(node);	
		}
	   }

	   // increase the global clock and put some idle time if there
	   // is no runnble nodes until all processors finish the
	   // current execution.
	   if(!parSched->scheduleIdle()) {
		// deadlock condition is met
		Error::abortRun("Parallel scheduler is deadlocked!");
		return FALSE;
	   }
	}

  // Hu's level scheduling algorithm produces node assignments.
  // Based on that assignment, we proceed list scheduling including
  // communication.
  // Currently, we do not allow wormholes for this
  if (!wormFlag) {
	mtarget->clearCommPattern();
	myGraph->resetGraph();
	parSched->initialize(myGraph);
	parSched->listSchedule(myGraph);
  }
  return TRUE;
}

/////////////////////////////
// displaySchedule
/////////////////////////////

StringList HuScheduler :: displaySchedule() {

	Galaxy* galaxy = myGraph->myGalaxy();

	StringList out;
	out += parSched->display(galaxy);
	out += ParScheduler :: displaySchedule();

	return out;
}

int HuScheduler :: createSubGals() {
	if (!wormFlag) return ParScheduler :: createSubGals();
	else return FALSE;
}
