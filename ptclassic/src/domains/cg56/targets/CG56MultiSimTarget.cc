static const char file_id[] = "CG56MultiSimTarget.cc";
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

 Programmer: S. Ha, Jose Luis Pino

 Fiction target

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "pt_fstream.h"
#include "Error.h"
#include "CGUtilities.h"
#include "CG56MultiSimTarget.h"
#include "CG56MultiSimSend.h"
#include "CG56MultiSimReceive.h"
#include "CG56Target.h"
#include "KnownTarget.h"
#include "FixState.h"
#include "pt_fstream.h"
#include "SimControl.h"

// ----------------------------------------------------------------------------	
CG56MultiSimTarget::CG56MultiSimTarget(const char* name,const char* starclass,
	const char* desc) : CGMultiTarget(name,starclass,desc), sharedMem(0) {

	starTypes += "AnyAsmStar";
	// make some states invisible
	childType.setInitValue("default-CG56");
	childType.setAttributes(A_NONSETTABLE);

	sharedMem = 0;
	addState(sMemMap.setState("sMemMap",this,"4096-4195",
		"shared memory map"));
	runFlag.setAttributes(A_NONSETTABLE);
}

// -----------------------------------------------------------------------------
Target* CG56MultiSimTarget :: createChild(int) {
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
	addCG56One(this,galaxy());
	LOG_DEL; delete sharedMem;
	LOG_NEW; sharedMem = new LinProcMemory("x",ANY,ANY,sMemMap);
	CGMultiTarget :: setup();
}

void CG56MultiSimTarget :: prepareCodeGen() {

       	// allocate the sharedMemory
	sharedMem->performAllocation();
}

void CG56MultiSimTarget :: writeCode() 
{
	// Note: this also writes the command file for the Motorola simulators;
	for (int i = 0 ; i < nChildrenAlloc ; i++) 
	{
		((CGTarget*)child(i))->writeCode();
		 //    create the .cmd file
		StringList fName,cmd;
		fName << "command" << i << ".cmd";
		cmd << "load " << (const char*) filePrefix << i << "\n";
		cmd << "go \n";
		rcpWriteFile("localhost",destDirectory,fName,cmd);
	}
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

