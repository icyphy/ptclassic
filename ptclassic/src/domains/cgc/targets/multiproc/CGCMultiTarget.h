#ifndef _CGCMultiTarget_h
#define  _CGCMultiTarget_h 1

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

#include "CGSharedBus.h"
#include "StringState.h"
#include "IntArrayState.h"
#include "IntState.h"

class EventHorizon;
class CGCTarget;

class MachineInfo {
friend class CGCMultiTarget;
	const char* nm;	// machine name
	const char* inetAddr;	// internet address
	int localFlag;	// if it is the localhost.
public:
	MachineInfo(): nm(0), inetAddr(0), localFlag(0) {}
};

class CGCMultiTarget : public CGSharedBus {
public:
	CGCMultiTarget(const char* name, const char* starclass, const char* desc);
	~CGCMultiTarget();

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

	// spread and collect
	DataFlowStar* createSpread();
	DataFlowStar* createCollect();

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

private:
	// state to disallow compiling code.
	IntState doCompile;

	// states indicate which machines to use.
	StringState machineNames;
	StringState nameSuffix;

	// Temporary hack!
	// Starting port_number. Port_number will be increased by one
	// for each pair of send/receive stars
	IntState portNumber;
	int currentPort;

	// information on the machines
	MachineInfo* machineInfo;

	// identify machines
	int identifyMachines();

	// return the machine_id of the given target.
	int machineId(Target*);

	// generate error message
	void reportError(int, const char*, const char*);
};

#endif
