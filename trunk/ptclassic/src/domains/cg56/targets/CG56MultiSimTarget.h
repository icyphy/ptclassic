#ifndef _CG56MultiSimTarget_h
#define  _CG56MultiSimTarget_h 1

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

 Programmer: S. Ha, J. Pino

 This is a test multitarget class for CGCdomain.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGMultiTarget.h"
#include "IntState.h"
#include "StringState.h"
#include "ProcMemory.h"

class CG56MultiSimTarget : public CGMultiTarget {
public:
	CG56MultiSimTarget(const char* name, const char* starclass, const char* desc);

	void setup();
	void wrapup();
	Block* makeNew() const;
	int isA(const char*) const;

	// compile and run the code
	int compileCode();
	int runCode();

	// redefine IPC funcs
	DataFlowStar* createSend(int from, int to, int num);
	DataFlowStar* createReceive(int from, int to, int num);

	// redefine
	void addProcessorCode(int, const char* s);
	void pairSendReceive(DataFlowStar* s, DataFlowStar* r);

	// allocate the shared memory
	/* virtual */ void prepareCodeGen();

protected:
	// redefine 
	Target* createChild();

        // AsmTargets always support "AnyAsmStar" stars.
        const char* auxStarClass() const;


	// redefine
	int sendWormData(PortHole&);
	int receiveWormData(PortHole&);

	// C++ hiding rule requires that since we do the above,
	// we do the following as well (boo, hiss)
	int sendWormData() { return CGMultiTarget::sendWormData();}
	int receiveWormData() { return CGMultiTarget::receiveWormData();}

	// The following method downloads code for the inside of a wormhole
	// and starts it executing.
	int wormLoadCode();

	// shared memory
	ProcMemory* sharedMem;

private:
	// state to disallow compiling code.
	IntState doCompile;
	StringState sMemMap;
};

#endif
