static const char file_id[] = "CGTarget.cc";
/******************************************************************
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

 Programmer: J. Buck, J. Pino, T. M. Parks

 Baseclass for all single-processor code generation targets.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Cluster.h"
#include "CGTarget.h"
#include "CGStar.h"
#include "CGPortHole.h"
#include "Domain.h"
#include "GalIter.h"
#include "Error.h"
#include "SDFScheduler.h"
#include "EventHorizon.h"
#include "LoopScheduler.h"
#include "SDFCluster.h"
#include "miscFuncs.h"
#include "CGUtilities.h"
#include "pt_fstream.h"
#include "SimControl.h"
#include "KnownBlock.h"
#include "Tokenizer.h"
#include <time.h>

extern const char* CODE = "code";
extern const char* PROCEDURE = "procedure";

// Return a string for indenting to the given depth
StringList CGTarget::indent(int depth) {
	StringList out;
	out = "";
	for(int i=0; i<depth; i++) {
		out += "    ";
	}
	return out;
}

// constructor
CGTarget::CGTarget(const char* name,const char* starclass,
		   const char* desc, char sep)
: Target(name,starclass,desc), defaultStream(&myCode), schedFileName(0), noSchedule(0)
  
{
	separator = sep;
	targetNestedSymbol.setSeparator(separator);
	targetNestedSymbol.setCounter(symbolCounter());
	sharedSymbol.setSeparator(separator);
	sharedSymbol.setCounter(symbolCounter());

	starTypes += "HOFStar";
	starTypes += "AnyCGStar";

	addState(targetHost.setState("host", this, "",
	    "Host machine to compile or assemble code on."));
	addState(destDirectory.setState("directory", this, "$HOME/PTOLEMY_SYSTEMS",
	    "Directory to write to"));
	addState(filePrefix.setState("file", this, "",
	    "Prefix for file names."));
	addState(loopingLevel.setState("Looping Level",this,"0",
		"Specify whether to use loop scheduler and in what level."));
	addState(displayFlag.setState("display?", this, "YES",
	    "Enable displaying of code."));
	addState(compileFlag.setState("compile?", this, "YES",
	    "Enable compilation of code."));
	addState(loadFlag.setState("load?", this, "YES",
	    "Enable loading of code."));
	addState(runFlag.setState("run?", this, "YES",
	    "Enable running of code."));

	/* Hide parameters by making them nonsettable.
	   Derived Targets can re-enable those that are appropriate.
	*/
	targetHost.clearAttributes(A_SETTABLE);
	filePrefix.clearAttributes(A_SETTABLE);
	displayFlag.clearAttributes(A_SETTABLE);
	compileFlag.clearAttributes(A_SETTABLE);
	loadFlag.clearAttributes(A_SETTABLE);
	runFlag.clearAttributes(A_SETTABLE);

	procedures.initialize();

	addStream(CODE, &myCode);
	addStream(PROCEDURE, &procedures);
	counter = 0;
}

// destructor
CGTarget::~CGTarget() {
	delSched();
	LOG_DEL; delete [] schedFileName;
}


// Methods used in setup(), run(), wrapup(), and generateCode().
// The default versions do nothing.
// Return FALSE on error.
int CGTarget :: modifyGalaxy() { return TRUE; }
int CGTarget :: codeGenInit() { return TRUE; }
int CGTarget :: allocateMemory() { return TRUE; }
int CGTarget :: compileCode() { return TRUE; }
int CGTarget :: loadCode() { return TRUE; }
int CGTarget :: runCode() { return TRUE; }

// the main guy.

