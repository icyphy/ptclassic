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

// Ptolemy kernel includes
#include "Domain.h"
#include "Block.h"
#include "KnownBlock.h"
#include "GalIter.h"
#include "Error.h"
#include "EventHorizon.h"
#include "miscFuncs.h"
#include "pt_fstream.h"
#include "SimControl.h"
#include "Tokenizer.h"
#include "ProfileTimer.h"
#include "checkConnect.h"

// Ptolemy domain includes
#include "LoopScheduler.h"
#include "AcyLoopScheduler.h"
#include "SDFScheduler.h"
#include "SDFCluster.h"
#include "CGUtilities.h"
#include "CGStar.h"
#include "CGPortHole.h"

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>			// define stat structure

extern const char* CODE = "code";
extern const char* PROCEDURE = "procedure";

// constructor
CGTarget::CGTarget(const char* name, const char* starclass,
		   const char* desc, const char* assocDomain, char sep) :
Target(name, starclass, desc, assocDomain), defaultStream(&myCode),
schedFileName(0), noSchedule(0), typeConversionTable(0)
{
	counter = 0;
	spliceList.initialize();

	separator = sep;
	targetNestedSymbol.setSeparator(separator);
	targetNestedSymbol.setCounter(symbolCounter());
	sharedSymbol.setSeparator(separator);
	sharedSymbol.setCounter(symbolCounter());

	starTypes += "HOFStar";
	starTypes += "AnyCGStar";

	// The value of the destination directory must be stored in a data
	// member.  If it were a local variable, then its contents would be
	// deleted when the constructor finishes.
	destDirName = destDirectoryName(assocDomain);

	addState(targetHost.setState("host", this, "",
	    "Host machine to compile or assemble code on."));
	addState(destDirectory.setState("directory", this,
	    destDirName, "Directory to write to"));
	addState(filePrefix.setState("file", this, "",
	    "Prefix for file names."));
	addState(loopingLevel.setState
		("Looping Level",this,"ACYLOOP #choices are: DEF, CLUST, SJS, ACYLOOP",
		"SDF Schedulers:\n"
		"\tDEF - The default SDF scheduler\n"
		"\tCLUST - J. Buck's loop scheduler\n"
		"\tSJS - Shuvra/Joe/Soonhoi's loop scheduler\n"
		"\tACYLOOP - P. Murthy/S. Bhattacharyya's loop scheduler\n"));
	addState(displayFlag.setState("display?", this, "YES",
	    "Enable displaying of code."));
	addState(compileFlag.setState("compile?", this, "YES",
	    "Enable compilation of code."));
	addState(loadFlag.setState("load?", this, "YES",
	    "Enable loading of code."));
	addState(runFlag.setState("run?", this, "YES",
	    "Enable running of code."));
	addState(writeScheduleFlag.setState("write schedule?", this,"NO",
	    "Write the resultant schedule to a file."));
        addState(reportMemoryUsage.setState("show memory usage?", this, "NO",
	    "Report memory usage"));
	addState(reportExecutionTime.setState("show run time?", this, "NO",
	    "Report execution time"));

	// Hide parameters by making them nonsettable.
	// Derived Targets can re-enable those that are appropriate.
	targetHost.clearAttributes(A_SETTABLE);
	filePrefix.clearAttributes(A_SETTABLE);
	displayFlag.clearAttributes(A_SETTABLE);
	compileFlag.clearAttributes(A_SETTABLE);
	loadFlag.clearAttributes(A_SETTABLE);
	runFlag.clearAttributes(A_SETTABLE);
	reportMemoryUsage.clearAttributes(A_SETTABLE);
	reportExecutionTime.clearAttributes(A_SETTABLE);

	// Initialize code streams and the code string list
	procedures.initialize();
        myCode.initialize();
	makefile.initialize();
	makeAllList.initialize();
	codeStringLists.initialize();

	addStream(CODE, &myCode);
	addStream(PROCEDURE, &procedures);
}

// destructor
CGTarget::~CGTarget() {
	delSched();
	LOG_DEL; delete [] schedFileName;
}

// Return a string for indenting to the given depth
StringList CGTarget::indent(int depth) {
	StringList out = "";
	for(int i = 0; i < depth; i++) {
		out += "    ";
	}
	return out;
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


	if (!scheduler()) chooseScheduler();

	if (!galaxySetup()) return;
	if (haltRequested()) return;
	if (filePrefix.null() && galaxy()) filePrefix = galaxy()->name();

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

	// error checking
	if (!noSchedule) {
		if(!schedulerSetup()) return;
	}
	if (haltRequested()) return;

	// reset for next setup
	noSchedule = 0;

	// If in a WormHole, generate, compile, load, and run code.
	// Ignore flags which may otherwise disable these functions.
	if (inWormHole() && alone()) {
	    adjustSampleRates();
	    generateCode();
	    if (haltRequested()) return;
	    if (compileCode()) {
		if (loadCode()) {
		    if (!runCode())
			Error::abortRun(*this, "could not run!");
		}
		else {
		    Error::abortRun(*this, "could not load!");
		}
	    }
	    else {
		Error::abortRun(*this, "could not compile!");
	    }
	}
}

