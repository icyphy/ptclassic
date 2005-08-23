static const char file_id[] = "LoopScheduler.cc";
/******************************************************************
Version identification:
@(#)LoopScheduler.cc	1.15	2/5/96

Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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
#include "pt_fstream.h"


////////////////////////////
// Loop Scheduler methods //
////////////////////////////

//
// Main routine for LoopScheduler
//
int LoopScheduler::computeSchedule(Galaxy& g)
{
	LOG_DEL; delete cgal;

	// log file stuff.
	ostream* logstrm = 0;
	if (logFile && *logFile) {
		logstrm = new pt_ofstream(logFile);
		if (!*logstrm) {
			LOG_DEL; delete logstrm;
			return FALSE;
		}
	}

	LOG_NEW; DecomGal* dGal = new DecomGal(g, logstrm);
	cgal = dGal;

	// Step 1. Initially, perform Joe's clustering
	//	   After removing the arcs with enough delays.
	dGal->simplify();	// remove arcs with enough delays.
	dGal->cluster();
	setGalaxy(*dGal);
	repetitions();

	// Step 2. Shuvra's decomposition idea.
	dGal->decompose();	// decomposition step.

	// Step 3. Joe's clustering algorithm. Again.
	// It now deals correctly with feed-forward delays, so we
	// do not need to apply it twice as before.

	dGal->cluster();

	// if the clusterGal is not uni-rate, do this
	if (dGal->uniformRate() == FALSE) {
		dGal->loopAll();
		dGal->mergePass();
	}

	//
	// Step4. generate top-level schedule.  Subschedules are
	// generated by simRunStar methods for bag clusters.
	
	if (SDFScheduler::computeSchedule(*dGal)) {
		if (logstrm) {
			*logstrm << "Doing simulated run to set geodesics\n";
		}
		dGal->genSubScheds();
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
	sch << "{\n  { scheduler \"Bhattacharyya's Loop SDF Scheduler\" }\n";
	while ((c = (SDFCluster*) next++) != 0) {
		sch << ((SDFCluster*) c)->displaySchedule(0);
	}
	sch << "}\n";
	return sch;
}

void LoopScheduler::compileRun() {
	Target& targ = target();
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*) next++) != 0) {
		c->genCode(targ,1);
	}
}
