static const char file_id[] = "CGMultiTarget.cc";

/******************************************************************
Version identification:
@(#)CGMultiTarget.cc	1.75	12/08/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer: Soonhoi Ha, Jose Luis Pino

 Base target for multiprocessors (fully-connected).

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <unistd.h>             // unlink()
#include "Domain.h"
#include "Profile.h"
#include "ParNode.h"
#include "ParProcessors.h"
#include "CGMultiTarget.h"
#include "DLScheduler.h"
#include "HuScheduler.h"
#include "DeclustScheduler.h"
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
#include "HierScheduler.h"
#include "Cluster.h"

CGMultiTarget::CGMultiTarget(const char* name, const char* sClass,
			     const char* desc, const char* assocDomain) :
MultiTarget(name,sClass,desc,assocDomain), modifiedGalaxy(0), rm(0)
{
    addState(childType.setState("childType", this, "default-CG",
	"child proc types"));
    addState(resources.setState("resources", this, "",
	"define the specific resources of child targets (separated by ;)"));
    addState(relTimeScales.setState("relTimeScales", this, "1",
	"define the relative time scales of child targets"));
    filePrefix.setAttributes(A_SETTABLE);
    addState(schedName.setState
	     ("schedName(DL,HU,DC,HIER,CGDDF)",this,"DL",
	      "Multiprocessor Schedulers:\n"
	      "\tHU - T.C. Hu's level scheduler (ignores IPC)\n"
	      "\tDL - G.C. Sih's dynamic level scheduler\n"
	      "\tDC - G.C. Sih's clustering/declustering scheduler\n"
	      "\tHIER(DL,HU,DC) - J.L. Pino's hierarchical scheduler, "
	      "defaults to DL top-level parallel scheduler\n"
	      "\tCGDDF - S. Ha's dynamic constructs scheduler"));
    addState(ganttChart.setState("ganttChart",this,"YES",
	 "if true, display Gantt chart"));
    addState(logFile.setState("logFile", this, "",
	"log file to write to (none if empty)"));
    addState(amortizedComm.setState("amortizedComm",this,"NO",
	"try to amortize communication?"));
    addedStates.initialize();
}

CGMultiTarget::~CGMultiTarget() {
    deleteChildren();
    delSched();
    addedStates.deleteAll();
    LOG_DEL; delete rm;
}

Block* CGMultiTarget::makeNew() const {
    LOG_NEW; return new CGMultiTarget(name(), starType(), descriptor(),
				      getAssociatedDomain());
}

// setup
void CGMultiTarget::setup() {
    // Method works okay if gal is NULL
    Galaxy* gal = galaxy();

    // if the filePrefix is not set, set it to the galaxy name.
    if (gal && filePrefix.null()) filePrefix = gal->name();		

    // prepare child targets
    prepareChildren();
    if (SimControl :: haltRequested()) return;

    // We only want to modify the galaxy once.  See the comment in
    // CGMultiTarget.h for protected member modifiedGalaxy for details.
    if (!modifiedGalaxy) {
	if (!gal || haltRequested()) return;
	gal->setTarget(this);
	gal->initialize();
	modifiedGalaxy = 1;
	if (!modifyGalaxy()) return;
    }

    // check whether communication can be amortized or not.
    // If YES, setup the  Reachability matrix.
    if ((nChildrenAlloc > 1) && int(amortizedComm)) {
	LOG_NEW; 
	rm = new BooleanMatrix(nChildrenAlloc, nChildrenAlloc); 
    }

    // choose the right scheduler
    chooseScheduler();
    
    // in case of heterogeneous targets, flatten the wormholes.
    if (childType.size() > 1) {
	flattenWorm();
    }

    if (inWormHole()) wormPrepare();
    
    ParScheduler* sched = (ParScheduler*) scheduler();
    if (gal) sched->setGalaxy(*gal);
    sched->setUpProcs(nChildrenAlloc);

    // CG stuff
    myCode.initialize();

    if (gal && gal->parent()) {
	sched->ofWorm();
    }
    installDDF();
    Target :: setup();

    if (SimControl::haltRequested()) return;
    writeSchedule();
    if (inWormHole()) {
	adjustSampleRates();
	generateCode();
	writeCode();
	if (SimControl::haltRequested()) {
	    Error::abortRun(*this, "could not write code!");
	    return;
	}
	if (compileCode()) {
	    if (loadCode()) {
		if (!runCode())
		    Error::abortRun(*this, "could not run!");
	    }
	    else Error::abortRun(*this, "could not load!");
	}
	else Error::abortRun(*this, "could not compile!");
    }
}

int CGMultiTarget::recursiveModifyGalaxy(Galaxy& gal) {
    if (SimControl::haltRequested()) return FALSE;
    gal.initialize();
    
    StringList topStarType;
    topStarType << gal.domain() << "Star";
    int i;
    for (i = 0 ; i < nChildrenAlloc ; i++) {
	if (strcmp(child(i)->starType(), topStarType) == 0) {
	    if (cgChild(i)) {
		cgChild(i)->setGalaxy(gal);
		gal.setTarget(cgChild(i));
		if (!cgChild(i)->modifyGalaxy()) return FALSE;
		cgChild(i)->clearGalaxy();
		break;
	    }
	}
    }
    if (i == nChildrenAlloc) {
	Error::abortRun(*this, "Could not find child target to modify "
			"the galaxy named: ", gal.name());
    }
    
    GalStarIter nextStar(gal);
    CGStar* s;
    while ((s = (CGStar*)nextStar++) != 0) {
	if (s->isItWormhole()) {
	    Galaxy& innerGalaxy = s->asWormhole()->insideGalaxy();
	    if (!recursiveModifyGalaxy(innerGalaxy)) return FALSE;
	}
    }

    return TRUE;
}

int CGMultiTarget::modifyGalaxy() {
    if (!galaxy()) return FALSE;
    return recursiveModifyGalaxy(*galaxy());
}

// Return TRUE if the string c is equal to "HIER" (case-insensitive).
// Compared to the names of the other schedulers, the second letter is unique.
inline int hierSchedulerTest(const char* c) {
    // HPUX10 hppa.cfront can't handle inline functions that have statments
    // after 'return', so we combine the test into a single statement
    return ((*c == 'H') || (*c == 'h')) &&
	   ((*(c+1) == 'I') || (*(c+1) == 'i'));
}

void CGMultiTarget :: prepareChildren() {
    deleteChildren();
    nChildrenAlloc = nprocs;
    if (childType.size() > nChildrenAlloc) {
	Error :: warn(*this, "too many child types are",
		      " specified: look at [nprocs] parameter");
    }
    if (childType.size() != relTimeScales.size()) {
	Error :: warn(*this, "unmatched number of parameters: ",
		      "[childType] and [relTimeScales].\n",
		      "By default, we assume the same time scale, 1");
    }
    reorderChildren(0);	// first call initializes the structure
    StringList tname;
    int i;
    for (i = 0; i < nChildrenAlloc; i++) {
      Target* t = createChild(i);
      if (!t) return;
      addChild(*t);
      tname.initialize();
      tname << filePrefix << i;
      const char* childFilePrefix = hashstring(tname);
      t->setParent(this);
      if (cgChild(i)) {
	// Carefully test for the presence of each state
	// before attempting to set its value.
	State* s;
	if ((s = t->stateWithName("file")) != NULL) {
	  s->setInitValue(childFilePrefix);
	}
	else {
	  Error::warn(*t, "No state with given name: ", "file");
	}
	if ((s = t->stateWithName("directory")) != NULL) {
	  char* expandedPathName =
	    expandPathName(destDirectory.initValue());
	  s->setInitValue(hashstring(expandedPathName));
	  delete [] expandedPathName;
	}
	else {
	  Error::warn(*t, "No state with given name: ", "directory");
	}
	if ((s = t->stateWithName("display?")) != NULL) {
	  s->setInitValue(displayFlag.initValue());
	}
	else {
	  Error::warn(*t, "No state with given name: ", "display?");
	}
	if ((s = t->stateWithName("Looping Level"))
	    != NULL) {
	  s->setInitValue(hashstring("0"));
	}
	else {
	  Error::warn(*t, "No state with given name: ", "Looping Level");
	}
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
		Error :: abortRun(*this, item, " has unknown proc id");
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
    int hierSchedulingFlag = FALSE;
    size_t snamelen = strlen(sname);
    if (hierSchedulerTest(sname)) {
	hierSchedulingFlag = TRUE;
	const char* paren = strchr(sname,'(');
	if (paren && ! ++paren == '\0') sname = paren;
	// else, default to DL
    }

    // Set the scheduler
    StringList logPath = logFilePathName(destDirectory, logFile);
    if (snamelen > 1 && (*(sname+1) == 'U' || *(sname+1) == 'u')) {
    	LOG_NEW; mainScheduler = new HuScheduler(this, logPath);
    }
    else if (snamelen > 1 && (*(sname+1) == 'C' || *(sname+1) == 'c')) {
    	if (childType.size() > 1) {
	    Error :: warn(*this, "Declustering technique cannot be "
			  "applied for heterogeneous targets.\nBy default "
			  "we use dynamic-level scheduling.");
	    LOG_NEW; mainScheduler = new DLScheduler(this, logPath, 1);
    	}
	else if (resources.size() > 1) {
	    Error :: warn(*this, "Declustering technique cannot be "
			  "applied with resource restriction.\nBy default "
			  "we use dynamic-level scheduling.");
	    LOG_NEW; mainScheduler = new DLScheduler(this, logPath, 1);
    	}
	else {
	    LOG_NEW; mainScheduler = new DeclustScheduler(this, logPath);
    	}
    }
    else if (! hierSchedulingFlag && snamelen > 1 &&
	       (*(sname+1) == 'G' || *(sname+1) == 'g')) {
    	LOG_NEW; mainScheduler = new CGDDFScheduler(this, logPath);
    }
    else {
    	mainScheduler = new DLScheduler(this, logPath, 1);
    }

    if (hierSchedulingFlag)
	mainScheduler = new HierScheduler(this, logPath, *mainScheduler);

    setSched(mainScheduler);
}

void CGMultiTarget :: flattenWorm() {
    Galaxy* myGalaxy = galaxy();
    if (! myGalaxy) return;
    GalStarIter next(*myGalaxy);
    CGStar* s;
    while ((s = (CGStar*) next++) != 0) {
	if (s->isItWormhole()) {
	    CGWormBase* w = (CGWormBase*)s->asWormhole();
	    if (w == NULL) {
		Error::abortRun(*this, "flattenWorm: Wormhole does "
				"returns a NULL pointer.  Is the "
				"myWormhole() method defined for "
				"the current domain?");
		return;
	    }
	    // if inside domain is a CG domain, explode wormhole.
	    if (w->isCGinside()) {
		Galaxy* inG = w->explode();
		s->parent()->asGalaxy().addBlock(*inG, inG->name());
		delete s;
		next.remove();
	    }
	}
    }
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


// Display Gantt chart if requested.
void CGMultiTarget::writeSchedule() {
    if (nChildrenAlloc == 1) return;
    ParScheduler* s = (ParScheduler*) scheduler();
    if (int(ganttChart)) {
	char* gname = tempFileName();
	pt_ofstream o(gname);
	if (o) {
	    s->writeGantt(o);
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
// subGalaxies which cannot be expressed by regular porthole connection.
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
    if (! galaxy()) return FALSE;
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
    if (! galaxy()) return FALSE;
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

void CGMultiTarget :: writeCode() {
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
    if (! galaxy()) return;

    int* iprocs = new int[nChildrenAlloc];
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
		    s->cgTarget()->
			wormInputCode(*(s->portWithName(realP.name())));
		}
		pn = (ParNode*) pn->getNextInvoc();
	    }
	}
    }
    LOG_DEL; delete [] iprocs;
}

void CGMultiTarget :: allWormOutputCode() {
    if ( !galaxy()) return;

    int* iprocs = new int[nChildrenAlloc];
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
		    s->cgTarget()->
			wormOutputCode(*(s->portWithName(realP.name())));
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
    CGSend* newS = new CGSend;
    newS->registerProcs(from, to);
    return newS;
}

DataFlowStar* CGMultiTarget :: createReceive(int from, int to, int /*num*/) {
    CGReceive* newS = new CGReceive;
    newS->registerProcs(from, to);
    return newS;
}

// create Spread and Collect
DataFlowStar* CGMultiTarget :: createSpread() {
    return (new CGSpread);
}
DataFlowStar* CGMultiTarget :: createCollect() {
    return (new CGCollect);
}

// create Macro Star
CGStar* CGMultiTarget :: createMacro(CGStar*, int, int, int) {
    Error :: abortRun(*this, "code generation fails with macro actor",
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
    int i;
    for (i = 0; i < nChildrenAlloc; i++) 
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

static CGMultiTarget targ("FullyConnected", "CGStar",
			  "Fully-connected targets for parallel scheduling",
			  CGdomainName);

static KnownTarget entry(targ,"FullyConnected");

