static const char file_id[] = "ParScheduler.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

#include "ParScheduler.h"
#include "ParGraph.h"
#include "GalIter.h"
#include "UniProcessor.h"
#include "StringList.h"
#include "streamCompat.h"
#include "../../../pigilib/ptk.h"
#include <tcl.h>

void ParScheduler::setup() {
    	// compute repetitions, schedule (by calling computeSchedule)
    	SDFScheduler::setup();

	// targetPtr setup for each processor
	mapTargets();

	// sub-universe creation.
	if (!createSubGals(*galaxy())) {
		invalid = TRUE;
		return;
	}

	prepareCodeGen();
	if (haltRequested()) { invalid = TRUE; return; }

	// make parallel target intervene here to do something necessary
	// by default, do nothing
	mtarget->prepareCodeGen();
	if (haltRequested()) { invalid = TRUE; return; }
}

int ParScheduler::dagNodes() const {
    if (exGraph == NULL) {
      Error::warn("ParScheduler::dagNodes: The multirate dataflow graph has not been expanded into a precedence DAG");
      return 0;
    }
    return exGraph->numNodes();
}

/////////////////////////////
// computeSchedule
/////////////////////////////

// main body of the parallel schedule

ParScheduler :: ParScheduler(MultiTarget* t, const char* logName) {
	mtarget = t;
	logFile = logName;
	logstrm = 0;
	exGraph = 0;
	inUniv = TRUE;
	EGcreated = 0;
}

ParScheduler :: ~ParScheduler() {
}

int isaOSOPschedule(Galaxy& g) {
	GalStarIter iter(g);
	CGStar* s;
	int globalTest = TRUE;
	while ((s = (CGStar*) iter++) != 0) {
		ParNode* n = (ParNode*) s->myMaster();
		ParNode* firstN = n;
		int pid = n->getProcId();
		int temp = TRUE;
		while ((n = (ParNode*) n->getNextInvoc()) != 0) {
			if (pid != n->getProcId()) {
				firstN->setOSOPflag(0);
				temp = FALSE;
				globalTest = FALSE;
				break;
			}
		}
		if (temp) firstN->setOSOPflag(1);
	}
	return globalTest;
}

