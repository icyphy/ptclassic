static const char file_id[] = "CGMultiTarget.cc";

/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

#include "Domain.h"
#include "Profile.h"
#include "ParNode.h"
#include "ParProcessors.h"
#include "CGMultiTarget.h"
#include "DLScheduler.h"
#include "HuScheduler.h"
#include "DeclustScheduler.h"
#include "MacroScheduler.h"
#include "CGDDFScheduler.h"
#include "KnownTarget.h"
#include "CGSpread.h"
#include "CGCollect.h"
#include "CGSend.h"
#include "CGReceive.h"
#include "CGWormBase.h"
#include "Geodesic.h"
#include "pt_fstream.h"
#include "stdlib.h"
#include "BooleanMatrix.h"
#include "MultiScheduler.h"
#include "ProfileTimer.h"

CGMultiTarget::CGMultiTarget(const char* name,const char* sClass,
			     const char* desc) :
	MultiTarget(name,sClass,desc), modifiedGalaxy(0), rm(0)
{
	addState(childType.setState("childType",this,"default-CG",
				    "child proc types"));
	addState(resources.setState("resources",this,"",
	"define the specific resources of child targets (separated by ;)"));
	addState(relTimeScales.setState("relTimeScales",this,"1",
		"define the relative time scales of child targets"));
	filePrefix.setAttributes(A_SETTABLE);
        addState(schedName.setState("schedName(DL,HU,DC,MACRO,CGDDF)",this,"DL",
           "schedulers: DL - dynamic level \n\t HU - ignore IPC \n\t DC - clustering \n\t MACRO - parallel task\n\t CGDDF - dynamic constructs \n"));
        addState(ganttChart.setState("ganttChart",this,"YES",
                                     "if true, display Gantt chart"));
        addState(logFile.setState("logFile",this,"",
                                     "log file to write to (none if empty)"));
        addState(amortizedComm.setState("amortizedComm",this,"NO",
                                     "try to amortize communication?"));
	addState(useMultipleSchedulers.setState("Use multiple schedulers?",
						this,"NO",
				   "Use hierarchical schedulers?"));
	addState(displaySchedulerStats.setState("Display scheduler stats?",
		 this,"NO","Print DAG nodes and scheduling time to stdout?"));
	addedStates.initialize();
}

CGMultiTarget::~CGMultiTarget() {
	deleteChildren();
	delSched();
	addedStates.deleteAll();
	LOG_DEL; delete rm;
}

Block* CGMultiTarget::makeNew() const {
	LOG_NEW; return new CGMultiTarget(name(),starType(),descriptor());
}

	/////////////////////////
	// setup
	/////////////////////////