void CGTarget :: chooseScheduler() {

    const char* tmpname = loopingLevel;
    Tokenizer lexer(tmpname,"#");
    char buf[1024];
    lexer >> buf;
    loopingLevel.setCurrentValue(buf);

    // Full path name of the log file
    StringList logPath = logFilePathName(destDirectory, "schedule.log");
	delete [] schedFileName;
	schedFileName = logPath.newCopy();

    if (strcasecmp(buf,"ACYLOOP") == 0) {
	// Determine if the graph is acyclic.  It not, use
	// another loop scheduler.
	// FIXME:
	// NOTE: Error::message will halt the simulation
	// till the message is dismissed.  This will be fixed
	// later on when we use something to post messages that
	// does not require dismissal.
	if (galaxy()) {
	    if (!isAcyclic(galaxy(), 0)) {
		StringList message;
		message << "The scheduler option that you selected, "
		<< "ACYLOOP, "
		<< "in the SDFTarget parameters represents a "
		<< "loop scheduler that only works on acyclic "
		<< "graphs.  Since this graph is not acyclic "
		<< "the scheduler SJS will be used "
		<< "(corresponding to old option 2).\n";
		loopingLevel.setCurrentValue("SJS");
	    }
	}
    }
    const char* sname = loopingLevel;

    if (strcasecmp(sname,"DEF")==0 || strcmp(sname,"0")==0 ||
		strcasecmp(sname,"NO") == 0) {
	setSched(new SDFScheduler);
    } else if (strcasecmp(sname,"CLUST")==0 || strcmp(sname,"1")==0) {
	setSched(new SDFClustSched(schedFileName));
    } else if (strcasecmp(sname,"SJS") == 0 || strcmp(sname,"2")==0 ||
		strcasecmp(sname,"YES")==0 ) {
	setSched(new LoopScheduler(schedFileName));
    } else if (strcasecmp(sname,"ACYLOOP") == 0) {
	setSched(new AcyLoopScheduler(schedFileName));
    } else {
	Error::abortRun(*this, "Unknown scheduler.");
	return;
    }
}

int CGTarget::schedulerSetup() {
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
    if (!scheduler()) {
	Error::abortRun(*this, "Target has no scheduler");
	return FALSE;
    }
    if (!galaxy()) {
	Error::abortRun(*this, "Target has no galaxy");
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
	if(!allocateMemory()) {
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

// If there is a CGTarget parent, return its counter.
// Otherwise, return the counter for this target.
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
	if (!codeGenInit()) {
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
	Galaxy* saveGal = galaxy();	// method will work fine if NULL
	setGalaxy(*g);
	Target& saveT = s->target();
	s->setTarget(*this);

        if(!allocateMemory() || !codeGenInit()) return FALSE;
        compileRun(s);
        Target :: wrapup();

	// restore info
	if (saveGal) setGalaxy(*saveGal);
	s->setTarget(saveT);
	return TRUE;
}

void CGTarget :: compileRun(SDFScheduler* s) {
	s->compileRun();
}

void CGTarget :: addStream(const char* name, CodeStream* code)
{
	if (! codeStringLists.append(code, name) ) {
		StringList message = "addStream: ";
		message << name << " already exists";
		Error::abortRun(*this, message);
	}
}

// Lookup a CodeStream by name.
CodeStream* CGTarget :: getStream(const char* name)
{
    CodeStream* stream = name ? codeStringLists.get(name) : defaultStream;

    if (stream == NULL) {
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
	LOG_NEW; return new CGTarget(name(), starType(), descriptor(),
				     getAssociatedDomain());
}

void CGTarget :: headerCode () {
	*defaultStream << headerComment();
}

void CGTarget :: trailerCode() {}

void CGTarget :: beginIteration(int reps, int depth) {
     *defaultStream << "REPEAT " << reps << " TIMES { /* depth " << depth << "*/\n";
}

void CGTarget :: endIteration(int /*reps*/, int depth) {
     *defaultStream << "} /* end repeat, depth " << depth << "*/\n";
}


int CGTarget :: incrementalAdd(CGStar*, int) {
	Error:: abortRun("No Incremental-Add(so, no macro) supported yet");
	return FALSE;
}


int CGTarget::inWormHole() {
	return int(galaxy() && galaxy()->parent());
}

// If a CG domain is inside a wormhole, we may need to change
// the sample rate of event horizons after scheduling is performed.
void CGTarget :: adjustSampleRates() {
   if (! galaxy()) return;
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
	if (! galaxy()) return;
	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItInput()) wormInputCode(p->newConnection());
	}
}
void CGTarget :: wormInputCode(PortHole& p) {
	*defaultStream << "/* READ from wormhole port " << p.fullName() << " */\n";
}

void CGTarget :: allWormOutputCode() {
	if (! galaxy()) return;
	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItOutput()) wormOutputCode(p->newConnection());
	}
}
void CGTarget :: wormOutputCode(PortHole& p) {
	*defaultStream << "/* WRITE to wormhole port " << p.fullName() << " */\n";
}

