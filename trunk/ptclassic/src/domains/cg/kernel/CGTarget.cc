static const char file_id[] = "CGTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: J. Buck, J. Pino, T. M. Parks

 Baseclass for all single-processor code generation targets.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

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
#include "ProfileTimer.h"
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
: Target(name,starclass,desc), defaultStream(&myCode), schedFileName(0), noSchedule(0),typeConversionTable(0),typeConversionTableRows(0)
  
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
	addState(loopingLevel.setState("Looping Level",this,"1",
		"Specify whether to use loop scheduler and in what level."));
	addState(displayFlag.setState("display?", this, "YES",
	    "Enable displaying of code."));
	addState(compileFlag.setState("compile?", this, "YES",
	    "Enable compilation of code."));
	addState(loadFlag.setState("load?", this, "YES",
	    "Enable loading of code."));
	addState(runFlag.setState("run?", this, "YES",
	    "Enable running of code."));
	addState(writeScheduleFlag.setState
		 ("write schedule?", this,"NO",
		  "Write the resultant schedule to a file."));

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
int CGTarget :: codeGenInit() { return TRUE; }
int CGTarget :: allocateMemory() { return TRUE; }
int CGTarget :: compileCode() { return TRUE; }
int CGTarget :: loadCode() { return TRUE; }
int CGTarget :: runCode() { return TRUE; }

CommPair CGTarget::fromCGC(PortHole&) {
    CommPair empty;
    Error::abortRun(*this,"This target does not support the CGWormTarget.");
    return empty;
}

CommPair CGTarget::toCGC(PortHole&) {
    CommPair empty;
    Error::abortRun(*this,"This target does not support the CGWormTarget.");
    return empty;
}

int setAsynchCommState(AsynchCommPair pair, const char* stateName,
		     const char* value) {
    if (pair.peek == NULL || pair.poke == NULL) return FALSE;
    State* peekState = pair.peek->stateWithName(stateName);
    State* pokeState = pair.poke->stateWithName(stateName);
    if (!(peekState && pokeState)) {
	Error::abortRun(
	    "Sorry, the peek/poke actors do not support ",stateName);
	return FALSE;
    }
	
    const char* pValue = hashstring(value);
    peekState->setInitValue(pValue);
    pokeState->setInitValue(pValue);
    peekState->initialize();
    pokeState->initialize();
    return TRUE;
}

AsynchCommPair CGTarget::createPeekPoke(CGTarget& /*peekTarget*/,
					CGTarget& /*pokeTarget*/) {
    AsynchCommPair dummy;
    return dummy;
}

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
	if (!parent()) {
	    if (!galaxy() || haltRequested()) return;
	    galaxy()->initialize();
	    if (!modifyGalaxy()) return;
	}

	if (inWormHole()) wormPrepare();

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

/*virtual*/ int CGTarget::schedulerSetup() {
    ProfileTimer schedulingTimer;
    int status = Target::schedulerSetup();

    if (status && int(writeScheduleFlag)) {
	double schedulingTime =  schedulingTimer.elapsedCPUTime();
	StringList schedule;
	schedule << headerComment("") << "\nScheduling time (seconds): "
		 << schedulingTime << "\n\n" << scheduler()->displaySchedule();
	writeFile(schedule,".sched",displayFlag);
    }
    return status;
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
	if (haltRequested()) return;
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
       // DO WE NEED THIS ANYMORE?  By the time we get here
       // the CGWormTarget has spliced in the communication stars. FIXME
       if (dp->far()->asEH())
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

int CGTarget :: modifyGalaxy() {
    if(haltRequested()) return FALSE;
    if(!typeConversionTable) return TRUE;

    extern int warnIfNotConnected (Galaxy&);

    Galaxy& gal = *galaxy();
    GalStarIter starIter(gal);
    Star* star;
    const char* domain = gal.domain();

    // type conversion table;
    // procession takes place in chronological order

    if (warnIfNotConnected(gal)) return FALSE;

    while ((star = starIter++) != NULL) {
	BlockPortIter portIter(*star);
	PortHole* port;
	while ((port = portIter++) != NULL) {
	    // Splice in type conversion stars.
	    if (needsTypeConversionStar(*port)) {
		PortHole* input = port->far();	// destination input PortHole

		int i;
		for (i=0; i < typeConversionTableRows; i++) {
		    if (((port->type() == typeConversionTable[i].src) ||
			 (typeConversionTable[i].src == ANYTYPE)) &&
			((port->resolvedType() ==
			  typeConversionTable[i].dst) ||
			 (typeConversionTable[i].dst == ANYTYPE)))
		    {
			Block* s = 0;
			if (!(s = spliceStar(input,
					     typeConversionTable[i].star,
					     TRUE, domain)))
			    return FALSE;
			else {
			    s->setTarget(this);
			}
		    }
		}
	    }
	}
    }
    return TRUE;
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
	StringList newname("splice_");
	newname << newb->className() << "_" << spliceList.size();

	// try and resolve which galaxy we should add the splice
	// block to.  Ideally we want the block added to one of the
	// galaxies of the stars that we are connecting this spliced
	// star to.
	Galaxy* addTo;

	if (p->atBoundary() && p->isItOutput()) {
	    addTo = (Galaxy*) pfar->parent()->parent();
	    if (pfar->aliasFrom()) {
		GenericPort* galPort = pfar->aliasFrom();
		pfar->clearAliases();
		galPort->setAlias(*op);
	    }
	}
	else {
	    addTo = (Galaxy*) p->parent()->parent();
	    if (p->aliasFrom()) {
		GenericPort* galPort = p->aliasFrom();
		p->clearAliases();
		galPort->setAlias(*ip);
	    }
	}
	
	addTo->addBlock(*newb,hashstring(newname));
	newb->setParent(addTo);

 	// connect up the new star.  This must be done after adding the
	// block to the galaxy so that the geodesics are allocated
	// properly.  It must be also be done before initializing so that
	// the spliced in block can resolve the data type on its ports.
	pfar->connect(*ip,delayBefore ? ndelay : 0);
	op->connect(*p,delayBefore ? 0 : ndelay);

	newb->initialize();

	// save in the list of spliced stars
	spliceList.put(newb);

	// check errors in initialization
	if (Scheduler::haltRequested()) return 0;

	return newb;
}

int CGTarget::needsTypeConversionStar(PortHole& port) {
    if (port.isItOutput()) {
	// splice conversion star if type of output port does not
	// match the type of the data connection
	if ((port.type() != port.resolvedType()) && (port.type() != ANYTYPE))
	    return TRUE;
    }
    return FALSE;
}