void CGMultiTarget::setup() {
	// if the filePrefix is not set, set it to the galaxy name.
	if (filePrefix.null()) filePrefix = galaxy()->name();		

	// prepare child targets
	prepareChildren();
	if (SimControl :: haltRequested()) return;

	// This will be phased out.  Use either CGTarget::writeFile
	// or CGUtilities.h rpcWriteFile.
	writeDirectoryName(destDirectory);

	// We only want to modify the galaxy once.  See the comment in
	// CGMultiTarget.h for protected member modifiedGalaxy for details.
	if (!modifiedGalaxy) {
	    modifiedGalaxy = 1;
	    if (!modifyGalaxy()) return;
	}

	// check whether communication can be amortized or not.
	// If YES, setup the  Reachability matrix.
	if ((nChildrenAlloc > 1) && int(amortizedComm)) {
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

	// CG stuff
	myCode.initialize();

	if (galaxy()->parent()) {
		sched->ofWorm();
	}
	installDDF();
	Target :: setup();

	writeSchedule();
	if (inWormHole()) {
	   	adjustSampleRates();
		generateCode();
		if (compileCode())
		{
			if (loadCode())
			{
				if (!runCode())
				    Error::abortRun(*this, "could not run!");
			}
			else Error::abortRun(*this, "could not load!");
		}
		else Error::abortRun(*this, "could not compile!");
	}
}

void CGMultiTarget :: prepareChildren() {
	deleteChildren();
	nChildrenAlloc = nprocs;
	if (childType.size() > nChildrenAlloc) {
	    Error :: warn("too many child types are",
	      " specified: look at [nprocs] parameter");
	}
	if (childType.size() != relTimeScales.size()) {
	    Error :: warn("unmatched number of parameters: ",
	    "[childType] and [relTimeScales].\n",
	    "By default, we assume the same time scale, 1");
	}
	reorderChildren(0);	// first call initializes the structure
	StringList tname;
	for (int i = 0; i < nChildrenAlloc; i++) {
	    Target* t = createChild(i);
	    if (!t) return;
	    addChild(*t);
	    tname.initialize();
	    tname << filePrefix << i;
	    const char* childFilePrefix = hashstring(tname);
	    t->setNameParent(childFilePrefix,this);
	    if (cgChild(i)) {
		t->stateWithName("file")->setInitValue(childFilePrefix);
		t->stateWithName("directory")
		 ->setInitValue(destDirectory.initValue());
		t->stateWithName("display?")
                 ->setInitValue(displayFlag.initValue());
		t->stateWithName("Looping Level")
		 ->setInitValue(hashstring("0"));
	    }
	}
	resourceInfo();
	for (i = 0; i < nChildrenAlloc; i++) {
		((CGTarget*) child(i))->childInit();
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
    ParScheduler* mainScheduler;
    const char* sname = schedName;
    if (*sname == 'M' || *sname == 'm') {
    	LOG_NEW; mainScheduler = new MacroScheduler(this, logFile);
    } else if (*sname == 'H' || *sname == 'h') {
    	LOG_NEW; mainScheduler = new HuScheduler(this, logFile);
    } else if (*(sname+1) == 'C' || *(sname+1) == 'c') {
    	if (childType.size() > 1) {
    		Error :: warn("Declustering technique can not be",
    		"applied for heterogeneous targets.\n By default",
    		"we use dynamic-level scheduling");
    		LOG_NEW; mainScheduler = new DLScheduler(this, logFile, 1);
    	} else if (resources.size() > 1) {
    		Error :: warn("Declustering technique can not be",
    		"applied with resource restriction.\n By default",
    		"we use dynamic-level scheduling");
    		LOG_NEW; mainScheduler = new DLScheduler(this, logFile, 1);
    	} else {
    		LOG_NEW; mainScheduler = new DeclustScheduler(this, logFile);
    	}
    } else if (*(sname+1) == 'G' || *(sname+1) == 'g') {
    	LOG_NEW; mainScheduler = new CGDDFScheduler(this, logFile);
    } else {
    	LOG_NEW; mainScheduler = new DLScheduler(this, logFile, 1);
    }

    if(int(useMultipleSchedulers)) {
	LOG_NEW; mainScheduler=new MultiScheduler(this,logFile,*mainScheduler);
    }
    setSched(mainScheduler);
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
			CGWormBase* w = (CGWormBase*)s->asWormhole();
			if (w == NULL) {
				Error::abortRun(*this,"flattenWorm: Wormhole does returns a NULL pointer.  Is the myWormhole() method defined for the current domain?");
				return;
			}
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
	for (int i = 0; i < nChildrenAlloc; i++) {
		child(i)->setStopTime(f);
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
		LOG_DEL; delete [] gname;
        }
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
	if (!allSendWormData()) return FALSE;
	return allReceiveWormData();
}

int CGMultiTarget :: allSendWormData() {
/*	if (nChildrenAlloc == 1) {
		CGTarget* t = (CGTarget*) child(0);
		return t->allSendWormData();
	} */

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

int CGMultiTarget :: allReceiveWormData() {
/*	if (nChildrenAlloc == 1) {
		CGTarget* t = (CGTarget*) child(0);
		return t->allReceiveWormData();
	} */

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

/*virtual*/ int CGMultiTarget::schedulerSetup() {
    ProfileTimer schedulingTimer;
    int status = MultiTarget::schedulerSetup();

    if (int(displaySchedulerStats)) {
	double schedulingTime =  schedulingTimer.elapsedTime();

	StringList message;

	// Compute total stars
	GalStarIter nextStar(*galaxy());
	DataFlowStar* star;
	int totalStars = 0;
	while ((star = (DataFlowStar*) nextStar++) != NULL)
	    totalStars++;
	    
	int dagNodes = ((ParScheduler*)scheduler())->dagNodes();

	message << "The time to compute the schedule was "
		<< schedulingTime 
		<< " seconds.  There were a total of "
		<< dagNodes << " precedence DAG nodes "
		<< "constructed from a multirate dataflow graph of " 
		<< totalStars << " nodes.";
	Error::message(message);
	cout << message << "\n";
	cout.flush();
    }
    return status;
}    

// default code generation for a wormhole.  This produces an infinite
// loop that reads from the inputs, processes the schedule, and generates
// the outputs.
// Note that wormOutputCode is called before compileRun(). In compileRun(),
// all child targets will finish generating code.

void CGMultiTarget :: generateCode() {
    if (parent()) setup();		// check later whether this is right.
    int iterations = inWormHole()? -1 : (int)scheduler()->getStopTime();
    beginIteration(iterations,0);
    scheduler()->compileRun();
	if (SimControl::haltRequested()) return;
	for (int i = 0 ; i < nChildrenAlloc ; i++) {
		if (child(i)->isA("CGTarget")) {
			((CGTarget*)child(i))->generateCode();
		}
	}
        endIteration(iterations,0);
}

void CGMultiTarget::wrapup() {
	if (!inWormHole()&&!parent()) writeCode();
	MultiTarget::wrapup();
}

/*virtual*/ void CGMultiTarget :: writeCode() {
        for (int i = 0 ; i < nChildrenAlloc ; i++) {
                if (child(i)->isA("CGTarget")) {
                        ((CGTarget*)child(i))->writeCode();
                }
        }
}

/*virtual*/ int CGMultiTarget :: compileCode() {
        int status = TRUE;
        for (int i = 0 ; i < nChildrenAlloc ; i++) {
                if (child(i)->isA("CGTarget")) {
                        status &= ((CGTarget*)child(i))->compileCode();
                }
        }
	return status;
}

/*virtual*/ int CGMultiTarget :: runCode() {
        int status = TRUE;
        for (int i = 0 ; i < nChildrenAlloc ; i++) {
                if (child(i)->isA("CGTarget")) {
                        status &= ((CGTarget*)child(i))->runCode();
                }
        }
	return status;
}

/*virtual*/ int CGMultiTarget :: loadCode() {   
        int status = TRUE;
        for (int i = 0 ; i < nChildrenAlloc ; i++) {
                if (child(i)->isA("CGTarget")) {
                        status &= ((CGTarget*)child(i))->loadCode();
                }
        }
        return status;
}

int CGMultiTarget :: childSupport(Target* t, Star* s) {
    return t->support(s);
}

// Trickiness again for wormholes. 
// Since more than one ParNodes are correspond to one star in a subGal,
// we are careful in searching the stars at the wormhole boundary!

void CGMultiTarget :: allWormInputCode() {
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
					s->cgTarget()->wormInputCode(
					 *(s->portWithName(realP.name())));
				}
				pn = (ParNode*) pn->getNextInvoc();
			}
		}
        }
	LOG_DEL; delete [] iprocs;
}

