static const char file_id[] = "ParScheduler.cc";

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

#include "ParScheduler.h"
#include "ParGraph.h"
#include "GalIter.h"
#include "UniProcessor.h"
#include "CGWormhole.h"
#include "StringList.h"
#include "streamCompat.h"

/////////////////////////////
// computeSchedule
/////////////////////////////

// main body of the parallel schedule


ParScheduler :: ParScheduler(BaseMultiTarget* t, const char* logName) {
	mtarget = t;
	logFile = logName;
	exGraph = 0;
	inUniv = TRUE;
}

ParScheduler :: ~ParScheduler() {
}

int ParScheduler :: computeSchedule(Galaxy& galaxy)
{
	myGal = &galaxy;

	if (overrideSchedule()) {
		if (!exGraph) {
			Error::abortRun("We can not override an empty",
			"schedule.");
			return FALSE;
		} 
	} else {
	       if ( logFile && logFile != '\0' ) {
		    logstrm.open(logFile);
	       }
	}

	if (logstrm.good())
		logstrm << "Starting computeSchedule\n";

	exGraph->setLog(&logstrm);

	// form the expanded graph
	if (!exGraph->createMe(galaxy, OSOPreq())) {
		Error::abortRun("Could not create expanded graph");
		invalid = TRUE;
		return FALSE;
	}
	// compute the total (expected) workload with a single processor.
	totalWork = exGraph->getExecTotal();

	// prepare scheduling
	if (!preSchedule()) {
		invalid = TRUE;
		return FALSE;
	}

	// schedule it.
	if(!mainSchedule(galaxy)) {
		invalid = TRUE;
		return FALSE;
	}

	// finalize the schedule of wormholes.
	if (inUniv) {
		finalSchedule(galaxy);
	}
 
	// targetPtr setup for each processor
	parProcs->mapTargets();

	if (logstrm.good()) {
		logstrm.flush();
	}

        return TRUE;
}


/////////////////////////////
// setUpProcs
/////////////////////////////

void ParScheduler :: setUpProcs(int num) {
	numProcs = num;
	avail.create(numProcs);
}

int ParScheduler :: createSubGals() {
	exGraph->restoreHiddenGates();
	parProcs->createSubGals();
	return TRUE;
}

/////////////////////////////
// mainSchedule 
/////////////////////////////

int ParScheduler :: mainSchedule(Galaxy& g) {
	// If manual assignment is requested
	if (assignManually()) return scheduleManually(g);
	// Else, call automatic scheduling routine.
	else {
		if(!scheduleIt()) return FALSE;
		if (OSOPreq()) saveProcIds(g);
	}
	return TRUE;
}

/////////////////////////////
// saveProcIds
/////////////////////////////

void ParScheduler :: saveProcIds(Galaxy& g) {
	GalStarIter iter(g);
	CGStar* s;
	while ((s = (CGStar*) iter++) != 0) {
		ParNode* n = (ParNode*) s->myMaster();
		s->setProcId(n->getProcId());
	}
}

/////////////////////////////
// scheduleManually
/////////////////////////////

int ParScheduler :: scheduleManually(Galaxy& g) {
	// Make sure all stars are assigned to processors.
	GalStarIter iter(g);
	CGStar* s;
	while ((s = (CGStar*) iter++) != 0) {
		if (s->getProcId() < 0) {
			Error::abortRun(*s, " is not assigned a processor",
				"Manual assignment fails.");
			return FALSE;
		} else if (s->getProcId() > numProcs) {
			Error :: abortRun(*s, " procId is out of range",
				"Manual assignment is failed");
		} else {
			// set procId of all invocations of the star
			ParNode* n = (ParNode*) s->myMaster();
			while (n) {
				n->setProcId(s->getProcId());
				n = (ParNode*) n->getNextInvoc();
			}
		}
	}

	// Based on the assignment, let's make a list schedule
	mtarget->clearCommPattern();
	parProcs->initialize();
	exGraph->findRunnableNodes();
	parProcs->listSchedule(exGraph);
	mtarget->saveCommPattern();
	return TRUE;
}

/////////////////////////////
// virtual methods
/////////////////////////////

int ParScheduler :: preSchedule() { return TRUE; }
int ParScheduler :: scheduleIt() { return FALSE; }

/////////////////////////////
// runOnce
/////////////////////////////

void ParScheduler::oldRun() {
// run the schedule for each target
	for (int i = 0; i < mtarget->nProcs(); i++) {
		mtarget->setCurChild(i);
		parProcs->getProc(i)->run();
	}
}

void ParScheduler :: compileRun() {
	// step 1. sub-universe creation.
	if (!createSubGals()) {
		// in case of wormhole, we resort to the old routine.
		// we will remove this later.
		oldRun();
		return;
	}

	if (logstrm.good()) {
		logstrm << parProcs->displaySubUnivs();
		logstrm.flush();
	}

	// run sub-universe in each processor to generate the code
	parProcs->generateCode();
}

/////////////////////////////
// setProfile
/////////////////////////////

void ParScheduler :: setProfile(Profile* profile) {

	int effNum = numProcs;

	for (int i = 0; i < numProcs; i++) {
		UniProcessor* proc = parProcs->getProc(i);

		// set the start & finish time
		profile->setStartTime(i, proc->getStartTime());
		profile->setFinishTime(i, proc->getAvailTime());
		if (!(proc->getAvailTime())) effNum--;
	}
	profile->setEffP(effNum);
	profile->setMinDisplacement(exGraph->pairDistance());
	profile->summary();
}

/////////////////////////////
// finalSchedule
/////////////////////////////

void ParScheduler :: finalSchedule(Galaxy& galaxy) {

	// detect the wormholes, and finalize their schedule.
	GalStarIter next(galaxy);
	CGStar* s;
	while ((s = (CGStar*) next++) != 0) {
		if (s->isItWormhole()) {
			CGWormhole* worm = s->myWormhole();
			worm->insideSchedule();
		}
	}
}

/////////////////////////////
// displaySchedule
/////////////////////////////

StringList ParScheduler :: displaySchedule() {

	StringList out;

	// Display Wormhole schedules.

	out += "*********** Wormhole Schedules ************\n";
	GalStarIter next(*myGal);
	CGStar* s;
	while ((s = (CGStar*) next++) != 0) {
		if (s->isItWormhole()) {
			out += "-------- \n\n";
			CGWormhole* worm = s->myWormhole();
			out += worm->displaySchedule();
		}
	}
	return out;
}

/////////////////////////////
// write Gantt chart
/////////////////////////////

void ParScheduler::writeGantt(ostream& o) {
	int span = parProcs->getMakespan();

	o << "no_processors " << numProcs << "\n";
	o << "period " << span << "\n\n";
	int total = 0;
	for (int i = 0; i < numProcs; i++) {
		UniProcessor* proc = parProcs->getProc(i);
		total += proc->writeGantt(o);
	}

	// this stuff is not right
	o << "min " << span << "\n";
	double p = (100.0 * total) / double(span * numProcs);;
	o << "percentage " << float(p) << "\n";
	o << "optimum " << float(p) << "\n";
	o << "runtime " << total << "\n";
	o.flush();
}

