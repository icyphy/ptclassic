/******************************************************************
Version identification:
$Id$
 
Copyright (c) 1995-%Q%  The Regents of the University of California.
All Rights Reserved.
 
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
 
 Programmer: Patrick Warner
 
*******************************************************************/

#ifndef _CGCNOWamTarget_h
#define  _CGCNOWamTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CGMultiTarget.h"
#include "StringState.h"
#include "IntArrayState.h"
#include "IntState.h"

class EventHorizon;
class CGCTarget;

class VirtualInfo {
friend class CGCNOWamTarget;
	unsigned long inetAddr; // internet address
	int virtNode;	// active message virtual node
	const char* nm;		// machine name
public:
	VirtualInfo(): virtNode(0), nm(0) {}
};

class CGCNOWamTarget : public CGMultiTarget {
public:
	CGCNOWamTarget(const char* name, const char* starclass, const char* desc);
	~CGCNOWamTarget();

	Block* makeNew() const;
	int isA(const char*) const;

	// redefine IPC funcs
	DataFlowStar* createSend(int from, int to, int num);
	DataFlowStar* createReceive(int from, int to, int num);

	// spread and collect
	DataFlowStar* createSpread();
	DataFlowStar* createCollect();

	// redefine
	void pairSendReceive(DataFlowStar* s, DataFlowStar* r);

	// get VirtualInfo
	VirtualInfo* getVirtualInfo() { return machineInfo; }
	void setMachineAddr(CGStar*, CGStar*);
	
	// signal TRUE when replication begins, or FALSE when ends
	void signalCopy(int flag) { replicateFlag = flag; }

protected:
	void setup();

	// redefine
	int sendWormData(PortHole&);
	int receiveWormData(PortHole&);

private:
	// states indicate which machines to use.
	StringState machineNames;
	StringState nameSuffix;

	// In case, the cody body is replicated as in "For" and "Recur"
	// construct, save this information to be used in getMachineAddr().
	IntArray* mapArray;
	int baseNum;
	int replicateFlag;

	// information on the machines
	VirtualInfo* machineInfo;

	// number of send/receive pairs
	int pairs;	

	// identify machines
	int identifyMachines();

	// return the machine_id of the given target.
	int machineId(Target*);
};

#endif
