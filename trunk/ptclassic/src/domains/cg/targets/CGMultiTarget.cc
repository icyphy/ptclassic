static const char file_id[] = "CGMultiTarget.cc";

/******************************************************************
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

 Base target for multiprocessors (fully-connected).

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGMultiTarget.h"
#include "DLScheduler.h"
#include "HuScheduler.h"
#include "DeclustScheduler.h"
#include "KnownTarget.h"
#include "CGSpread.h"
#include "CGCollect.h"
#include "CGSend.h"
#include "CGWormBase.h"
#include "CGReceive.h"
#include "Geodesic.h"
#include "pt_fstream.h"
#include "stdlib.h"
#include "BooleanMatrix.h"

CGMultiTarget::CGMultiTarget(const char* name,const char* sClass,
			     const char* desc) :
	MultiTarget(name,sClass,desc), parProcs(0), rm(0)
{
	addState(childType.setState("childType",this,"default-CG",
				    "child proc types"));
	addState(resources.setState("resources",this,"",
	"define the specific resources of child targets (separated by ;)"));
	addState(relTimeScales.setState("relTimeScales",this,"1",
		"define the relative time scales of child targets"));
	addState(filePrefix.setState("filePrefix",this,"code_proc",
				    "prefix for output code files"));
        addState(ganttChart.setState("ganttChart",this,"YES",
                                     "if true, display Gantt chart"));
        addState(logFile.setState("logFile",this,"",
                                     "log file to write to (none if empty)"));
        addState(ammortizedComm.setState("ammortizedComm",this,"NO",
                                     "try to ammortize communication?"));
	addState(ignoreIPC.setState("ignoreIPC",this,"NO",
				   "ignore communication cost?"));
	addState(overlapComm.setState("overlapComm",this,"NO",
				   "processor can overlap communication?"));
	addState(useCluster.setState("useCluster",this,"NO",
				   "Use Gil's declustering algorithm?"));
	addedStates.initialize();
}

CGMultiTarget::~CGMultiTarget() {
	if (!inherited()) deleteChildren();
	delSched();
	addedStates.deleteAll();
	LOG_DEL; delete rm;
}

Block* CGMultiTarget::makeNew() const {
	LOG_NEW; return new CGMultiTarget(name(),starType(),descriptor());
}

	/////////////////////////
	// setup, wrapup
	/////////////////////////

void CGMultiTarget::setup() {
	// prepare child targets
	prepareChildren();
	if (SimControl :: haltRequested()) return;

	// check whether communication can be amortized or not.
	// If YES, setup the  Reachability matrix.
	if ((nChildrenAlloc > 1) && int(ammortizedComm)) {
		LOG_NEW; 
		rm = new BooleanMatrix(nChildrenAlloc, nChildrenAlloc); 
	}

	//choose the right scheduler
	chooseScheduler();
	
	// in case of heterogeneous targets, flatten the wormholes.
	if (childType.size() > 1) {
		flattenWorm();
	}

	ParScheduler* sched = (ParScheduler*) scheduler();
	sched->setGalaxy(*galaxy());
	sched->setUpProcs(nChildrenAlloc);
	parProcs = sched->myProcs();

	canProcs.create(nChildrenAlloc);

	// CG stuff
	writeDirectoryName(destDirectory);
	myCode.initialize();

	if (galaxy()->parent()) {
		sched->ofWorm();
	}
	Target :: setup();

	if (!inherited()) {
		writeSchedule();
		if (inWormHole()) {
		   	adjustSampleRates();
			generateCode();
			wormLoadCode();
		}
	}
}

void CGMultiTarget :: prepareChildren() {
	if (!inherited()) {
		deleteChildren();
		nChildrenAlloc = nprocs;
		if (childType.size() > nChildrenAlloc) {
			Error :: warn("too many child types are",
			  " sepcified: look at [nprocs] parameter");
		}
		if (childType.size() != relTimeScales.size()) {
			Error :: warn("unmatched number of parameters: ",
			"[childType] and [relTimeScales].\n",
			"By default, we assume the same time scale, 1");
		}
		StringList tname;
		for (int i = 0; i < nChildrenAlloc; i++) {
			Target* t = createChild(i);
			if (!t) return;
			addChild(*t);
			tname.initialize();
			tname << (const char*) filePrefix << i;
			t->setNameParent(hashstring(tname),this);
		}
		resourceInfo();
	   	for (i = 0; i < nChildrenAlloc; i++) {
			child(i)->initialize();
	   	}
	}
}

Target* CGMultiTarget :: createChild(int i) {
	int ix = childType.size();
	const char* temp;
	if (i < ix) temp = childType[i];
	else temp = childType[ix-1];
	return KnownTarget::clone(temp);
}

// parse the resource parameter and adjust the resources parameter of
// child targets.
void CGMultiTarget :: resourceInfo() {
	int sz = resources.size();
	const char* item;
	int start = TRUE;
	int no = -1;
	StringList added;
	for (int i = 0; i < sz; i++) {
		item = resources[i];
		if (!strcmp(item, "")) return;
		if (start) {
			no = atoi(item);
			StringList temp = no;
			if (strcmp(item, temp) || 
				(no < 0) || (no > nChildrenAlloc)) {
				Error :: abortRun(item, ":unknown proc Id");
				return;
			}
			added.initialize();
			start = FALSE;
		} else if (!strcmp(item, ";")) {
			start = TRUE;
		} else {
			added << item;
		}
		if (start || (i == sz - 1)) {
			// add resources to the child targets.
			Target* t = child(no);
			State* s = t->stateWithName("resources");
			StringArrayState* as = 0;
			if (!s) {
				LOG_NEW; as = new StringArrayState;
				addedStates.put(*as);
				t->addState(as->setState("resources",t,"",""));
			} else if (s->isA("StringArrayState")) {
				as = (StringArrayState*) s;
			} else {
				Error::abortRun(*s,": not a StringArrayState");
				return;
			}
			StringList cur = as->currentValue();
			cur << added;
			as->setInitValue(hashstring(cur));
		} 
	}
}
		
// select the scheduler depending on the options.
void CGMultiTarget :: chooseScheduler() {
	delSched();
	if (int(ignoreIPC)) {
		LOG_NEW; setSched(new HuScheduler(this, logFile));
	} else if (int(overlapComm)) {
		LOG_NEW; setSched(new DLScheduler(this, logFile, 0));
	} else if (int(useCluster)) {
		if (childType.size() > 1) {
			Error :: warn("Declustering technique can not be",
			  "applied for heterogeneous targets.\n By default",
			  "we use dynamic-level scheduling");
			LOG_NEW; setSched(new DLScheduler(this, logFile, 1));
		} else if (resources.size() > 1) {
			Error :: warn("Declustering technique can not be",
			  "applied with resource restriction.\n By default",
			  "we use dynamic-level scheduling");
			LOG_NEW; setSched(new DLScheduler(this, logFile, 1));
		} else {
			LOG_NEW; setSched(new DeclustScheduler(this, logFile));
		}
	} else {
		LOG_NEW; setSched(new DLScheduler(this, logFile, 1));
	}
}

void CGMultiTarget :: flattenWorm() {
	Galaxy* myGalaxy = galaxy();
	GalStarIter next(*myGalaxy);
	CGStar* s;
	CGStar* prev = 0;
	Galaxy* prevG = myGalaxy;
	int changeFlag = FALSE;
	while ((s = (CGStar*) next++) != 0) {
		if (prev) {
			prevG->removeBlock(*prev);
			LOG_DEL; delete prev;
			prev = 0;
		}
		if (s->isItWormhole()) {
			CGWormBase* w = s->myWormhole();
			// if inside domain is a CG domain, explode wormhole.
			prevG = (Galaxy*) s->parent();
			if (w->isCGinside()) {
				prev = s;
				Galaxy* inG = w->explode();
				prevG->addBlock(*inG, inG->name());
				changeFlag = TRUE;
			}
		}
	}
	if (prev) {
		prevG->removeBlock(*prev);
		LOG_DEL; delete prev;
	}

	// recursive application of this routine.
	if (changeFlag) flattenWorm();
}
				
void CGMultiTarget :: setStopTime(double f) {
	Target::setStopTime(f);

	// For child targets
	if (!inherited()) {
		for (int i = 0; i < nChildrenAlloc; i++) {
			child(i)->setStopTime(f);
		}
	}
}

// redefine beginIteration method to set the stop time of the child target
// do nothing for endIteration
void CGMultiTarget :: beginIteration(int num, int) {
	setStopTime(num);
}

void CGMultiTarget :: endIteration(int, int) {}


extern "C" int displayGanttChart(const char*);

// Display Gantt chart if requested.
void CGMultiTarget::writeSchedule() {
	if (nChildrenAlloc == 1) return;
	ParScheduler* s = (ParScheduler*) scheduler();
        if (int(ganttChart)) {
		char* gname = tempFileName();
		pt_ofstream o(gname);
		if (o) {
			s->writeGantt(o);
			displayGanttChart(gname);
			unlink(gname);
		}
		LOG_DEL; delete gname;
        }
}

void CGMultiTarget::wrapup() {

	StringList logMsg;
	for (int i = 0; i < nProcs(); i++) {
		// write out generated code.
		StringList name = (const char*)filePrefix;
		name += i;
		CGTarget* nextChild = (CGTarget*)child(i);
		nextChild->writeCode(name);
		logMsg += "code for ";
		logMsg += nextChild->fullName();
		logMsg += " written to ";
		logMsg += (const char*)filePrefix;
		logMsg += i;
		logMsg += "\n";
	}
	Error::message(logMsg);
}

	/////////////////////
	// Wormhole support
	/////////////////////

// Wormhole support routines are tricky to interpret. The major difficulty
// comes from the fact that one EventHorizon may be connected to more than
// subGalaxies which can not be expressed by regular porthole connection.
// So, we examine the stars at the wormhole boudnary (via expandedgraph nodes)
// instead of portholes, make a temporary connection between EH and 
// the corresponding porthole, and do things -> major hack!.

int CGMultiTarget :: run() {
	if((inWormHole() == FALSE) || (nChildrenAlloc == 1))
		return CGTarget :: run();

	// if a wormhole, setup already generated code.
	// We must do the transfer of data to and from the target.
	if (!sendWormData()) return FALSE;
	return receiveWormData();
}

int CGMultiTarget :: sendWormData() {
	if (nChildrenAlloc == 1) {
		CGTarget* t = (CGTarget*) child(0);
		return t->receiveWormData();
	}

	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItInput()) {
			PortHole& realP = p->newConnection();
			Geodesic* realG = realP.far()->geo();
			ParNode* pn = (ParNode*)
				((DataFlowStar*) realP.parent())->myMaster();
			while (pn) {
			    CGStar* s = (CGStar*) pn->getCopyStar();
			    PortHole* tp = s->portWithName(realP.name());
			    realG->setDestPort(*tp);
			    if(!sendWormData(*tp)) return FALSE;
			    pn = (ParNode*) pn->getNextInvoc();
			}
		}
	}
	return !Scheduler::haltRequested();
}

int CGMultiTarget :: receiveWormData() {
	if (nChildrenAlloc == 1) {
		CGTarget* t = (CGTarget*) child(0);
		return t->receiveWormData();
	}

	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItOutput()) {
			PortHole& realP = p->newConnection();
			Geodesic* realG = realP.far()->geo();
			ParNode* pn = (ParNode*)
				((DataFlowStar*) realP.parent())->myMaster();
			while (pn) {
			    CGStar* s = (CGStar*) pn->getCopyStar();
			    PortHole* tp = s->portWithName(realP.name());
			    realG->setSourcePort(*tp);
			    if(!receiveWormData(*tp)) return FALSE;
			    pn = (ParNode*) pn->getNextInvoc();
			}
		}
	}
	return !Scheduler::haltRequested();
}

// default code generation for a wormhole.  This produces an infinite
// loop that reads from the inputs, processes the schedule, and generates
// the outputs.
// Note that wormOutputCode is called before compileRun(). In compileRun(),
// all child targets will finish generating code.

void CGMultiTarget :: generateCode() {
	if (nChildrenAlloc == 1) {
		CGTarget* t = (CGTarget*) child(0);
		t->generateCode();
		addProcessorCode(0,(*t->getStream("code")));
		return;
	}

	if (parent()) setup();		// check later whether this is right.

	int iterations = inWormHole()? -1 : (int)scheduler()->getStopTime();
        beginIteration(iterations,0);
	if (inWormHole()) {
		wormInputCode();
		wormOutputCode();	// note the change of calling order.
	}
	scheduler()->compileRun();
        endIteration(iterations,0);
}
	
// Trickiness again for wormholes. 
// Since more than one ParNodes are correspond to one star in a subGal,
// we are careful in searching the stars at the wormhole boundary!

void CGMultiTarget :: wormInputCode() {
	LOG_NEW; int* iprocs = new int[nChildrenAlloc];
	for (int i = 0; i < nChildrenAlloc; i++)
		iprocs[i] = 0;

        BlockPortIter nextPort(*galaxy());
        PortHole* p;
        // generate wormhole inputs
        while ((p = nextPort++) != 0) {
                if (p->isItInput()) {
			PortHole& realP = p->newConnection();
			ParNode* pn = (ParNode*)
				((DataFlowStar*) realP.parent())->myMaster();
			while (pn) {
				int ip = pn->getProcId();
				if (iprocs[ip] == 0) {
					iprocs[ip] = 1;
					CGStar* s = (CGStar*)pn->getCopyStar();
					s->myTarget()->wormInputCode(
					 *(s->portWithName(realP.name())));
				}
				pn = (ParNode*) pn->getNextInvoc();
			}
		}
        }
	LOG_DEL; delete [] iprocs;
}

void CGMultiTarget :: wormOutputCode() {
	LOG_NEW; int* iprocs = new int[nChildrenAlloc];
	for (int i = 0; i < nChildrenAlloc; i++)
		iprocs[i] = 0;

        // generate wormhole outputs
        BlockPortIter nextPort(*galaxy());
        PortHole* p;
        while ((p = nextPort++) != 0) {
                if (p->isItOutput()) {
			PortHole& realP = p->newConnection();
			ParNode* pn = (ParNode*)
				((DataFlowStar*) realP.parent())->myMaster();
			while (pn) {
				int ip = pn->getProcId();
				if (iprocs[ip] == 0) {
					iprocs[ip] = 1;
					CGStar* s = (CGStar*)pn->getCopyStar();
					s->myTarget()->wormOutputCode(
					 *(s->portWithName(realP.name())));
				}
				pn = (ParNode*) pn->getNextInvoc();
			}
		}
        }
	LOG_DEL; delete [] iprocs;
}

// Use for nChildren == 1.
void CGMultiTarget :: wormInputCode(PortHole& p) {
	((CGTarget*) child(0))->wormInputCode(p);
}

void CGMultiTarget :: wormOutputCode(PortHole& p) {
	((CGTarget*) child(0))->wormInputCode(p);
}
	///////////////////////////////
	// parallel scheduler support
	///////////////////////////////

// IPC stuff
int CGMultiTarget::commTime(int, int, int nSamps, int type) {
        int cost = nSamps * int(sendTime);
        return type == 2 ? 2*cost : cost;
}

// return the array of candidate processors
IntArray* CGMultiTarget :: candidateProcs(ParProcessors* parSched,
	DataFlowStar* s) {
	
	// clear the array.
	int k = 0;
	int flag = 0;
	int temp = -1;

	// heterogeneous case
	int hetero = (childType.size() > 1) || (relTimeScales.size() > 1);

	// Scan the processors. By default, include only one
	// unused processor and all used processors which satisfy
	// resource constraints.
	for (int i = 0; i < parSched->size(); i++) {
		UniProcessor* uni = parSched->getProc(i);
		CGTarget* t = uni->target();
		if (s && t && !t->support(s)) continue;
		else if (s && t && !childHasResources(*s, i)) continue;
		else if (uni->getAvailTime() || hetero) {
			canProcs[k] = i;
			k++;
		} else if (!flag) {
			temp = i;
			flag = 1;
		} 
	}
	if (temp >= 0) { canProcs[k] = temp; k++; }
	canProcs.truncate(k);
	return &canProcs;
}
	
ParNode* CGMultiTarget :: backComm(ParNode* n) {
	if (n->getType() >= -1) return 0;
	ParAncestorIter iter(n);
	return (iter++);
}

void CGMultiTarget :: resetResources() {}			

// create communication stars
DataFlowStar* CGMultiTarget :: createSend(int from, int to, int /*num*/) {
	LOG_NEW; CGSend* newS = new CGSend;
	newS->registerProcs(from, to);
	return newS;
}