void CGTarget::setup() {
	myCode.initialize();
	makefile.initialize();
	makeAllList.initialize();

	// This only initializes the streams owned by 'codeStringLists'
	codeStringLists.initialize();
	if (!scheduler()) {
	    switch (int(loopingLevel)) {
	    case 0:
		LOG_NEW; setSched(new SDFScheduler);
		break;
	    case 1:
		delete [] schedFileName;
		schedFileName = writeFileName("schedule.log");
		LOG_NEW; setSched(new SDFClustSched(schedFileName));
		break;
	    case 2:
		delete [] schedFileName;
		schedFileName = writeFileName("schedule.log");
		LOG_NEW; setSched(new LoopScheduler(schedFileName));
		break;
	    default:
		Error::abortRun(*this,"Unknown scheduler");
		break;
	    }
	}
	if (!galaxySetup()) return;
	if (haltRequested()) return;
	if (filePrefix.null()) filePrefix = galaxy()->name();
	
	// This will be phased out.  Use either CGTarget::writeFile
	// or CGUtilities.h rpcWriteFile.
	writeDirectoryName(destDirectory);

	// Reset the symbol lists.
	counter = 0;
	targetNestedSymbol.setCounter(symbolCounter());
	targetNestedSymbol.initialize();
	sharedSymbol.setCounter(symbolCounter());
	sharedSymbol.initialize();

	// be sure that this routine does not initialize the galaxy if it
	// is a child target. It is already initialized, and done more!
	if (!parent())
	    if (!modifyGalaxy()) return;

	if (!noSchedule) {
		if(!schedulerSetup()) return;
	}
	if (haltRequested()) return;
	noSchedule = 0;		// reset for next setup.

	// If in a WormHole, generate, compile, load, and run code.
	// Ignore flags which may otherwise disable these functions.
	if (inWormHole() && alone())
	{
	    adjustSampleRates();
	    wormPrepare();
	    generateCode();
	    if (haltRequested()) return;
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

void CGTarget::childInit() {
	// initialize state
	initState();
	// scheduler setup
	if (!scheduler()) {
		LOG_NEW; setSched(new SDFScheduler);
	}
}

void CGTarget::wormPrepare() { }

int CGTarget :: run() {
    // avoid core dumps from incorrect operation
    if (!scheduler() || !galaxy()) {
	Error::abortRun(*this, "Target has not been setup correctly");
	return FALSE;
    }
    // if a wormhole, we must do the transfer of data to and from the target.
    if(inWormHole()) {
	if(!allSendWormData()) return FALSE;
	if(!allReceiveWormData()) return FALSE;
    }
    else
	generateCode();
	
    return !haltRequested();
}

// If not in a WormHole, conditionally compile, load, and run code.
void CGTarget :: wrapup()
{
    if (haltRequested()) return;
    if (!inWormHole())
    {
	if (compileFlag)
	{
	    if (!compileCode())
		Error::abortRun(*this, "could not compile code!");
	    else if (loadFlag)
	    {
		if (!loadCode())
		    Error::abortRun(*this, "could not load code!");
		else if (runFlag)
		{
		    if (!runCode())
			Error::abortRun(*this, "could not run code!");
		}
	    }
	}
    }
}

void CGTarget::generateCode() {
	if (haltRequested()) return;
	if (parent()) setup();
	if (haltRequested()) return;
	if(!allocateMemory()) 
	{
		Error::abortRun(*this,"Memory allocation error");
		return;
	}
	if (haltRequested()) return;
	generateCodeStreams();
	if (haltRequested()) return;
	frameCode();
	if (haltRequested()) return;
	if (!parent()) writeCode();

	// now initialize for the next run
	procedures.initialize();
}

int* CGTarget::symbolCounter() {
	if (parent()) {
		if(((CGTarget*)parent())->isA("CGTarget")) {
			return ((CGTarget*)parent())->symbolCounter();
		}
	}
	return &counter;
}

void CGTarget::generateCodeStreams() {
	if (SimControl::haltRequested()) return;
	headerCode();
	if (!codeGenInit())
	{
		Error::abortRun(*this,"codeGenInit error");
		return;
	}
	mainLoopCode();
	Target :: wrapup();
	trailerCode();
}

void CGTarget :: mainLoopCode() {
	// Generate Code.  Iterate an infinite number of times if 
	// target is inside a wormhole
	int iterations = inWormHole()? -1 : (int)scheduler()->getStopTime();
	beginIteration(iterations,0);
	if (inWormHole()) allWormInputCode();
	compileRun((SDFScheduler*) scheduler());
	if (inWormHole()) allWormOutputCode(); 
	endIteration(iterations,0);
}

int CGTarget :: insertGalaxyCode(Galaxy* g, SDFScheduler* s) {
	// save infos
	Galaxy* saveGal = galaxy();
	setGalaxy(*g);
	Target& saveT = s->target();
	s->setTarget(*this);

        if(!allocateMemory() || !codeGenInit()) return FALSE;
        compileRun(s);
        Target :: wrapup();

	// restore info
	setGalaxy(*saveGal);
	s->setTarget(saveT);
	return TRUE;
}	

// virtual function
void CGTarget :: compileRun(SDFScheduler* s) {
	s->compileRun();
}

void CGTarget :: addStream(const char* name, CodeStream* code)
{
	int flag = codeStringLists.append(code,name);
	if (flag == FALSE)
	{
		StringList message;
		message << "addStream: " << name << " already exists";
		Error::abortRun(*this,message);
	}
}

// Lookup a CodeStream by name.
CodeStream* CGTarget :: getStream(const char* name)
{
    CodeStream* stream;

    if (name == NULL) stream = defaultStream;
    else stream  = codeStringLists.get(name);

    if (stream == NULL)
    {
	Error::abortRun(*this, "getStream: could not find ", name);
    }

    return stream;
}


void CGTarget :: frameCode() {}

void CGTarget :: copySchedule(SDFSchedule& s) {
	if (!scheduler()) {
		LOG_NEW; setSched(new SDFScheduler);
	}

	SDFScheduler* sched = (SDFScheduler*) scheduler();
	sched->copySchedule(s);
	// indicate multiprocessor scheduler already generated schedule
	noSchedule = TRUE;
}

Block* CGTarget :: makeNew() const {
	LOG_NEW; return new CGTarget(name(),starType(),descriptor());
}

void CGTarget :: headerCode () {
	myCode << headerComment();
}

void CGTarget :: trailerCode() {}

void CGTarget :: beginIteration(int reps, int depth) {
    myCode << "REPEAT " << reps << " TIMES { /* depth " << depth << "*/\n";
}

void CGTarget :: endIteration(int /*reps*/, int depth) {
	myCode << "} /* end repeat, depth " << depth << "*/\n";
}


int CGTarget :: incrementalAdd(CGStar*, int) {
	Error:: abortRun("No Incremental-Add(so, no macro) supported yet");
	return FALSE;
}


int CGTarget::inWormHole() {
	return int(galaxy()->parent());
}

// If a CG domain is inside a wormhole, we may need to change
// the sample rate of event horizons after scheduling is performed.
void CGTarget :: adjustSampleRates() {
   BlockPortIter nextPort(*galaxy());
   PortHole* p;
   while ((p = nextPort++) != 0) {
		DFPortHole* dp = (DFPortHole*) &p->newConnection();
		int num = dp->numXfer() * dp->parentReps();
		dp->far()->asEH()->setParams(num);
	}
}

void CGTarget :: allWormInputCode() {
	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItInput()) wormInputCode(p->newConnection());
	}
}
void CGTarget :: wormInputCode(PortHole& p) {
	myCode << "/* READ from wormhole port " << p.fullName() << " */\n";
}

