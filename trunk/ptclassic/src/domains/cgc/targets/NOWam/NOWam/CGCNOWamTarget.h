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
	unsinged long inetAddr;
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

	// identify machines
	int identifyMachines();

	// return the machine_id of the given target.
	int machineId(Target*);
};

#endif
