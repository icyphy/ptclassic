static const char file_id[] = "CGTarget.cc";
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

 Programmer: J. Buck, J. Pino, T. M. Parks

 Baseclass for all single-processor code generation targets.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGTarget.h"
#include "CGStar.h"
#include "GalIter.h"
#include "Error.h"
#include "SDFScheduler.h"
#include "EventHorizon.h"
#include "LoopScheduler.h"
#include "SDFCluster.h"
#include "CGDisplay.h"
#include "miscFuncs.h"
#include "CGUtilities.h"
#include "pt_fstream.h"
#include "SimControl.h"
#include "KnownBlock.h"
#include <stream.h>
#include <time.h>
#include <string.h>

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
: Target(name,starclass,desc), schedFileName(0), noSchedule(0), inheritFlag(0)
{
	separator = sep;
	targetNestedSymbol.setSeparator(separator);
	targetNestedSymbol.setCounter(&symbolCounter);
	sharedSymbol.setSeparator(separator);
	sharedSymbol.setCounter(&symbolCounter);

	addState(destDirectory.setState("Destination Directory",this,
		"PTOLEMY_SYSTEMS","Directory to write to"));
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

	addStream(CODE, &myCode);
	addStream(PROCEDURE, &procedures);
	symbolCounter = 0;
}