void CGTarget :: allWormOutputCode() {
	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItOutput()) wormOutputCode(p->newConnection());
	}
}
void CGTarget :: wormOutputCode(PortHole& p) {
	myCode << "/* WRITE to wormhole port " << p.fullName() << " */\n";
}

int CGTarget :: allSendWormData() {
    BlockPortIter nextPort(*galaxy());
    PortHole* p;
    while ((p = nextPort++) != 0) {
	if (p->isItInput() && !sendWormData(p->newConnection())) 
	    return FALSE;
    }
    return TRUE;
}

int CGTarget :: sendWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	// data are discarded
	cp.forceGrabData();
	return TRUE;
}

int CGTarget :: allReceiveWormData() {
    BlockPortIter nextPort(*galaxy());
    PortHole* p;
    while ((p = nextPort++) != 0) {
	if (p->isItOutput() && !receiveWormData(p->newConnection())) 
	    return FALSE;
    }
    return TRUE;
}

int CGTarget :: receiveWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	int size = cp.bufSize();
	// insert zero-valued particles onto cp's geodesic
	for (int i = 0; i < size; i++)
		cp%i << 0;
	cp.forceSendData();
	return TRUE;
}

void CGTarget :: writeFiring(Star& s,int) { // depth parameter ignored
	s.run();
}

// generate loop initialization/end code.  Star is assumed to be a
// DataFlowStar.
void CGTarget :: genLoopInit(Star& s, int reps) {
	DataFlowStar* ds = (DataFlowStar*)&s;
	ds->beginLoop(reps);
}

void CGTarget :: genLoopEnd(Star& s) {
	DataFlowStar* ds = (DataFlowStar*)&s;
	ds->endLoop();
}

void CGTarget :: writeCode() {
    writeFile(myCode,"",displayFlag);
}

int CGTarget::writeFile(const char* text,const char* suffix,
			int display,int mode){
    StringList fileName;
    fileName << filePrefix << suffix;
    int status = rcpWriteFile( targetHost, destDirectory, fileName, text, 
		 display, mode);
    if (status) {
	return TRUE;
    }
    else {
	StringList errMsg;
	errMsg << "Write of file " << fileName << " unsuccessful.";
	Error::abortRun(*this,errMsg);
	return FALSE;
    }
}

ISA_FUNC(CGTarget,Target);