void CGMultiTarget :: allWormOutputCode() {
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
					s->cgTarget()->wormOutputCode(
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

// is a heterogeneous target?
int CGMultiTarget :: isHetero() {
	return (childType.size() > 1) || (relTimeScales.size() > 1);
}

ParNode* CGMultiTarget :: backComm(ParNode* n) {
	if (n->getType() >= -1) return 0;
	ParAncestorIter iter(n);
	return (iter++);
}

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
	
// create Macro Star
CGStar* CGMultiTarget :: createMacro(CGStar*, int, int, int) {
	Error :: abortRun("code generation fails with macro actor",
	"\n create domain-specific Macro star");
	return 0;
}

// return the execution of a star if scheduled on the given target.
// If the target does not support the star, return -1.
int CGMultiTarget :: execTime(DataFlowStar* s, CGTarget* t) {
	if ((!t) || (relTimeScales.size() <= 1)) return s->myExecTime();
	else if (!childSupport(t,s)) return -1;

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

// return TRUE if amortization is possible, FALSE otherwise.
int CGMultiTarget :: amortize(int from, int to) {
	if (!rm) return FALSE;
	int x = rm->getElem(from, to) + rm->getElem(to, from);
	if (x < 2) return TRUE;
	else return FALSE;
}

ISA_FUNC(CGMultiTarget, MultiTarget);

// check for presence of child targets of appropriate type.
int CGMultiTarget :: childIsA(const char* type) const {

	// problem: when this is called on the obj on the known
	// target list,
	// its state will not be initialized, so we have to use
	// a different StringArrayState that we can initialize.

	StringArrayState copy;
	copy.setInitValue(childType.initValue());
	copy.initialize();

	int nt = copy.size();
	for (int i = 0; i < nt; i++) {
		const Target* t = KnownTarget::find(copy[i]);
		if (t && t->isA(type)) return TRUE;
	}
	return Target::childIsA(type);
}

static CGMultiTarget targ("FullyConnected","CGStar",
"Fully-connected targets for parallel scheduling");

static KnownTarget entry(targ,"FullyConnected");

