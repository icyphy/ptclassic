static const char file_id[] = "HuScheduler.cc";

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

#include "HuScheduler.h"
#include "CGWormBase.h"
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
	parSched->initialize(myGraph);

	// reset the graph for the new parallel schedule
	myGraph->resetGraph();

	// schedule the runnable nodes until there is no more unscheduled node
	// or the graph is deadlocked.
	HuNode* node;
	while ((node = (HuNode*) myGraph->fetchNode()) != 0) {

		// read the star
		CGStar* obj = node->myStar();

		// check the atomicity of the star
		if (obj->isItWormhole() && node->invocationNumber() > 1) {
			node->withProfile(obj->getProfile(numProcs));
			parSched->copyBigSchedule(node, avail);

		} else if (obj->isParallel()) {
			// determine the pattern of processor availability.
			int when = parSched->determinePPA(node, avail);
			if (when < 0) continue;

			// compute the work-residual which can be scheduled in 
			// parallel with this construct.  
			// If the residual work is too small, we may 
			// want to devote more processors to the construct. 
			// If the residual work is big, use the optimal value.
			int resWork = myGraph->sizeUnschedWork() - 
				myGraph->workAfterMe(node);

			if (obj->isItWormhole()) {
				CGWormBase* worm = obj->myWormhole();

				// Possible revision: decide optimal number of 
				// processors by an iterative procedure.
				// calculate the optimal number of processors 
				// talking the "front-idle-time" into account.
				worm->computeProfile(numProcs,resWork,&avail);
				if (haltRequested()) return FALSE;

			} else {
				// TO_DO:
				// for data parallel star, obtain the best
				// profile.
			}

			node->withProfile(obj->getProfile(numProcs));
			parSched->scheduleBig(node, when, avail);

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

	// Hu's level scheduling algorithm produces node assignments.
	// Based on that assignment, we proceed list scheduling including
	// communication.
	mtarget->clearCommPattern();
	myGraph->resetGraph();
	parSched->initialize(myGraph);
	parSched->listSchedule(myGraph);
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