StringList CGTarget::comment(const char* text, const char* b, 
    const char* e, const char* cont)
{
    StringList cmnt;
    const char* begin = "# ";
    const char* end = "";
    const char* special = "";
    const char* white = "\n";
    Tokenizer lexer(text, special, white);
    char line[1024];

    if (text==NULL) return cmnt;
    if (b!=NULL) begin = b;
    if (e!=NULL) end = e;

    if (cont==NULL)	// generate multiple single-line comments
    {
	lexer >> line;
	while (*line != '\0')
	{
	    cmnt << begin << line << end << '\n';
	    lexer >> line;
	}
    }
    else if (*cont == '\0')	// no special continuation string required
    {
	cmnt << begin << text << end << '\n';
    }
    else	// special continuation required for multi-line comments
    {
	lexer >> line;
	cmnt << begin << line;
	lexer >> line;
	while (*line != '\0')
	{
	    cmnt << '\n' << cont << line;
	    lexer >> line;
	}
	cmnt << end << '\n';
    }
    return cmnt;
}

StringList CGTarget::headerComment(const char* begin, const char* end, 
    const char* cont) {
	StringList header,msg;
	time_t t = time(NULL);
	msg 	<< "User:      " << cuserid(NULL) << "\n"
		<< "Date:      " << ctime(&t)
		<< "Target:    " << fullName() << "\n"
		<< "Universe:  " << galaxy()->name() << "\n";
	if (begin==NULL) header << comment(msg);
	else header << comment(msg,begin,end,cont);
	return header;
}
	
int CGTarget::systemCall(const char*command,const char*error,const char*host){
	int i;
	if( (i=rshSystem(host,command,workingDirectory())) != 0 && 
          error != NULL)
		Error::abortRun(error);
	return i;
}

const char* CGTarget::lookupSharedSymbol(const char* scope, const char* name)
{
    return sharedSymbol.lookup(scope, name);
}

// splice star "name" to the porthole p.
Block* CGTarget::spliceStar(PortHole* p, const char* name,
				int delayBefore, const char* dom)
{
	PortHole* pfar = p->far();
	int ndelay = p->numInitDelays();
	p->disconnect();
	if (p->isItOutput()) {
		PortHole* t = p;
		p = pfar;
		pfar = t;
	}
	
	// p is now an input port.
	Block* newb = KnownBlock::clone(name,dom);
	if (newb == 0) {
		Error::abortRun("failed to clone a ", name, "!");
		return 0;
	}

	PortHole* ip = newb->portWithName("input");
	PortHole* op = newb->portWithName("output");
	if (ip == 0 || op == 0) {
		Error::abortRun(name, " has the wrong interface for splicing");
		LOG_DEL; delete newb;
		return 0;
	}

	// initialize the new star.  Name it and add it to the galaxy.
	// using size of splice list in name forces unique names.
	StringList newname("splice-");
	newname << newb->className() << "-" << spliceList.size();

	// try and resolve which galaxy we should add the splice
	// block to.  Ideally we want the block added to one of the
	// galaxies of the stars that we are connecting this spliced
	// star to.
	Galaxy* addTo;

	if (p->atBoundary() && p->isItOutput())
	    addTo = (Galaxy*) pfar->parent()->parent();
	else
	    addTo = (Galaxy*) p->parent()->parent();
	
	addTo->addBlock(*newb,hashstring(newname));
	newb->setParent(addTo);

 	// connect up the new star.  This must be done after adding the
	// block to the galaxy so that the geodesics are allocated
	// properly.  It must be also be done before initializing so that
	// the spliced in block can resolve the data type on its ports.
	pfar->connect(*ip,delayBefore ? ndelay : 0);
	op->connect(*p,delayBefore ? 0 : ndelay);

	newb->initialize();

	// If stars have cluster parents, must add to a cluster
	DataFlowStar* dfnewb = (DataFlowStar*)newb;
	DataFlowStar* pfarStar = (DataFlowStar*)pfar->parent();
	DataFlowStar* pStar = (DataFlowStar*)p->parent();
	if (pfarStar->parentCluster() && pfar->numXfer() == ip->numXfer()) {
	    pfarStar->parentCluster()->addSplicedStar(*dfnewb);
	}
	else if (pStar->parentCluster() && p->numXfer() == op->numXfer()) {
	    pStar->parentCluster()->addSplicedStar(*dfnewb);
	}
	else if (pStar->parentCluster() || pfarStar->parentCluster()) {
	    Error::abortRun(*this,"Could not place into a cluster:",
			    newb->name());
	}
	
	// save in the list of spliced stars
	spliceList.put(newb);

	// check errors in initialization
	if (Scheduler::haltRequested()) return 0;

	return newb;
}

