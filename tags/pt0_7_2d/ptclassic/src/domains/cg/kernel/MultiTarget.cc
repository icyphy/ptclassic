static const char file_id[] = "MultiTarget.cc";

/******************************************************************
Version identification:
@(#)MultiTarget.cc	1.38	12/08/97

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

 Programmer: Soonhoi Ha

 Scheduler-independent Baseclass for all multi-processor code generation 
 targets. 

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGStar.h"
#include "MultiTarget.h"
#include "SDFScheduler.h"
#include "Error.h"
#include "Profile.h"
#include "CGDDFCode.h"
#include <std.h>

// constructor
MultiTarget::MultiTarget(const char* name, const char* starclass,
			 const char* desc, const char* assocDomain) :
CGTarget(name,starclass,desc,assocDomain), nChildrenAlloc(0),
reorder(0), ddfcode(0)
{
	// loop scheduler makes no sense for a multitarget
	loopingLevel.setAttributes(A_NONSETTABLE);
        addState(nprocs.setState("nprocs",this,"2","number of processors"));
        addState(sendTime.setState("sendTime",this,"1",
                                   "time to send one datum"));
        addState(oneStarOneProc.setState("oneStarOneProc",this,"NO",
	   "If yes, place all invocations of a star into the same processor"));
        addState(manualAssignment.setState("manualAssignment",this,"NO",
	   "If yes, specify processor assignment of stars manually"));
        addState(adjustSchedule.setState("adjustSchedule",this,"NO",
	   "If yes, overide the previously obtained schedule by manual assignment"));
	displayFlag.setAttributes(A_SETTABLE);
	compileFlag.setAttributes(A_SETTABLE);
	runFlag.setAttributes(A_SETTABLE);
	installDDF();
}

MultiTarget::~MultiTarget() {
	LOG_DEL; delete ddfcode;
}
 
void MultiTarget :: installDDF() {
    LOG_DEL; delete ddfcode;
    LOG_NEW; ddfcode = new CGDDFCode(this);
}
 
// am I a heterogeneous target?
int MultiTarget :: isHetero() { return FALSE; }
 
// virtual methods
void MultiTarget :: resetResources() {}
State* MultiTarget :: galParam(Galaxy* g, const char* name) {
	return g->stateWithName(name);
}

// Based on priorities of the parameters, we may need to adjust the
// value of the parameters of low priority.
void MultiTarget :: initState() {
	Block :: initState();
	if (int(adjustSchedule)) manualAssignment.setInitValue("YES");
	if (int(manualAssignment)) oneStarOneProc.setInitValue("YES");
}
	
// associate send and receive stars.  If output of receive star
// is ANYTYPE, sets the send to be of
void MultiTarget :: pairSendReceive(DataFlowStar* send, DataFlowStar* receive) {
	PortHole* input = send->portWithName("input");
	if (!input) {
		Error::abortRun(*this,send->name(),": Send star does not have a porthole name 'input'");
		return;
	}
	PortHole* output = receive->portWithName("output");
	if (!output) {
		Error::abortRun(*this,receive->name(),": Receive star does not have a porthole named 'output'");
		return;
	}	
	if (output->type() == ANYTYPE) output->inheritTypeFrom(*input);
}

AsynchCommPair MultiTarget::createPeekPokeProcId(int peekProcId,
					     int pokeProcId) {
    AsynchCommPair pair;
    CGTarget* peekChild = cgChild(peekProcId);
    CGTarget* pokeChild = cgChild(pokeProcId);
    pair = peekChild->createPeekPoke(*peekChild,*pokeChild);
    if (pair.peek != NULL && pair.poke != NULL) return pair;
    pair = pokeChild->createPeekPoke(*peekChild,*pokeChild);
    if (pair.peek != NULL && pair.poke != NULL) return pair;

    StringList message;
    message << "this target does not support peek/poke actors from "
	    << peekChild->name() << " to " << pokeChild->name();
    Error::abortRun(*this,message);
    return pair;
}

void MultiTarget :: prepareCodeGen() {}
int MultiTarget :: prepareSchedule() { return TRUE; }

void MultiTarget :: addProcessorCode(int, const char* s) { myCode += s; }

void MultiTarget :: saveCommPattern() {}
void MultiTarget :: restoreCommPattern() {}
void MultiTarget :: clearCommPattern() {}

// By default, assume zero communication time.
int MultiTarget :: scheduleComm(ParNode*,int when, int) 
	{ return when; }

ParNode* MultiTarget :: backComm(ParNode*) { return 0; }

IntArray* MultiTarget :: candidateProcs(ParProcessors*, DataFlowStar*) 
	{ return NULL; }

CGTarget* MultiTarget :: cgChild(int n) {
	Target* childTarget;
	if (!(childTarget = child(n))) return NULL;
	if (childTarget->isA("CGTarget")) return (CGTarget*) childTarget;
	else return NULL;
}

int MultiTarget :: reorderChildren(IntArray* a) {
	int x = reorder.size();
	if (!x) {
		reorder.create(nChildrenAlloc);
		restore.create(nChildrenAlloc);
		for (int i = 0; i < nChildrenAlloc; i++) {
			reorder[i] = i;
			restore[i] = i;
		}
	}
 
	// save old information
        int i;
	for (i = 0; i < x; i++) {
		restore[i] = reorder[i];
	}
	restore.truncate(x);
 
	// update information
	if (!a) {
		for (i = 0; i < nChildrenAlloc; i++) {
			reorder[i] = i;
		}
	} else {
		int effSz = a->size();
		if (effSz > x) {
			Error::abortRun("Index too larget to reorder ",
			"child targets: reorderChildren error");
			return FALSE;
		}
		IntArray temp;
		temp.create(effSz);
		for (i = 0; i < effSz; i++) {
			temp[i] = reorder[a->elem(i)];
		}
		for (i = 0; i < effSz; i++) {
			reorder[i] = temp[i];
		}
		reorder.truncate(effSz);
	}
	return TRUE;
}
 
// restore the old ordering information
void MultiTarget :: restoreChildOrder() {
	int x = restore.size();
	for (int i = 0; i < x; i++) {
		reorder[i] = restore[i];
	}
	reorder.truncate(x);
}
 
// virtual function
Target* MultiTarget :: child(int n) {
    if ( n < 0 || n >= nProcs() || nChildrenAlloc == 0) return (Target*) NULL;
    int ix = reorder[n];
    return Target::child(ix);
}

ISA_FUNC(MultiTarget, CGTarget);
