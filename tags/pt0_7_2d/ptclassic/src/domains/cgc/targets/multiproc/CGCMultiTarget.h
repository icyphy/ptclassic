#ifndef _CGCMultiTarget_h
#define  _CGCMultiTarget_h 1

/******************************************************************
Version identification:
@(#)CGCMultiTarget.h	1.14	04/07/97

@Copyright (c) 1992-1997 The Regents of the University of California.
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
#include "CGCDDFCode.h"

class EventHorizon;
class CGCTarget;

class MachineInfo {
friend class CGCMultiTarget;
	const char* inetAddr;	// internet address
	const char* nm;		// machine name
	Target* t;
public:
	MachineInfo(): inetAddr(0), nm(0), t(0) {}
};

class CGCMultiTarget : public CGSharedBus {
public:
	CGCMultiTarget(const char* name, const char* starclass, const char* desc);
	~CGCMultiTarget();

	Block* makeNew() const;
	int isA(const char*) const;

	// redefine IPC funcs
	DataFlowStar* createSend(int from, int to, int num);
	DataFlowStar* createReceive(int from, int to, int num);

	// spread and collect
	DataFlowStar* createSpread();
	DataFlowStar* createCollect();

	// macro star
	CGStar* createMacro(CGStar*, int, int, int);

	// redefine
	void pairSendReceive(DataFlowStar* s, DataFlowStar* r);

	// get MachineInfo
	MachineInfo* getMachineInfo() { return machineInfo; }
	
	// CGDDF support.
	// In case of code replication into the different set of target,
	// we need to modify the target dependent code especially for
	// communication stars: UnixSend and UnixReceive
	void prepCode(Profile*, int, int);

	// signal TRUE when replication begins, or FALSE when ends
	void signalCopy(int flag) { replicateFlag = flag; }

	// redefine
	/* virtual */ void installDDF();
	/* virtual */ void prepareChildren();

protected:
	void setup();

	// redefine
	int sendWormData(PortHole&);
	int receiveWormData(PortHole&);

private:
	// states indicate which machines to use.
	StringState machineNames;
	StringState nameSuffix;

	// Temporary hack!
	// Starting port_number. Port_number will be increased by one
	// for each pair of send/receive stars
	IntState portNumber;
	int currentPort;

	// In case, the cody body is replicated as in "For" and "Recur"
	// construct, save this information to be used in getMachineAddr().
	IntArray* mapArray;
	int baseNum;
	int replicateFlag;

	// information on the machines
	MachineInfo* machineInfo;

	// identify machines
	int identifyMachines();

	// return the machine_id of the given target.
	int machineId(Target*);

	// set up socket connection
	void setupSocketConnection(CGCTarget* t, int i);
};

#endif
