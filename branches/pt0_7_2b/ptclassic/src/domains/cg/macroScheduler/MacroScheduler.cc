static const char file_id[] = "MacroScheduler.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 

MacroScheduler: efficient and highly flexible parallel scheduler

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "MacroScheduler.h"
#include "CGMacroCluster.h"
#include "Target.h"
#include "Error.h"
#include "pt_fstream.h"


////////////////////////////
// Macro Scheduler methods //
////////////////////////////

void MacroScheduler :: setup() {
	if (!galaxy()) {
		Error:: abortRun("MacroScheduler: no galaxy!");
		return;
	}
	invalid = FALSE;

	checkConnectivity();
	if (invalid) return;

	prepareGalaxy();

	// do without repetitions at this moment
	// schedule the graph
	computeSchedule(*galaxy());
	if (invalid) return;

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

CGMacroClusterGal* MacroScheduler :: createClusterGal(Galaxy* g, ostream* l) {
	LOG_NEW; return new CGMacroClusterGal(g,l);
}

//
// Main routine for MacroScheduler
//
int MacroScheduler::computeSchedule(Galaxy& g)
{
        // make parallel target intervene here to do something necessary
        // before scheduling stars: e.g. setting machine_Information.
        // by default, do nothing
        if (mtarget->prepareSchedule() == FALSE) return FALSE;

	LOG_DEL; delete mGal;

	// log file stuff.
	ostream* logstrm = 0;
	if (logFile && *logFile) {
		logstrm = new pt_ofstream(logFile);
		if (!*logstrm) {
			LOG_DEL; delete logstrm;
			return FALSE;
		}
	}

	mGal = createClusterGal(&g, logstrm);


	// Step 1. Create CGMacroClusterGal and perform Galaxy clustering 
	//         (1) when requested with a galaxy parameter.
	//	   (2) default when graphical/textual representation is used.
	if(!mGal->clusterGalaxy(g)) { invalid = TRUE; return FALSE; }

	// Step 2. compute repetitions property of SDF type stars
	setGalaxy(*mGal);

	repetitions();
	if (Scheduler :: haltRequested()) return FALSE;

	CGMacroClusterGalIter nextClust(*mGal);
	CGMacroCluster* c;
	while ((c = nextClust++) != 0) {
		CGMacroClusterBag* cbag = c->asSpecialBag();
		if (cbag) {
			if (!cbag->prepareBag(mtarget, logstrm))
				return FALSE;
		}
	}

	// Step 3. If galaxy is enforced to be clustered, processor ids of the
	// assigned processors should be given as galaxy parameters!
	if (assignManually()) {
		if(!scheduleManually()) {
			invalid = TRUE;
			return FALSE;
		}
		return TRUE;
	}

	// Step 4. global loop clusters
	// FIXME: for now, bypass loop clustering.
	// if(!mGal->globalCluster()) { invalid = TRUE; return FALSE; }

	// Step 5. parallel scheduling of inside clusters
	if(!mGal->recursiveSched(mtarget)) { invalid = TRUE; return FALSE; }

	// Step 6. global scheduling: PRAM model

	generateSchedule(mGal); 
	if (Scheduler :: haltRequested()) { invalid = TRUE; return FALSE; }


	// Step 7. finalize the scheduling of clusters
	if (!assignManually()) {
		if (!finalSchedule()) { invalid = TRUE; return FALSE; }
	}

	if (logstrm) {
		*logstrm << "Schedule:\n" << displaySchedule();
		logstrm->flush();
		LOG_DEL; delete logstrm;
	}
	return TRUE;
}

int MacroParSched :: repetitions() {
	invalid = 0;
	return SDFScheduler :: repetitions();
}

int MacroParSched :: generateSchedule(CGMacroClusterGal* mgal) {
	exGraph->setLog(logstrm);

	// form expanded graph
	if (!exGraph->createMe(*mgal, OSOPreq())) {
		Error :: abortRun("Could not create expanded graph");
		invalid = TRUE;
		return FALSE;
	}

	// compute the total (expected) workload with a single processor
	totalWork = exGraph->getExecTotal();
	mapTargets();
	
	// schedule it
	int makespan = mainSchedule();
	if (Scheduler :: haltRequested()) {
		invalid = TRUE;
		return FALSE;
	}
	
	return makespan;
}


/////////////////////////////
// setUpProcs
/////////////////////////////

void MacroParSched :: setUpProcs(int num) {
	ParScheduler :: setUpProcs(num);
	LOG_DEL; if (parSched) delete parSched;
	LOG_NEW; parSched = new MacroParProcs(numProcs, mtarget);
	parProcs = parSched;
}

MacroParSched :: ~MacroParSched() {
	LOG_DEL; if (parSched) delete parSched;
	LOG_DEL; delete myGraph;
}

/////////////////////////////
// scheduleIt
/////////////////////////////

int MacroParSched :: scheduleIt()
{
  	parSched->initialize(myGraph);

  	// reset the graph for the new parallel schedule
  	myGraph->resetGraph();

	// reset Target
	mtarget->clearCommPattern();

  	// schedule the runnable nodes until there is no more unscheduled node
  	// or the graph is deadlocked.
  	MacroNode* node;
	while ((node = (MacroNode*) myGraph->fetchNode()) != 0) {

	   // read the star
	   CGMacroCluster* obj = (CGMacroCluster*) node->myMaster();

	   // check the atomicity of the star
	   if (!obj->isParallel()) {
	   	parSched->scheduleSmall(node);	

	   } else if ((!obj->isParallelizable()) && 
			node->invocationNumber() > 1) {
		node->withProfile(obj->getProfile(numProcs-1));
		parSched->copyBigSchedule(node, avail);

	   } else {	// parallel task.
		CGMacroClusterBag* cbag = obj->asSpecialBag();

	   	// determine the pattern of processor availability.
		// And return the schedule time.
	   	int when = parSched->determinePPA(node, avail);
		if (when < 0) return FALSE;

	   	// compute the work-residual which can be scheduled in 
	   	// parallel with this node.  
	   	// If the residual work is too small, we may 
	   	// want to devote more processors to this node.
	   	int resWork = myGraph->sizeUnschedWork() - node->workAfterMe();

		// compute the optimal number of the assigned processors
		// for macro node (data-parallel, cluster, or CGDDF nodes)
	   	if (!cbag->computeProfile(numProcs, resWork, &avail))
			return FALSE;
           	if (haltRequested()) return FALSE;

		node->withProfile(cbag->getProfile(numProcs-1));
	   	parSched->scheduleBig(node, when, avail);
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
// scheduleManually
/////////////////////////////

// Processor id of Fork star is  determined by the ancestor.

int MacroParSched :: procIdOfFork(CGMacroCluster* s) {
	CGClustPortIter piter(*s);
	CGClustPort* p;
	p = piter++;
	while (p->isItOutput()) p = piter++;
	CGMacroCluster* farS = p->far()->parentClust();
	if (farS->asBag()) {
		Error::abortRun("Sorry, autoFork is not disallowed after",
	   "clustered galaxy in manual scheduling options. Use a Fork star.");
		return -1;
	}
	if (farS->isItFork()) {
		int temp = procIdOfFork(farS);
		if (temp < 0) return -1;
		s->setProcId(temp);
	} else {
		s->setProcId(farS->getProcId());
	}
	return  s->getProcId();
}

// When assignement of all stars are given by user,
// just perform the list scheduling based on that assignment.

int MacroParSched :: scheduleManually() {
	exGraph->setLog(logstrm);

	// form expanded graph
	if (!exGraph->createMe(*galaxy(), OSOPreq())) {
		Error :: abortRun("Could not create expanded graph");
		invalid = TRUE;
		return FALSE;
	}

	totalWork = exGraph->getExecTotal();
	mapTargets();

	// compute the total (expected) workload with a single processor
	// Make sure all stars are assigned to processors.
	CGMacroClusterGal* clustGal = (CGMacroClusterGal*) galaxy();
	CGMacroClusterGalIter iter(*clustGal);
	CGMacroCluster* s;
	while ((s = iter++) != 0) {
		if (s->asSpecialBag()) {
			// for parallel stars, set up profile of ParNode.
			Profile* pf = s->getProfile();
			pf = s->asSpecialBag()->manualSchedule(s->reps());

			if (!pf) {
				invalid = TRUE;
				return FALSE;
			}

			MacroNode* n = (MacroNode*) s->myMaster();
			while (n) {
				n->setProcId(s->getProcId());
				n->withProfile(pf);
				n = (MacroNode*) n->getNextInvoc();
			}
		} else {
			CGAtomCluster* as = (CGAtomCluster*) s;
			// If it is a Fork, assign procId with the parent's.
			if (as->isItFork() && (as->getProcId() < 0)) {
				if (procIdOfFork(as) < 0) return FALSE;
			}
			int sId = as->getProcId();
			if (sId < 0) {
				Error::abortRun(as->real(), " is not assigned"
				" a processor. Manual assignment fails.");
				return FALSE;
			} else if (sId > numProcs) {
				Error :: abortRun(as->real(), " procId is out"
				" of range. Manual assignment is failed");
				return FALSE;
			} else {
				// set procId of all invocations of the star
				MacroNode* n = (MacroNode*) as->myMaster();
				CGTarget* t = parProcs->getProc(sId)->target();
				if ((!t->support(&as->real())) || 
					(!t->hasResourcesFor(as->real()))) {
				   Error::abortRun(*t, " is not matched with",
				   as->real().name());
				   return FALSE;
				}
				while (n) {
					n->setProcId(as->getProcId());
					n = (MacroNode*) n->getNextInvoc();
				}
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
// finalSchedule
/////////////////////////////
// At the last stage of scheduling, make the final schedule for clusters

int MacroParSched :: finalSchedule() {

	CGMacroClusterGal* clustGal = (CGMacroClusterGal*) galaxy();
	CGMacroClusterGalIter iter(*clustGal);
	CGMacroCluster* s;
	while ((s = iter++) != 0) {
		CGMacroClusterBag* cbag = s->asSpecialBag();
		if (cbag) {
			 if (!cbag->finalSchedule(numProcs)) return FALSE;
		}
	}
	return TRUE;
}

/////////////////////////////
// displaySchedule
/////////////////////////////

StringList MacroParSched :: displaySchedule() {

	CGMacroClusterGal* clustGal = (CGMacroClusterGal*) myGraph->myGalaxy();

	StringList out;
	out += parSched->display(clustGal->realGal());

	// display macro schedule
	out += "*********** macro Schedules ************\n";
	CGMacroClusterGalIter next(*clustGal);
	CGMacroCluster* s;
	while ((s = next++) != 0) {
		CGMacroClusterBag* cbag = s->asSpecialBag();
		if (cbag) {
			out += "-------- \n\n";
			out += cbag->displaySchedule();
		}
	}

	return out;
}