// destructor
CGTarget::~CGTarget() {
	delSched();
	LOG_DEL; delete schedFileName;

	ListIter next(spliceList);
	Block* b;
	while ((b = (Block*)next++) != 0) {
		// prevent some galaxy types from deleting b twice.
		galaxy()->removeBlock(*b);
		LOG_DEL; delete b;
	}
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
// If there is no galaxy yet, we just create a scheduler and return.
// This is not to my liking; the whole way we do this needs to be
// rethought.  It also means that any derived classes of CGTarget that
// are intended to be used as child targets of CGMultiTarget must
// behave the same way.

void CGTarget::setup() {
	myCode.initialize();
	procedures.initialize();
	writeDirectoryName(destDirectory);
	if (!scheduler()) {
		int lv = int(loopingLevel);
		if(lv > 0) {
			schedFileName = writeFileName("schedule.log");
			if (lv == 1) {
			   LOG_NEW; setSched(new SDFClustSched(schedFileName));
			} else {
			   LOG_NEW; setSched(new LoopScheduler(schedFileName));
			}
		} else {
			LOG_NEW; setSched(new SDFScheduler);
		}
	}
	if (!galaxy()) return;

	// Reset the symbol lists.
	symbolCounter = 0;
	targetNestedSymbol.initialize();
	sharedSymbol.initialize();

	// be sure that this routine does not initialize the galaxy if it
	// is a child target. It is already initialized, and done more!
	if (!modifyGalaxy()) return;

	if (!noSchedule) {
		Target::setup();
		if (haltRequested()) return;
	}
	noSchedule = 0;		// reset for next setup.

	// If in a WormHole, generate, compile, load, and run code.
	// Ignore flags which may otherwise disable these functions.
	if (inWormHole() && alone())
	{
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

// If not in a WormHole, conditionally display, compile, load, and run code.
void CGTarget :: wrapup()
{
    if (!inWormHole())
    {
	if (displayFlag) display(myCode);
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
	CGTarget* parT = (CGTarget*) parent();
	if (parT) { 
		setup();
		symbolCounter = parT->symbolCounter;
	}
	if (SimControl::haltRequested()) return;
	headerCode();
	if(!allocateMemory()) 
	{
		Error::abortRun(*this,"Memory allocation error");
		return;
	}
	if (!codeGenInit())
	{
		Error::abortRun(*this,"codeGenInit error");
		return;
	}
	mainLoopCode();
	Target :: wrapup();
	trailerCode();
	frameCode();
	if (!parent()) writeCode();
	if (parT) parT->symbolCounter = symbolCounter;
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

void CGTarget :: addStream(const char* name, CodeStream* slist)
{
	int flag = codeStringLists.add(name,slist);
	if (flag == FALSE)
	{
		StringList message;
		message << "addStream: " << name << " already exists";
		Error::abortRun(*this,message);
	}
}

CodeStream* CGTarget :: getStream(const char* name)
{
	return codeStringLists.get(name);
}

int CGTarget :: run() {
    // if a wormhole, we must do the transfer of data to and from the target.
    if(inWormHole()) {
	if(!allSendWormData()) return FALSE;
	if(!allReceiveWormData()) return FALSE;
    }
    else 	
	generateCode();
	
    return !haltRequested();
}

void CGTarget :: frameCode() {}

void CGTarget :: copySchedule(SDFSchedule& s) {
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
	Error:: abortRun("No Incremental-Add(so, no wormhole) supported yet");
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
	cout << "sending " << cp.bufSize() << " values to worm port " 
	     << cp.fullName() << "\n";
	cout.flush();
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
	cout << "receiving " << size <<
		" values from worm port " << p.fullName() << "\n";
	cout.flush();
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

void CGTarget :: writeCode(const char* name) {
    if (name == NULL) name = "code.output";
    char* codeFileName = writeFileName(name);
    pt_ofstream codeFile(codeFileName);
    if (!codeFile) return;
    codeFile << myCode;
    codeFile.flush();
}

ISA_FUNC(CGTarget,Target);

StringList CGTarget::comment(const char* msg, const char* b, 
const char* e, const char* cont) {
    StringList cmt;
    if (msg==NULL) return cmt;
    char begin;
    if (b==NULL) begin ='#';
    else begin = *b;
    char end;
    if (e==NULL) end =' ';
    else end = *e;
    if (cont==NULL) {	/*multiline comments unsupported*/
	char* c = savestring(msg);
	for (char* line=strtok(c,"\n"); line; line=strtok(NULL,"\n"))
	    cmt << begin << line << end << "\n";
	LOG_DEL; delete c;
    }
    else if (*cont=='\0') /*multiline comments supported, no cont char*/
	cmt << begin << msg << end << "\n";
    else { /*multiline comments supported, continue char desired*/
	char* c = savestring(msg);
	char* line = strtok(c,"/n");
	cmt << begin << line << "\n";
	while(line = strtok(NULL,"\n"))
	    cmt << cont << line << '\n';
	cmt << end << '\n';
	LOG_DEL; delete c;
    }
    return cmt;
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
	if(i=rshSystem(host,command,workingDirectory()) != 0 && 
          error != NULL)
		Error::abortRun(error);
	return i;
}

const char* CGTarget::lookupSharedSymbol(const char* scope, const char* name)
{
    return sharedSymbol.lookup(scope, name);
}

void CGTarget :: switchCodeStream(Block* b, CodeStream* cs) {
	CGStar* s;
	if (b->isItAtomic()) {
		s = (CGStar*) b;
		s->myCode = cs;
	} else {
		Galaxy *g = (Galaxy*) b;
		GalStarIter nextS(*g);
		while ((s = (CGStar*) nextS++) != 0)
			s->myCode = cs;
	}
}

// do I support a given star
int CGTarget :: support(Star* s) {
	return (s->isA(starType()) || s->isA(auxStarClass()));
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

	// connect up the new star
	pfar->connect(*ip,delayBefore ? ndelay : 0);
	op->connect(*p,delayBefore ? 0 : ndelay);

	// save in the list of spliced stars
	spliceList.put(newb);

	// initialize the new star.  Name it and add it to the galaxy.
	// using size of splice list in name forces unique names.
	StringList newname("splice-");
	newname << newb->className() << "-" << spliceList.size();
	galaxy()->addBlock(*newb,hashstring(newname));
	newb->initialize();

	// check errors in initialization
	if (Scheduler::haltRequested()) return 0;
	return newb;
}

