static const char file_id[] = "FictionTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: S. Ha

 Fiction target

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "pt_fstream.h"
#include "Error.h"
#include "CGDisplay.h"
#include "FictionTarget.h"
#include "CG56FictionSend.h"
#include "CG56FictionReceive.h"
#include "CG56Target.h"
#include "KnownTarget.h"

// -----------------------------------------------------------------------------	
FictionTarget::FictionTarget(const char* name,const char* starclass,
	const char* desc) : CGMultiTarget(name,starclass,desc), sharedMem(0) {

        addState(doCompile.setState("doCompile",this,"NO",
                "disallow compiling during development stage"));

	// make some states invisible
	childType.setAttributes(A_NONSETTABLE);

	sharedMem = 0;
	addState(sMemMap.setState("sMemMap",this,"4096-4195",
		"shared memory map"));
}

// -----------------------------------------------------------------------------
Target* FictionTarget :: createChild() {
	LOG_NEW; return new CG56Target("single-CG56",
	"56000 code target for a Fiction Target.");
}

// -----------------------------------------------------------------------------
DataFlowStar* FictionTarget :: createSend(int from, int to, int num) {
	LOG_NEW; CG56FictionSend* s = new CG56FictionSend;
	s->setProperty(num);
	return s;
}

DataFlowStar* FictionTarget :: createReceive(int from, int to, int num) {
	LOG_NEW; CG56FictionReceive* r =  new CG56FictionReceive;
	r->setProperty(num);
	return r;
}

void FictionTarget :: pairSendReceive(DataFlowStar* s, DataFlowStar* r) {
	// connect send and receive
	InCG56Port& rp = ((CG56FictionReceive*) r)->input;
	OutCG56Port& sp = ((CG56FictionSend*) s)->output;
	sp.connect(rp,0);

	// memory allocation
	r->repetitions = Fraction(1);
	sharedMem->allocReq(rp);
}

// -----------------------------------------------------------------------------
			///////////////////
			// setup
			///////////////////

const Attribute ANY = {0,0};

void FictionTarget :: setup() {
	LOG_DEL; delete sharedMem;
	LOG_NEW; sharedMem = new LinProcMemory("x",ANY,ANY,sMemMap);
	CGMultiTarget :: setup();

	// allocate the sharedMemory
	sharedMem->performAllocation();
}

			///////////////////
			// wrapup
			///////////////////

void FictionTarget :: wrapup() {
	if (galaxy()->parent() == 0)		 wormLoadCode();
}
// -----------------------------------------------------------------------------

void FictionTarget :: addProcessorCode(int i, const char* s) {
	StringList code = s;
	StringList fileName;
	fileName << i ;
	fileName << (const char*) filePrefix;
	fileName << ".asm";
	char* codeFileName = writeFileName((const char*) fileName);
	display(code,codeFileName);

// 	to create the .cmd file
	StringList fName;
	fName << i ;
	fName << "command";
	fName << ".cmd";
	StringList cmd = "load ";
	cmd << i << (const char*) filePrefix << "\n";
	cmd << "go \n";
	char* cmdFileName = writeFileName((const char*) fName);
	display(cmd,cmdFileName);
}

			///////////////////
			// wormLoadCode
			///////////////////

int FictionTarget::wormLoadCode() {

    if (compileCode()) runCode();

    // done
    if(Scheduler::haltRequested()) return FALSE;
    return TRUE;
}

// -----------------------------------------------------------------------------
int FictionTarget :: compileCode() {
	if (int(doCompile) == 0) return TRUE;

	int flag = TRUE;
	for (int i = 0; i < nChildrenAlloc; i++) {
		StringList fileName;
		fileName << i ;
		fileName << (const char*) filePrefix;
		fileName << ".asm";
		char* codeFileName = writeFileName((const char*) fileName);

		StringList assembleCmds  = "asm56000";
		assembleCmds += " -A -b -l ";
		assembleCmds += codeFileName;
		flag = !systemCall(assembleCmds,"Errors in assembly");
		if (flag == FALSE) break;
	}
	return flag;
}

// -----------------------------------------------------------------------------
int FictionTarget :: runCode() {
    return TRUE;
}
// -----------------------------------------------------------------------------
Block* FictionTarget :: makeNew() const {
	LOG_NEW; return new FictionTarget(name(),starType(),descriptor());
}
// -----------------------------------------------------------------------------
			/////////////////////////////
			// wormhole interface method
			/////////////////////////////

int FictionTarget :: receiveWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceSendData();
	return TRUE;
}
// -----------------------------------------------------------------------------
int FictionTarget :: sendWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceGrabData();
	return TRUE;
}
// -----------------------------------------------------------------------------
ISA_FUNC(FictionTarget,CGMultiTarget);

static FictionTarget targ("Fiction-56000","CG56Star",
"A test target for parallel MC56000-code generation");

static KnownTarget entry(targ,"Fiction-56000");

