#ifndef _FictionTarget_h
#define  _FictionTarget_h 1

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: S. Ha

 This is a test multitarget class for CGCdomain.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGMultiTarget.h"
#include "IntState.h"
#include "StringState.h"
#include "ProcMemory.h"

class FictionTarget : public CGMultiTarget {
public:
	FictionTarget(const char* name, const char* starclass, const char* desc);

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

protected:
	// redefine 
	Target* createChild();

	// redefine
	int sendWormData(PortHole&);
	int receiveWormData(PortHole&);

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
