static const char file_id[] = "CG56FictionTarget.cc";
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
#include "CG56FictionTarget.h"
#include "CG56FictionSend.h"
#include "CG56FictionReceive.h"
#include "CG56Target.h"
#include "KnownTarget.h"
#include "FixState.h"

const char* CG56FictionTarget::auxStarClass() const {return "AnyAsmStar";}
// -----------------------------------------------------------------------------	
CG56FictionTarget::CG56FictionTarget(const char* name,const char* starclass,
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
Target* CG56FictionTarget :: createChild() {
	LOG_NEW; return new CG56Target("single-CG56",
	"56000 code target for a Fiction Target.");
}

// -----------------------------------------------------------------------------
DataFlowStar* CG56FictionTarget :: createSend(int from, int to, int num) {
	LOG_NEW; CG56FictionSend* s = new CG56FictionSend;
	s->setProperty(num);
	return s;
}

DataFlowStar* CG56FictionTarget :: createReceive(int from, int to, int num) {
	LOG_NEW; CG56FictionReceive* r =  new CG56FictionReceive;
	r->setProperty(num);
	return r;
}

void CG56FictionTarget :: pairSendReceive(DataFlowStar* s, DataFlowStar* r) {
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

void CG56FictionTarget :: setup() {
	LOG_DEL; delete sharedMem;
	LOG_NEW; sharedMem = new LinProcMemory("x",ANY,ANY,sMemMap);
	CGMultiTarget :: setup();
	destDirectory.setValue("~/DSPcode");
	// allocate the sharedMemory
	sharedMem->performAllocation();
	Galaxy* g = galaxy();
	if (g && (g->stateWithName("ONE") == 0)) {
		LOG_NEW; FixState& ONE = *new FixState;
		g->addState(ONE.setState("ONE",this,"",
					"Max Fix point value",
					A_NONSETTABLE|A_CONSTANT));
		ONE.setInitValue(CG56_ONE);
	}
}

			///////////////////
			// wrapup
			///////////////////

void CG56FictionTarget :: wrapup() {
	if (galaxy()->parent() == 0)		 wormLoadCode();
}
// -----------------------------------------------------------------------------

void CG56FictionTarget :: addProcessorCode(int i, const char* s) {
	StringList code = s;
	StringList fileName;
	fileName << (const char*) filePrefix << i << ".asm";
	char* codeFileName = writeFileName((const char*) fileName);
	display(code,codeFileName);

// 	to create the .cmd file
	StringList fName;
	fName << "command" << i << ".cmd";
	StringList cmd = "load ";
	cmd << (const char*) filePrefix << i << "\n";
	cmd << "go \n";
	char* cmdFileName = writeFileName((const char*) fName);
	display(cmd,cmdFileName);
}

			///////////////////
			// wormLoadCode
			///////////////////

int CG56FictionTarget::wormLoadCode() {

    if (compileCode()) runCode();

    // done
    if(Scheduler::haltRequested()) return FALSE;
    return TRUE;
}

// -----------------------------------------------------------------------------
int CG56FictionTarget :: compileCode() {
	if (int(doCompile) == 0) return TRUE;

	int flag = TRUE;
	for (int i = 0; i < nChildrenAlloc; i++) {
		StringList fileName;
		fileName << (const char*) filePrefix << i << ".asm";
		char* codeFileName = writeFileName((const char*) fileName);

		StringList assembleCmds;
		assembleCmds << "asm56000" << "-A -b -l " << codeFileName;
		flag = !systemCall(assembleCmds,"Errors in assembly");
		if (flag == FALSE) break;
	}
	return flag;
}

// -----------------------------------------------------------------------------
int CG56FictionTarget :: runCode() {
    return TRUE;
}
// -----------------------------------------------------------------------------
Block* CG56FictionTarget :: makeNew() const {
	LOG_NEW; return new CG56FictionTarget(name(),starType(),descriptor());
}
// -----------------------------------------------------------------------------
			/////////////////////////////
			// wormhole interface method
			/////////////////////////////

int CG56FictionTarget :: receiveWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceSendData();
	return TRUE;
}
// -----------------------------------------------------------------------------
int CG56FictionTarget :: sendWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceGrabData();
	return TRUE;
}
// -----------------------------------------------------------------------------
ISA_FUNC(CG56FictionTarget,CGMultiTarget);

static CG56FictionTarget targ("Fiction-56000","CG56Star",
"A test target for parallel MC56000-code generation");

static KnownTarget entry(targ,"Fiction-56000");