int ParScheduler :: computeSchedule(Galaxy& g)
{
	// make parallel target intervene here to do something necessary
	// before scheduling stars: e.g. setting machine_Information.
	// by default, do nothing
	if (mtarget->prepareSchedule() == FALSE) return FALSE;

	// if numProcs == 1, use simple SDF scheduling
	if (numProcs == 1) {
		int flag =  SDFScheduler :: computeSchedule(g);

		// total work computation. set targets.
		int sum = 0;
		DFGalStarIter nextStar(*galaxy());
		DataFlowStar* s;
		CGTarget* t = (CGTarget*) mtarget->child(0);
		while ((s = nextStar++) != 0) {
			sum += s->myExecTime() * s->reps();
			s->setTarget(t);
		}
		totalWork = sum;
		t->setGalaxy(*galaxy());
		t->scheduler()->setGalaxy(*galaxy());
		t->copySchedule(mySchedule);
		return flag;
	}

	if (overrideSchedule()) {
		if (!EGcreated) {
			Error::abortRun("We can not override an empty",
			"schedule.");
			return FALSE;
		} 
	} else {
	       	if ( logFile && *logFile != '\0' ) {
			logstrm_real.open(logFile);
			logstrm = &logstrm_real;
		}
	}

	if (logstrm)
		*logstrm << "Starting computeSchedule\n";

	exGraph->setLog(logstrm);

	// form the expanded graph
	if (!exGraph->createMe(g, OSOPreq())) {
		Error::abortRun("Could not create expanded graph");
		invalid = TRUE;
		return FALSE;
	}
	EGcreated = TRUE;

	// compute the total (expected) workload with a single processor.
	totalWork = exGraph->getExecTotal();

	// targetPtr setup for each processor
	mapTargets();

	// schedule it.
	mainSchedule();
	if (haltRequested()) { invalid = TRUE; return FALSE; }

	// finalize the schedule of wormholes.
	if (inUniv && (!assignManually())) {
		if (!finalSchedule()) {
			invalid = TRUE;
			return FALSE;
		}
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

// sub-universe creation.

int ParScheduler :: createSubGals(Galaxy& g) {
	// explicit set the OSOP request flag by looking at the schedule
	// result if possible to avoid Spread/Collect stars as much as
	// possible
	if (OSOPreq() != TRUE) mtarget->setOSOPreq(isaOSOPschedule(g));

	// main routine
	exGraph->restoreHiddenGates();
	parProcs->createSubGals();

	if (SimControl::haltRequested()) return FALSE;

	// log the created subgals.
	if (logstrm) {
		*logstrm << parProcs->displaySubUnivs();
		logstrm->flush();
	}

	return TRUE;
}

/////////////////////////////
// mainSchedule 
/////////////////////////////

int ParScheduler :: mainSchedule() {
	// If manual assignment is requested
	if (assignManually()) return scheduleManually();
	// Else, call automatic scheduling routine.
	else {
		if(!scheduleIt()) return FALSE;
		if (OSOPreq()) saveProcIds();
	}
	return TRUE;
}

/////////////////////////////
// saveProcIds
/////////////////////////////

// after automatic scheduling is performed, we save the procId of
// ParNode class to stars. Later we allow the user to override
// the automatic scheduling results.

void ParScheduler :: saveProcIds() {
	GalStarIter iter(*galaxy());
	CGStar* s;
	while ((s = (CGStar*) iter++) != 0) {
		ParNode* n = (ParNode*) s->myMaster();
		s->setProcId(n->getProcId());
	}
}

// Processor id of Fork star is  determined by the ancestor.

int procIdOfFork(CGStar* s) {
	BlockPortIter piter(*s);
	PortHole* p;
	p = piter++;
	while (p->isItOutput()) p = piter++;
	CGStar* farS = (CGStar*) p->far()->parent();
	if (farS->isItFork()) {
		s->setProcId(procIdOfFork(farS));
	} else {
		s->setProcId(farS->getProcId());
	}
	return  s->getProcId();
}

/////////////////////////////
// scheduleManually
/////////////////////////////

// When assignement of all stars are given by user,
// just perform the list scheduling based on that assignment.

int ParScheduler :: scheduleManually() {
	// Make sure all stars are assigned to processors.
	GalStarIter iter(*galaxy());
	CGStar* s;
	while ((s = (CGStar*) iter++) != 0) {
		// If it is a Fork star, assign the procId with the parent's.
		if (s->isItFork() && (s->getProcId() < 0)) {
			procIdOfFork(s);
		}
		int sId = s->getProcId();
		if (sId < 0) {
			Error::abortRun(*s, " is not assigned a processor",
				"Manual assignment fails.");
			return FALSE;
		} else if (sId > numProcs) {
			Error :: abortRun(*s, " procId is out of range",
				"Manual assignment is failed");
			return FALSE;
		} else {
			// set procId of all invocations of the star
			ParNode* n = (ParNode*) s->myMaster();
			CGTarget* t = parProcs->getProc(sId)->target();
			if ((!t->support(s)) || (!t->hasResourcesFor(*s))) {
				Error::abortRun(*t, " is not matched with",
				  s->name());
				return FALSE;
			}

			// base scheduler can not handle parallel stars,
			Profile* pf = s->getProfile();
			if (pf) {
				Error::abortRun("Sorry, parallel task is",
	" not supported. Please use other scheduler, e.g. MacroScheduler.");
				return FALSE;
			}

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
	if (parProcs->listSchedule(exGraph) < 0) return FALSE;
	mtarget->saveCommPattern();
	return parProcs->getMakespan();
}

/////////////////////////////
// virtual methods
/////////////////////////////

// main automatic scheduling routine.
int ParScheduler :: scheduleIt() { return FALSE; }

/////////////////////////////
// compileRun
/////////////////////////////

void ParScheduler :: compileRun() {
	// If 0, be special.
	if (numProcs == 0) {
		SDFScheduler :: compileRun();
		return;
	}
	// run sub-universe in each processor to generate the code
//	parProcs->generateCode();
	if (haltRequested()) { invalid = TRUE; return; }
}

void ParScheduler :: prepareCodeGen() {
    // prepare code generation for each processing element:
    // galaxy initialize, copy schedule, and simulate the schedule.
    parProcs->prepareCodeGen();
}

/////////////////////////////
// setProfile
/////////////////////////////
//
// copy the scheduling results to the argument "profile".
// 
void ParScheduler :: setProfile(Profile* profile) {

	// special for numProcs == 1
	if (numProcs == 1) {
		profile->setEffP(1);
		profile->setStartTime(0,0);
		profile->setFinishTime(0,getTotalWork());
		profile->summary();
		if (EGcreated) {
			int temp = exGraph->pairDistance();
			if (temp < 0) temp = profile->getMakespan();
			profile->setMinDisplacement(temp);
		} else {
			profile->setMinDisplacement(0);
		}
		return;
	}

	int effNum = numProcs;

	for (int i = 0; i < numProcs; i++) {
		UniProcessor* proc = parProcs->getProc(i);

		// set the start & finish time
		profile->setStartTime(i, proc->getStartTime());
		profile->setFinishTime(i, proc->getAvailTime());
		if (!(proc->getAvailTime())) effNum--;
	}
	profile->setEffP(effNum);
	profile->summary();
	int temp = exGraph->pairDistance();
	if (temp < 0) temp = profile->getMakespan();
	profile->setMinDisplacement(temp);
}

int ParScheduler :: finalSchedule() { return TRUE; }

/////////////////////////////
// write Gantt chart
/////////////////////////////

void ParScheduler::writeGantt(ostream& o) {
	int span = parProcs->getMakespan();
	const char* universe = this->exGraph->myGalaxy()->name();
	char tmpbuf[160];
	sprintf(tmpbuf, "ptkGanttDisplay %s", universe);
	Tcl_Eval(ptkInterp, tmpbuf);
	int total = 0;
	for (int i = 0; i < numProcs; i++) {
		UniProcessor* proc = parProcs->getProc(i);
		total += proc->writeGantt(universe, numProcs, span);
	}

	// this stuff is not right
	double p = (100.0 * total) / double(span * numProcs);;
	sprintf(tmpbuf, "ptkGantt_MakeLabel %s %d %d %f %f", universe,
			span, span, p, p);
	Tcl_Eval(ptkInterp, tmpbuf);
	sprintf(tmpbuf, "ptkGantt_Bindings %s %d", universe, numProcs);
	Tcl_Eval(ptkInterp, tmpbuf);
}

/////////////////////////////
// Utilities
/////////////////////////////

int ParScheduler::withParallelStar() {
	DFGalStarIter nextStar(*galaxy());
	CGStar* s;
	while ((s = (CGStar*) nextStar++) != 0) {
		if (s->isParallel()) return TRUE;
	}
	return FALSE;
}