DataFlowStar* CGMultiTarget :: createReceive(int from, int to, int /*num*/) {
	LOG_NEW; CGReceive* newS = new CGReceive;
	newS->registerProcs(from, to);
	return newS;
}

// create Spread and Collect
DataFlowStar* CGMultiTarget :: createSpread() {
	LOG_NEW; return (new CGSpread);
}
DataFlowStar* CGMultiTarget :: createCollect() {
	LOG_NEW; return (new CGCollect);
}
	
// return the execution of a star if scheduled on the given target.
// If the target does not support the star, return -1.
int CGMultiTarget :: execTime(DataFlowStar* s, CGTarget* t) {
	if ((!t) || (relTimeScales.size() <= 1)) return s->myExecTime();
	else if (!t->support(s)) return -1;

	// for heterogeneous case, we need to consider relative time
	// scale
	for (int i = 0; i < nChildrenAlloc; i++) 
		if (child(i) == t) break;
	int mx = relTimeScales.size() - 1;
	if (i > mx) i = mx;
	return relTimeScales[i] * t->execTime(s);
}
		
	///////////////////////////////
	// Communication Amortization
	///////////////////////////////

// update the matrix from createSend() method.
void CGMultiTarget :: updateRM(int from, int to) {
	if (!rm) return;
	rm->setElem(from, to, 1);
	rm->orRows(from, to);
}

