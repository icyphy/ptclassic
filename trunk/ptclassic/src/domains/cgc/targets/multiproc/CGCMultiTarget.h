#ifndef _CGCMultiTarget_h
#define  _CGCMultiTarget_h 1

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
	const char* inetAddr;	// internet address
	const char* nm;		// machine name
public:
	MachineInfo(): inetAddr(0), nm(0) {}
};

class CGCMultiTarget : public CGSharedBus {
public:
	CGCMultiTarget(const char* name, const char* starclass, const char* desc);
	~CGCMultiTarget();

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

	// get MachineInfo
	MachineInfo* getMachineInfo() { return machineInfo; }
	int* getPortNumber() { return currentPort; }

protected:
	void setup();

	// redefine 
	Target* createChild();

	// redefine
	int sendWormData(PortHole&);
	int receiveWormData(PortHole&);

	// C++ "hiding rule" requires these.  Boo, hiss!
	int sendWormData() { return CGSharedBus::sendWormData();}
	int receiveWormData() { return CGSharedBus::receiveWormData();}

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
	int* currentPort;

	// information on the machines
	MachineInfo* machineInfo;

	// identify machines
	int identifyMachines();

	// return the machine_id of the given target.
	int machineId(Target*);
};

#endif
