static const char file_id[] = "CGFullConnect.cc";

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

#include "CGFullConnect.h"
#include "DLScheduler.h"
#include "QuasiScheduler.h"
#include "DeclustScheduler.h"
#include "KnownTarget.h"
#include "CGSend.h"
#include "CGReceive.h"
#include "Geodesic.h"
#include "pt_fstream.h"

CGFullConnect::CGFullConnect(const char* name,const char* sClass,
			     const char* desc) :
	BaseMultiTarget(name,sClass,desc), parProcs(0)
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

CGFullConnect::~CGFullConnect() {
	if (!inherited()) deleteChildren();
	delSched();
}

Block* CGFullConnect::clone() const {
	LOG_NEW; CGFullConnect *t = new CGFullConnect(readName(),starType(),readDescriptor());
	return &t->copyStates(*this);
}

	/////////////////////////
	// start, setup, wrapup
	/////////////////////////

void CGFullConnect::start() {
	if (!inherited()) {
	   if (nChildrenAlloc != int(nprocs) || strcmp(childType,oldChildType) != 0) {
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
	//choose the right scheduler
	chooseScheduler();

	ParScheduler* tempSched = (ParScheduler*) mySched();
	tempSched->setUpProcs(nChildrenAlloc);
	parProcs = tempSched->myProcs();

	canProcs.create(nChildrenAlloc);

	// CG stuff
	LOG_DEL; delete dirFullName;
	dirFullName = writeDirectoryName(destDirectory);
	myCode.initialize();
}

Target* CGFullConnect :: createChild() {
	return KnownTarget::clone(childType);
}
	
void CGFullConnect :: chooseScheduler() {

	delSched();
	if (int(ignoreIPC)) {
		LOG_NEW; setSched(new QuasiScheduler(this, logFile));
	} else if (int(overlapComm)) {
		LOG_NEW; setSched(new DLScheduler(this, logFile, 0));
	} else if (int(useCluster)) {
		LOG_NEW; setSched(new DeclustScheduler(this, logFile));
	} else {
		LOG_NEW; setSched(new DLScheduler(this, logFile, 1));
	}
}

void CGFullConnect :: setStopTime(float f) {
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
void CGFullConnect :: beginIteration(int num, int) {
	setStopTime(float(num));
}

void CGFullConnect :: endIteration(int, int) {}

int CGFullConnect::setup(Galaxy& gal) {
	if (gal.parent()) {
		((ParScheduler*) mySched())->ofWorm();
	}
	return Target :: setup(gal);
}


extern "C" int displayGanttChart(const char*);

// Display Gantt chart if requested.
void CGFullConnect::displaySchedule(ParScheduler* s) {
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

void CGFullConnect::wrapup() {

	displaySchedule((ParScheduler*) mySched());
	StringList logMsg;
	for (int i = 0; i < nProcs(); i++) {
		// write out generated code.
		StringList name = (const char*)filePrefix;
		name += i;
		pt_ofstream out(name);
		if (!out) return;
		CGTarget* nextChild = (CGTarget*)child(i);
		nextChild->writeCode(out);
		logMsg += "code for ";
		logMsg += nextChild->readFullName();
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

int CGFullConnect :: run() {
	if(gal->parent() == 0) return CGTarget :: run();

	// if a wormhole, setup already generated code.
	// We must do the transfer of data to and from the target.
	BlockPortIter nextPort(*gal);
	PortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItInput()) {
			PortHole& realP = p->newConnection();
			Geodesic* realG = realP.far()->myGeodesic;
			ParNode* pn = (ParNode*)
				((SDFStar*) realP.parent())->myMaster();
			while (pn) {
			    CGStar* s = (CGStar*) pn->getCopyStar();
			    PortHole* tp = s->portWithName(realP.readName());
			    realG->setDestPort(*tp);
			    if(!sendWormData(*tp)) return FALSE;
			    pn = (ParNode*) pn->getNextInvoc();
			}
		}
	}
	nextPort.reset();
	while ((p = nextPort++) != 0) {
		if (p->isItOutput()) {
			PortHole& realP = p->newConnection();
			Geodesic* realG = realP.far()->myGeodesic;
			ParNode* pn = (ParNode*)
				((SDFStar*) realP.parent())->myMaster();
			while (pn) {
			    CGStar* s = (CGStar*) pn->getCopyStar();
			    PortHole* tp = s->portWithName(realP.readName());
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
int CGFullConnect :: wormCodeGenerate(Galaxy&g) {
	int iprocs[nChildrenAlloc];
	for (int i = 0; i < nChildrenAlloc; i++)
		iprocs[i] = 0;

        BlockPortIter nextPort(g);
        PortHole* p;
        beginIteration(-1,0);

        // generate wormhole inputs
        while ((p = nextPort++) != 0) {
                if (p->isItInput()) {
			PortHole& realP = p->newConnection();
			ParNode* pn = (ParNode*)
				((SDFStar*) realP.parent())->myMaster();
			while (pn) {
				int ip = pn->getProcId();
				if (iprocs[ip] == 0) {
					iprocs[ip] = 1;
					CGStar* s = (CGStar*)pn->getCopyStar();
					s->myTarget()->wormInputCode(
					 *(s->portWithName(realP.readName())));
				}
				pn = (ParNode*) pn->getNextInvoc();
			}
		}
        }

	for (i = 0; i < nChildrenAlloc; i++)
		iprocs[i] = 0;

        nextPort.reset();
        // generate wormhole outputs
        while ((p = nextPort++) != 0) {
                if (p->isItOutput()) {
			PortHole& realP = p->newConnection();
			ParNode* pn = (ParNode*)
				((SDFStar*) realP.parent())->myMaster();
			while (pn) {
				int ip = pn->getProcId();
				if (iprocs[ip] == 0) {
					iprocs[ip] = 1;
					CGStar* s = (CGStar*)pn->getCopyStar();
					s->myTarget()->wormOutputCode(
					 *(s->portWithName(realP.readName())));
				}
				pn = (ParNode*) pn->getNextInvoc();
			}
		}
        }

        mySched()->compileRun();
        endIteration(-1,0);
        return wormLoadCode();
}

	///////////////////////////////
	// parallel scheduler support
	///////////////////////////////

// IPC stuff
int CGFullConnect::commTime(int, int, int nSamps, int type) {
        int cost = nSamps * int(sendTime);
        return type == 2 ? 2*cost : cost;
}

// return the array of candidate processors
IntArray* CGFullConnect :: candidateProcs(ParProcessors* parSched) {
	
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
	
ParNode* CGFullConnect :: backComm(ParNode* n) {
	if (n->getType() >= -1) return 0;
	ParAncestorIter iter(n);
	return (iter++);
}

void CGFullConnect :: resetResources() {}			

// create communication stars
SDFStar* CGFullConnect :: createSend(int from, int to, int num) {
	LOG_NEW; CGSend* newS = new CGSend;
	newS->registerProcs(from, to);
	return newS;
}

SDFStar* CGFullConnect :: createReceive(int from, int to, int num) {
	LOG_NEW; CGReceive* newS = new CGReceive;
	newS->registerProcs(from, to);
	return newS;
}

	/////////////////////////
	// CGDDF support
	/////////////////////////

void CGFullConnect :: setProfile(Profile* p) {
	((ParScheduler*) mySched())->setProfile(p);
}

int CGFullConnect :: computeProfile(int nP, int flag, IntArray*) {
	resetResources();
	ParScheduler* qs = (ParScheduler*) mySched();
	qs->setUpProcs(nP);
	int temp = qs->mainSchedule(*gal);
	if (flag < 0) qs->finalSchedule(*gal);
	return temp;
}

int CGFullConnect :: totalWorkLoad() {
        return ((ParScheduler*) mySched())->getTotalWork();
}

void CGFullConnect :: insideSchedule() {
 	((ParScheduler*) mySched())->sortProcessors();
}

void CGFullConnect::downLoadCode(int pId, Profile*) {
	((ParScheduler*)mySched())->getProc(pId)->run();
}

void CGFullConnect :: addCode(const char* code) {
	child(curChild)->addCode(code);
}

static CGFullConnect targ("FullyConnected","CGStar",
"Fully-connected targets for parallel scheduling");

static KnownTarget entry(targ,"FullyConnected");