// return TRUE if ammortization is possible, FALSE otherwise.
int CGMultiTarget :: ammortize(int from, int to) {
	if (!rm) return FALSE;
	int x = rm->getElem(from, to) + rm->getElem(to, from);
	if (x < 2) return TRUE;
	else return FALSE;
}

	/////////////////////////
	// CGDDF support
	/////////////////////////

void CGMultiTarget :: setProfile(Profile* p) {
	((ParScheduler*) scheduler())->setProfile(p);
}

// flag = -2 when manual scheduling option is taken (final stage).
// flag = -1 indicate the final scheduling from the automatic scheduling
// flag = 1 is the setup stage scheduling in the automatic scheduling.

int CGMultiTarget :: computeProfile(int nP, int flag, IntArray* procMap) {
	ParScheduler* qs = (ParScheduler*) scheduler();
	if (flag >= -1) {
		resetResources();
		qs->setUpProcs(nP);
		if (!qs->mainSchedule()) return FALSE;
	}
	if ((flag == -1) || ((flag == -2) && (nP > 1))) {
		qs->mapTargets(procMap);
		if (!qs->createSubGals(*galaxy())) return FALSE;
		return qs->finalSchedule();
	} 
	return TRUE;
}

int CGMultiTarget :: totalWorkLoad() {
        return ((ParScheduler*) scheduler())->getTotalWork();
}

int CGMultiTarget::downLoadCode(int index, int pId, Profile*) {
	if (nChildrenAlloc == 1) {
		ParScheduler* ps = (ParScheduler*) scheduler();

		// set the numProcs of ParScheduler to be zero to call the
		// SDFScheduler :: compileRun() method. Refer to the
		// ParScheduler :: compileRun() method when numProcs = 0.

		ps->setUpProcs(0);
		CGTarget* t = (CGTarget*) child(pId);
		GalStarIter next(*galaxy());
		Star* s;
		while ((s = next++) != 0) s->setTarget(t);

		return t->insertGalaxyCode(galaxy(), ps);
	} else {
		return ((ParScheduler*)scheduler())->
			getProc(index)->genCodeTo(child(pId));
	}
}

ISA_FUNC(CGMultiTarget, MultiTarget);

static CGMultiTarget targ("FullyConnected","CGStar",
"Fully-connected targets for parallel scheduling");

static KnownTarget entry(targ,"FullyConnected");

