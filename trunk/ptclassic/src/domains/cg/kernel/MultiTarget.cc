static const char file_id[] = "MultiTarget.cc";

/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer: Soonhoi Ha

 Scheduler-independent Baseclass for all multi-processor code generation 
 targets. Virtual methods are provided for uniform wormhole interface.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGStar.h"
#include "MultiTarget.h"
#include "SDFScheduler.h"
#include "Error.h"
#include "Profile.h"
#include <std.h>

// constructor
MultiTarget::MultiTarget(const char* name,const char* starclass,
		   const char* desc) : CGTarget(name,starclass,desc),
	nChildrenAlloc(0)
{
	// loop scheduler makes no sense for a multitarget
	loopingLevel.setAttributes(A_NONSETTABLE);
        addState(nprocs.setState("nprocs",this,"2","number of processors"));
        addState(inheritProcessors.setState(
	     "inheritProcessors",this,"NO","If yes, inherit child targets"));
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
}

// Based on priorities of the parameters, we may need to adjust the
// value of the parameters of low priority.
void MultiTarget :: initState() {
	Block :: initState();
	if (int(adjustSchedule)) manualAssignment = TRUE;
	if (int(manualAssignment)) oneStarOneProc = TRUE;
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

int MultiTarget :: createPeekPoke(PortHole& peekPort, PortHole& pokePort,
				   CGStar*& peek, CGStar*& poke) {
    if (peek == NULL || poke == NULL) {
	Error::abortRun(*this,"this target does not support peek/poke actors");
	return FALSE;
    }

    PortHole* input = poke->portWithName("input");
    if (!input) {
	Error::abortRun(*this,poke->name(),
			": Peek star does not have a porthole named 'input'");
	return FALSE;
    }	

    PortHole* output = peek->portWithName("output");
    if (!output) {
	Error::abortRun(*this,peek->name(),
			": Peek star does not have a porthole named 'output'");
	return FALSE;
    }	

    if (output->type() == ANYTYPE) input->inheritTypeFrom(*output);

    int numInitDelays;
    Galaxy* parentGal;
    StringList starName;
    // FIXME - we do not support initial delays on peek/poke arcs
    // The peekPort and pokePort are in a disconnected state here.
    // numInitDelays = peekPort.numInitDelays();
    numInitDelays = 0;
    peekPort.disconnect();
    output->connect(peekPort,numInitDelays);
    parentGal = (Galaxy*) peekPort.parent()->parent();
    starName.initialize();
    starName << symbol("Peek");
    if (peekPort.parent()->target()) 
	peek->setTarget(peekPort.parent()->target());
    parentGal->addBlock(*peek,hashstring(starName));

    numInitDelays = pokePort.numInitDelays();
    pokePort.disconnect();
    pokePort.connect(*input,numInitDelays);
    parentGal = (Galaxy*) pokePort.parent()->parent();
    starName.initialize();
    starName << symbol("Poke");
    if (pokePort.parent()->target()) 
	poke->setTarget(pokePort.parent()->target());
    parentGal->addBlock(*poke,hashstring(starName));

    return TRUE;
}

void MultiTarget :: setProfile(Profile*) {}
void MultiTarget :: prepareCodeGen() {}
int MultiTarget :: prepareSchedule() { return TRUE; }

int MultiTarget :: totalWorkLoad() { return -1; }		// undefined...

int MultiTarget :: computeProfile(int pNum, int, IntArray*) {
	// do nothing in this base class.
	return pNum;
}

int MultiTarget :: insideSchedule(Profile*) { return TRUE; }
Profile* MultiTarget :: manualSchedule(int) { return 0; }

int MultiTarget :: downLoadCode(int, int, Profile*) { return TRUE; }

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

void MultiTarget :: setTargets(int n) { 
	nprocs = n; 
	char temp[6];
	sprintf(temp, "%d", n);
	const char* t = hashstring(temp);
	nprocs.setInitValue(t); 
	nprocs.initialize(); }

int MultiTarget :: inheritChildTargets(Target* father) {

        if (int(nprocs) > father->nProcs()) {
                Error::abortRun(name(), " target has more children than \
its parent target: ", father->name());
                return FALSE;
        }

        nChildrenAlloc = int(nprocs);
	if (father->child(0)) {
		inheritChildren(father,0,nChildrenAlloc-1);
	} else {
		const char* saveNm = father->name();
		addChild(*father);
		father->setNameParent(saveNm, 0);
	}
	return TRUE;
}

CGTarget* MultiTarget :: cgChild(int n) {
	Target* childTarget;
	if (!(childTarget = child(n))) return NULL;
	if (childTarget->isA("CGTarget")) return (CGTarget*) childTarget;
	else return NULL;
}

ISA_FUNC(MultiTarget, CGTarget);
