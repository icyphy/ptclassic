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
#include "UserOutput.h"
#include "CGWormhole.h"
#include "StringList.h"
#include "streamCompat.h"

/////////////////////////////
// computeSchedule
/////////////////////////////

// main body of the parallel schedule

int ParScheduler :: computeSchedule(Galaxy& galaxy)
{
	myGal = &galaxy;

	// log file stuff.
	const char* file = logFile;
	logstrm = 0;
	if (file && *file) {
		if (strcmp(file,"cerr") == 0 || strcmp(file,"stderr") == 0)
			logstrm = &cerr;
		else {
			int fd = creat(expandPathName(file), 0666);
			if (fd < 0)
			   Error::warn(galaxy, "Can't open log file ",file);
			else {
			   LOG_NEW; logstrm = new ofstream(fd);
			}
		}
	}

	if (logstrm)
		*logstrm << "Starting computeSchedule\n";

	exGraph->setLog(logstrm);

	// form the expanded graph
	if (!exGraph->createMe(galaxy)) {
		Error::abortRun("Could not create expanded graph");
		invalid = TRUE;
		return FALSE;
	}
	// compute the total (expected) workload with a single processor.
	totalWork = exGraph->getExecTotal();

	// prepare scheduling
	if (!preSchedule()) return FALSE;

	// schedule it.
	if(!scheduleIt()) return FALSE;

	// finalize the schedule of wormholes.
	if (inUniv) finalSchedule(galaxy);
 
	// delete logging stream
	if (logstrm) logstrm->flush();
	LOG_DEL; delete logstrm;

        return TRUE;
}

/////////////////////////////
// setUpProcs
/////////////////////////////

void ParScheduler :: setUpProcs(int num) {
	numProcs = num;
	avail.create(numProcs);
}

/////////////////////////////
// virtual methods
/////////////////////////////

int ParScheduler :: scheduleIt() { return FALSE; }
int ParScheduler :: preSchedule() { return TRUE; }

/////////////////////////////
// runOnce
/////////////////////////////

void ParScheduler::runOnce() {
// run the schedule for each target
	int iters = mtarget->getIters();
	for (int i = 0; i < mtarget->nProcs(); i++) {
		mtarget->setCurChild(i);
		mtarget->beginIteration(iters,i);
		parProcs->getProc(i)->run();
		mtarget->endIteration(iters,i);
	}
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

	Galaxy* galaxy = exGraph->myGalaxy();

	StringList out;

	// Display Wormhole schedules.

	out += "*********** Wormhole Schedules ************\n";
	GalStarIter next(*galaxy);
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

void ParScheduler::writeGantt(UserOutput& o) {
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

