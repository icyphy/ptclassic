static const char file_id[] = "CG56MultiSimTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: S. Ha, J. Pino

 Fiction target

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "pt_fstream.h"
#include "Error.h"
#include "CGDisplay.h"
#include "CG56MultiSimTarget.h"
#include "CG56MultiSimSend.h"
#include "CG56MultiSimReceive.h"
#include "CG56Target.h"
#include "KnownTarget.h"
#include "FixState.h"
#include "pt_fstream.h"

// ----------------------------------------------------------------------------	
CG56MultiSimTarget::CG56MultiSimTarget(const char* name,const char* starclass,
	const char* desc) : CGMultiTarget(name,starclass,desc), sharedMem(0) {

        addState(doCompile.setState("doCompile",this,"NO",
                "disallow compiling during development stage"));

	// make some states invisible
	childType.setAttributes(A_NONSETTABLE);

	sharedMem = 0;
	addState(sMemMap.setState("sMemMap",this,"4096-4195",
		"shared memory map"));
	destDirectory.setValue("~/DSPcode");
}

// -----------------------------------------------------------------------------
Target* CG56MultiSimTarget :: createChild() {
	LOG_NEW; return new CG56Target("single-CG56",
	"56000 code target for a Multiple-Simulator Target.");
}

// -----------------------------------------------------------------------------
DataFlowStar* CG56MultiSimTarget :: createSend(int from, int to, int num) {
	LOG_NEW; CG56MultiSimSend* s = new CG56MultiSimSend;
	s->setProperty(num);
	return s;
}

DataFlowStar* CG56MultiSimTarget :: createReceive(int from, int to, int num) {
	LOG_NEW; CG56MultiSimReceive* r =  new CG56MultiSimReceive;
	r->setProperty(num);
	return r;
}

void CG56MultiSimTarget :: pairSendReceive(DataFlowStar* s, DataFlowStar* r) {
	// connect send and receive
	InCG56Port& rp = ((CG56MultiSimReceive*) r)->input;
	OutCG56Port& sp = ((CG56MultiSimSend*) s)->output;
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

void CG56MultiSimTarget :: setup() {
	LOG_DEL; delete sharedMem;
	LOG_NEW; sharedMem = new LinProcMemory("x",ANY,ANY,sMemMap);
	CGMultiTarget :: setup();
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

void CG56MultiSimTarget :: wrapup() {
	if (galaxy()->parent() == 0)		 wormLoadCode();
}
// -----------------------------------------------------------------------------

void CG56MultiSimTarget :: addProcessorCode(int i, const char* s) {
	StringList code = s;
	StringList fileName;
	fileName << (const char*) filePrefix << i << ".asm";
	char* codeFileName = writeFileName((const char*) fileName);
	display(code,codeFileName);
	LOG_DEL; delete codeFileName;

// 	to create the .cmd file
	StringList fName;
	fName << "command" << i << ".cmd";
	char* cmdFileName = writeFileName((const char*) fName);
	pt_ofstream cmd(cmdFileName); if (!cmd) return; // failed!
	cmd << "load " << (const char*) filePrefix << i << "\n";
	cmd << "go \n";
	LOG_DEL; delete cmdFileName;
}

			///////////////////
			// wormLoadCode
			///////////////////

int CG56MultiSimTarget::wormLoadCode() {

    if (compileCode()) runCode();

    // done
    if(Scheduler::haltRequested()) return FALSE;
    return TRUE;
}

// -----------------------------------------------------------------------------
int CG56MultiSimTarget :: compileCode() {
	if (int(doCompile) == 0) return TRUE;

	int flag = TRUE;
	for (int i = 0; i < nChildrenAlloc; i++) {
		StringList fileName;
		fileName << (const char*) filePrefix << i << ".asm";
		char* codeFileName = writeFileName((const char*) fileName);

		StringList assembleCmds;
		assembleCmds << "asm56000 -A -b -l " << codeFileName;
		flag = !systemCall(assembleCmds,"Errors in assembly");
		if (flag == FALSE) break;
		LOG_DEL; delete codeFileName;
	}
	return flag;
}

// -----------------------------------------------------------------------------
int CG56MultiSimTarget :: runCode() {
    return TRUE;
}
// -----------------------------------------------------------------------------
Block* CG56MultiSimTarget :: makeNew() const {
	LOG_NEW; return new CG56MultiSimTarget(name(),starType(),descriptor());
}
// -----------------------------------------------------------------------------
			/////////////////////////////
			// wormhole interface method
			/////////////////////////////

int CG56MultiSimTarget :: receiveWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceSendData();
	return TRUE;
}
// -----------------------------------------------------------------------------
int CG56MultiSimTarget :: sendWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceGrabData();
	return TRUE;
}
// -----------------------------------------------------------------------------
ISA_FUNC(CG56MultiSimTarget,CGMultiTarget);

static CG56MultiSimTarget targ("MultiSim-56000","CG56Star",
"A test target for parallel MC56000-code generation");

static KnownTarget entry(targ,"MultiSim-56000");