int CGTarget :: allSendWormData() {
    if (! galaxy()) return FALSE;
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
    if (! galaxy()) return FALSE;
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

int CGTarget::writeFile(const char* text, const char* suffix,
			int display, int mode){
    StringList fileName;
    fileName << filePrefix << suffix;
    int status = rcpWriteFile(targetHost, destDirectory, fileName, text, 
			      display, mode);
    if (status) {
	return TRUE;
    }
    else {
	StringList errMsg = "Writing to the file ";
	errMsg << fileName << " was unsuccessful.";
	Error::abortRun(*this, errMsg);
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
    // don't want Tokenizer to use # as the comment character
    (void)lexer.setCommentChar((char)0);
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
	StringList header, msg;
	time_t t = time(NULL);
	const char* galname = galaxy() ? galaxy()->name() : "-- UNKNOWN --";
	msg 	<< "User:      " << cuserid(NULL) << "\n"
		<< "Date:      " << ctime(&t)
		<< "Target:    " << fullName() << "\n"
		<< "Universe:  " << galname << "\n";
	if (begin == NULL) header << comment(msg);
	else header << comment(msg, begin, end, cont);
	return header;
}
	
int CGTarget::systemCall(const char* command, const char* error,
			 const char* host) {
	int i = rshSystem(host, command, destDirectory);
	if( i != 0 && error != NULL) Error::abortRun(error);
	return i;
}

const char* CGTarget::lookupSharedSymbol(const char* scope, const char* name)
{
    return sharedSymbol.lookup(scope, name);
}

int CGTarget :: modifyGalaxy() {
    if(! galaxy()) return FALSE;
    if(haltRequested()) return FALSE;

    // If the domain has no type conversion table,
    // assume it doesn't care about type mismatches
    // between source and destination ports.
    if (!typeConversionTable) return TRUE;

    // Splice type conversion stars as necessary.
    Galaxy& gal = *galaxy();

    if (warnIfNotConnected(gal)) return FALSE;

    const char* domain = gal.domain();
    GalStarIter starIter(gal);
    Star* star;

    while ((star = starIter++) != NULL) {
	BlockPortIter portIter(*star);
	PortHole* port;
	while ((port = portIter++) != NULL) {
	    if (needsTypeConversionStar(*port)) {
		// Need a type conversion star here.
		Block* s = 0;
		for (int i=0; i < typeConversionTable->numEntries(); i++) {
		    const ConversionTableRow& row =
		      * typeConversionTable->entry(i);
		    if (((port->preferredType() == row.src) ||
			 (row.src == ANYTYPE)) &&
			((port->resolvedType() == row.dst) ||
			 (row.dst == ANYTYPE)))
		    {
			PortHole* input = port->far(); // destination PortHole
			if (!(s = spliceStar(input, row.star, TRUE, domain)))
			    return FALSE;
			s->setTarget(this);
			break;	// out of table-searching loop
		    }
		}
		if (s == 0) {
		    // Failed to find a suitable conversion star.
		    StringList phInfo = "source type: ";
		    phInfo << port->preferredType() << ", destination type: "
			   << port->resolvedType();
		    Error::abortRun(*port,
			"No type conversion star available to convert:\n",
			(const char*) phInfo);
		    return FALSE;
		}
	    }
	}
    }

    // Additional post-check to mirror pre-check.
    // If gal wasn't broke, don't break it.
    if (warnIfNotConnected(gal)) return FALSE;

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
	    if (port.preferredType() != port.resolvedType())
		return TRUE;
	}
	return FALSE;
}

StringList destDirectoryName(const char* subdir) {
	StringList dir = "$HOME/PTOLEMY_SYSTEMS";
	if (subdir) {
	    dir << "/" << subdir;
	}
	return dir;
}

// If logFile is blank, return an empty StringList, otherwise return
// the entire path name of the log file, including its directory.
// Create the directory dir if it does not exist.
StringList CGTarget::logFilePathName(const char* dir, const char* logfile) {
	StringList pathname = "";

	// Create a directory on the local machine
	makeWriteableDirectory(0, dir);

	// Build up the complete path name of the log file
	if (logfile) {
	  if (strlen(logfile)) {
	    pathname << dir << "/" << logfile;
	    char* expandedPath = expandPathName(pathname);
	    pathname = expandedPath;
	    delete [] expandedPath;
	  }
	}

	return pathname;
}
