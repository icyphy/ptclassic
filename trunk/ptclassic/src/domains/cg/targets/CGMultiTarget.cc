static const char file_id[] = "CGMultiTarget.cc";

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

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
#include "CGReceive.h"
#include "Geodesic.h"
#include "pt_fstream.h"

CGMultiTarget::CGMultiTarget(const char* name,const char* sClass,
			     const char* desc) :
	MultiTarget(name,sClass,desc), parProcs(0)
{
	addState(childType.setState("childType",this,"default-CG",
				    "child proc type"));
	addState(filePrefix.setState("filePrefix",this,"code.p",
				    "prefix for output code files"));
        addState(ganttChart.setState("ganttChart",this,"YES",
                                     "if true, display Gantt chart"));
        addState(logFile.setState("logFile",this,"",
                                     "log file to write to (none if empty)"));
	addState(ignoreIPC.setState("ignoreIPC",this,"NO",
				   "ignore communication cost?"));
	addState(overlapComm.setState("overlapComm",this,"NO",
				   "processor can overlap communication?"));
	addState(useCluster.setState("useCluster",this,"YES",
				   "Use Gil's declustering algorithm?"));
	oldChildType[0] = 0;
}

CGMultiTarget::~CGMultiTarget() {
	if (!inherited()) deleteChildren();
	delSched();
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

	//choose the right scheduler
	chooseScheduler();
	
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
			generateCode();
			wormLoadCode();
		}
	}
}

void CGMultiTarget :: prepareChildren() {
	if (!inherited()) {
	   if (nChildrenAlloc != int(nprocs) 
			|| strcmp(childType,oldChildType) != 0) {
		deleteChildren();
		nChildrenAlloc = nprocs;
		strcpy (oldChildType, childType);
		for (int i = 0; i < nChildrenAlloc; i++) {
			Target* t = createChild();
			if (!t) return;
			addChild(*t);
		}
	   }
	   for (int i = 0; i < nChildrenAlloc; i++) {
		child(i)->initialize();
	   }
	}
}

Target* CGMultiTarget :: createChild() {
	return KnownTarget::clone(childType);
}

void CGMultiTarget :: chooseScheduler() {

	delSched();
	if (int(ignoreIPC)) {
		LOG_NEW; setSched(new HuScheduler(this, logFile));
	} else if (int(overlapComm)) {
		LOG_NEW; setSched(new DLScheduler(this, logFile, 0));
	} else if (int(useCluster)) {
		LOG_NEW; setSched(new DeclustScheduler(this, logFile));
	} else {
		LOG_NEW; setSched(new DLScheduler(this, logFile, 1));
	}
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

int CGMultiTarget :: run() {
	if((inWormHole() == FALSE) || (nChildrenAlloc == 1))
		return CGTarget :: run();

	// if a wormhole, setup already generated code.
	// We must do the transfer of data to and from the target.
	if (!sendWormData()) return FALSE;
	return receiveWormData();
}

int CGMultiTarget :: sendWormData() {
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
void CGMultiTarget :: generateCode() {
	if (nChildrenAlloc == 1) {
		CGTarget :: generateCode();
		return;
	}

	if (parent()) setup();		// check later whether this is right.

        beginIteration(-1,0);
	if (inWormHole()) {
		wormInputCode();
		wormOutputCode();	// note the change of calling order.
	}
	scheduler()->compileRun();
        endIteration(-1,0);
}
	
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
IntArray* CGMultiTarget :: candidateProcs(ParProcessors* parSched) {
	
	// clear the array.
	int k = 0;
	int flag = 0;
	int temp = -1;

	// Scan the processors. By default, include only one
	// unused processor and all used processors.
	for (int i = 0; i < parSched->size(); i++) {
		UniProcessor* uni = parSched->getProc(i);
		if (uni->getAvailTime()) {
			canProcs[k] = i;
			k++;
		} else if (!flag) {
			temp = i;
			flag = 1;
		}
	}
	if (temp > 0) { canProcs[k] = temp; k++; }
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
	
	/////////////////////////
	// CGDDF support
	/////////////////////////

void CGMultiTarget :: setProfile(Profile* p) {
	((ParScheduler*) scheduler())->setProfile(p);
}

int CGMultiTarget :: computeProfile(int nP, int flag, IntArray*) {
	resetResources();
	ParScheduler* qs = (ParScheduler*) scheduler();
	qs->setUpProcs(nP);
	int temp = qs->mainSchedule();
	if (flag < 0) qs->finalSchedule();
	return temp;
}

int CGMultiTarget :: totalWorkLoad() {
        return ((ParScheduler*) scheduler())->getTotalWork();
}

void CGMultiTarget :: insideSchedule() {
	if (nChildrenAlloc > 1)
 		((ParScheduler*) scheduler())->sortProcessors();
}

void CGMultiTarget::downLoadCode(int pId, Profile*) {
	if (nChildrenAlloc == 1) 
		((ParScheduler*)scheduler())->compileRun();
	else
		((ParScheduler*)scheduler())->getProc(pId)->run();
}

static CGMultiTarget targ("FullyConnected","CGStar",
"Fully-connected targets for parallel scheduling");

static KnownTarget entry(targ,"FullyConnected");

