static const char file_id[] = "CGTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

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
#include "SDFCluster.h"
#include "CGDisplay.h"
#include "miscFuncs.h"
#include "CGUtilities.h"
#include "pt_fstream.h"
#include <stream.h>
#include <time.h>
#include <string.h>

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
: Target(name,starclass,desc), schedFileName(0), noSchedule(0)
{
	separator = sep;
	targetNestedSymbol.setSeparator(separator);
	addState(destDirectory.setState("destDirectory",this,
		"PTOLEMY_SYSTEMS","Directory to write to"));
	addState(loopScheduler.setState("loopScheduler",this,"NO",
			"Specify whether to use loop scheduler."));
	addStream("myCode",&myCode);
	addStream("procedures",&procedures);
}

// destructor
CGTarget::~CGTarget() {
	delSched();
	LOG_DEL; delete schedFileName;
}

// dummy functions that perform parts of setup.
int CGTarget :: modifyGalaxy() { return TRUE; }
int CGTarget :: allocateMemory() { return TRUE; }
int CGTarget :: codeGenInit() { return TRUE; }

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
		if(int(loopScheduler)) {
			schedFileName = writeFileName("schedule.log");
			LOG_NEW; setSched(new SDFClustSched(schedFileName));
		} else {
			LOG_NEW; setSched(new SDFScheduler);
		}
	}
	if (!galaxy()) return;

	// reset the label counter
	BaseSymbolList::reset() ;

	// reset nestedSymbols
	targetNestedSymbol.initialize();

	if (!noSchedule) {
		if (!modifyGalaxy()) return;
		Target::setup();
		if (haltRequested()) return;
	}
	noSchedule = 0;		// reset for next setup.

	// choose sizes for buffers and allocate memory, if needed
	if (inWormHole() && (parent() == NULL)) {
		generateCode();
		wormLoadCode();
	}
}

void CGTarget::generateCode() {
	if (parent()) setup();
	headerCode();
	if(!allocateMemory() || !codeGenInit()) return;
	mainLoopCode();
	Target :: wrapup();
	trailerCode();
	frameCode();
	if (!parent()) writeCode();
}

void CGTarget :: mainLoopCode() {
	// Generate Code.  Iterate an infinite number of times if 
	// target is inside a wormhole
	int iterations = inWormHole()? -1 : (int)scheduler()->getStopTime();
	beginIteration(iterations,0);
	if (inWormHole()) wormInputCode();
	scheduler()->compileRun();
	if (inWormHole()) wormOutputCode(); 
	endIteration(iterations,0);
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
	if(!sendWormData()) return FALSE;
	if(!receiveWormData()) return FALSE;
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

int CGTarget :: compileCode() {
	Error::abortRun("No compileCode method defined for current target");
	return FALSE;
}

int CGTarget :: loadCode() {
	return TRUE; // load and Run can be combined in runCode method
}

int CGTarget :: runCode() {
	Error::abortRun("No runCode method defined for current target");
	return FALSE;
}

int CGTarget::inWormHole() {
	return int(galaxy()->parent());
}

void CGTarget :: wormInputCode() {
	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItInput()) wormInputCode(p->newConnection());
	}
}
void CGTarget :: wormInputCode(PortHole& p) {
	myCode << "/* READ from wormhole port " << p.fullName() << " */\n";
}

void CGTarget :: wormOutputCode() {
	BlockPortIter nextPort(*galaxy());
	PortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isItOutput()) wormOutputCode(p->newConnection());
	}
}
void CGTarget :: wormOutputCode(PortHole& p) {
	myCode << "/* WRITE to wormhole port " << p.fullName() << " */\n";
}

int CGTarget :: sendWormData() {
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

int CGTarget :: receiveWormData() {
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

int CGTarget :: wormLoadCode() {
	display(myCode);
	return TRUE;
}

void CGTarget :: writeFiring(Star& s,int) { // depth parameter ignored
	s.run();
}

void CGTarget :: wrapup() {
	display(myCode);
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
	LOG_NEW; char* c = new char[strlen(msg)];
	strcpy(c,msg);
	for (char* line=strtok(c,"\n"); line; line=strtok(NULL,"\n"))
	    cmt << begin << line << end << "\n";
	LOG_DEL; delete(c);
    }
    else if (*cont=='\0') /*multiline comments supported, no cont char*/
	cmt << begin << msg << end << "\n";
    else { /*multiline comments supported, continue char desired*/
	LOG_NEW; char* c = new char[strlen(msg)];
	char* line = strtok(c,"/n");
	cmt << begin << line << "\n";
	while(line = strtok(NULL,"\n"))
	    cmt << cont << line << '\n';
	cmt << end << '\n';
	LOG_DEL; delete(c);
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
	
int CGTarget :: systemCall(const char* command, const char* error) {
	int i;
	if(i=rshSystem(NULL,command,workingDirectory()) != 0 && 
          error != NULL)
		Error::abortRun(error);
	return i;
}
