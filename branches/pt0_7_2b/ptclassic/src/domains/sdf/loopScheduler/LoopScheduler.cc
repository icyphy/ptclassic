static const char file_id[] = "LoopScheduler.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Shuvra Bhattacharyya, Soonhoi Ha (4/92)
 Date of creation: 5/9/91

Loop scheduler

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "LoopScheduler.h"
#include "DecomGal.h"
#include "Target.h"
#include "Error.h"
#include "streamCompat.h"


////////////////////////////
// Loop Scheduler methods //
////////////////////////////

//
// Main routine for LoopScheduler
//
int LoopScheduler::computeSchedule(Galaxy& galaxy)
{
	LOG_DEL; delete cgal;

	// log file stuff.
	const char* file = logFile;
	ostream* logstrm = 0;
	if (file && *file) {
		if (strcmp(file,"cerr") == 0 || strcmp(file,"stderr") == 0)
			logstrm = &cerr;
		else {
			int fd = creat(expandPathName(file), 0666);
			if (fd < 0) {
			    Error::warn(galaxy, "Can't open log file ",file);
			} else {
				LOG_NEW; logstrm = new ofstream(fd);
			}
		}
	}

	// Step 1. Shuvra's decomposition idea.
	//	   After removing the arcs with enough delays.

	LOG_NEW; cgal = new DecomGal(galaxy, logstrm);
	DecomGal* dGal = (DecomGal*) cgal;

	dGal->simplify();	// remove arcs with enough delays.
	dGal->decompose();	// decomposition step.

	// Step 2. Joe's clustering algorithm.
	//         After removing all feed-forward delays, apply his 
	//         clustering again.

	if(!dGal->cluster()) {
		// remove forward delays in "cgal", and do more 
		// clustering if possible.
		if (dGal->removeDelay()) dGal->cluster();
	} 

	// if the clusterGal is not uni-rate, do this
	if (dGal->uniformRate() == FALSE) {
		dGal->loopAll();
		dGal->mergePass();
	}

	//
	// Step3. Inside the decomposed components, apply the complicated
	//	  loop scheduling algorithm.
	//

	if (!dGal->genSubScheds()) {
		invalid = TRUE;
		return FALSE;
	}

	//
	// Step4. generate top-level schedule
	//
	
	if (SDFScheduler::computeSchedule(*dGal)) {
		if (logstrm) {
			*logstrm << "Schedule:\n" << displaySchedule();
			logstrm->flush();
			LOG_DEL; delete logstrm;
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

StringList LoopScheduler::displaySchedule() {
	StringList sch;
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*) next++) != 0) {
		sch += ((SDFCluster*) c)->displaySchedule(0);
	}
	return sch;
}

StringList LoopScheduler::compileRun() {
	StringList code;
	Target& target = getTarget();
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*) next++) != 0) {
		code += ((SDFCluster*) c)->genCode(target,0);
	}
	return code;
}