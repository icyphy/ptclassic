#ifndef _UniProcessor_h
#define _UniProcessor_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#include "ParNode.h"
#include "DataStruct.h"
#include "UserOutput.h"
#include "DynamicGalaxy.h"
#include "BaseMultiTarget.h"

////////////////////////
// class NodeSchedule //
////////////////////////

// This class shows the information of a scheduled node.
class NodeSchedule : public DoubleLink {
friend class UniProcessor;
private:
	int idleFlag;		// set if idle time
	int duration;
	int index;		// index for dynamic constructs.
	ParNode* myNode;
	NodeSchedule* nextFree;	// free list management.
public:
	// Constructor
	NodeSchedule() : DoubleLink(0), myNode(0) { resetMembers(); }

	// Set the members
	void setMembers(ParNode* n, int val) {
		if (n->getType() > 0) idleFlag = TRUE;
		myNode = n; duration = val; }

	// reset members
	void resetMembers() { nextFree = 0; idleFlag = 0; index = -1; }

	// get the members
	int getDuration() { return duration; }
	ParNode* getNode() { return myNode; }

	// idle indication
	int isIdleTime() { return idleFlag; }

	// set and get index
	void setIndex(int i) { index = i; }
	int  getIndex()	     { return index; }

	// get next and previous link.
	NodeSchedule* nextLink() { return (NodeSchedule*) next; }
	NodeSchedule* previousLink() { return (NodeSchedule*) prev; }
};

////////////////////////
// class UniProcessor //
////////////////////////

// This class simulates a single processor.

class UniProcessor : public DoubleLinkList {

friend class ParProcessors;

private:
	// Memory management for NodeSchedules: list of free NodeSchedules.
	NodeSchedule* freeNodeSched;
	int numFree;		// debugging purpose.

	// galaxy of blocks assigned to this processor
	DynamicGalaxy* subGal;

	// my id
	int index;

	// create special stars and connect them
	SDFStar* makeSpread(PortHole* srcP, ParNode* sN);
	SDFStar* makeCollect(PortHole* destP, ParNode* dN);
	void makeReceive(int pindex, PortHole* rP,int delay,ParNode*,EGGate*);
	void makeSend(int pindex, PortHole* sP, ParNode*, EGGate*);

	// set the cloned star pointer of the Send/Receive node in OSOP option.
	void matchReceiveNode(SDFStar*, PortHole*, ParNode*);
	void matchSendNode(SDFStar*, PortHole*, ParNode*);

	// Depending on OSOPReq(), make connections
	void makeOSOPConnect(PortHole* p, SDFStar* org, SDFStar* far,
			     SequentialList&);
	void makeGenConnect(PortHole* p, ParNode*, SDFStar* org, SDFStar* far,
			     SequentialList&);

        // Check whether the user want to assign all invocations of a star
        // into the same processor or not.
        int OSOPreq() { return mtarget->getOSOPreq(); }

protected:
	// The time when the processor available
	int availTime;

	// The schedule of the currently executed node.
	NodeSchedule* curSchedule;

	// target pointer
	BaseMultiTarget* mtarget;

	// sum of idle time
	int sumIdle;

	NodeSchedule* getFree();	// get a free NodeSchedule
	void putFree(NodeSchedule*);	// put into the pool of free NodeSched.
	void clearFree();		// remove all created NodeSchedules.
	
	// sub-universe creation
	void makeConnection(ParNode* dN, ParNode* sN, PortHole* ref);
	void makeBoundary(ParNode* sN, PortHole* ref);

	// Simulate the schedule: obtain the buffer requirement.
	void simRunSchedule();

public:
	// constructor
	UniProcessor() : availTime(0), curSchedule(0), numFree(0),
			 freeNodeSched(0), subGal(0) {}
	~UniProcessor();

	// return the galaxy
	Galaxy* myGalaxy() { return subGal; }

	// set the target pointer
	void setTarget(BaseMultiTarget* t) { mtarget = t; }

	// create the galaxy
	void createSubGal();

	// return the size of the node
	int size() { return mySize(); }
	int myId() { return index; }

	// schedule a Communication node
	void scheduleCommNode(ParNode*, int start);

	// add node to the processor
	void addNode(ParNode* node, int start);

	// schedule a node at the end
	// return the completion time.
	int schedAtEnd(ParNode*, int, int);
	
	// schedule in the middle (during an idle slot)
	// return the completion time if done, "FALSE" otherwise.
	int schedInMiddle(ParNode*, int, int);

	// check whether the node can be filled in an idle slot.
	// return -1 if failed. Otherwise, return the schedule time.
	int filledInIdleSlot(ParNode*, int start, int limit = 0);

	// append a Node to the end of the schedule
	void appendNode(ParNode* n, int val);

  	// display the schedule
	StringList display(int makespan);
	StringList displaySubUniv() { return subGal->printRecursive(); }

	// write Gantt chart
	int writeGantt(UserOutput&);

	// down-load the code
	void run();
	
	// get the total idle time including the idle time at the end.
	// This method should be called after "display" method.
	int getSumIdle() { return sumIdle; }

	// set index of the current Node schedule
	void setIndex(int i)	{ curSchedule->setIndex(i); }

  	// set, get the availTime field.
  	void setAvailTime(int t) { availTime = t; }
  	int getAvailTime() { return availTime; }

	// return the start time of the schedule
	int getStartTime();

	// deallocate the links of this list.
	// reset the members.
	void initialize();

	// copy the schedule
	void copy(UniProcessor* org);

	// get NodeSchedule
	NodeSchedule* getNodeSchedule(ParNode* n);
	NodeSchedule* getCurSchedule() { return curSchedule; }
};

/////////////////////////
// class ProcessorIter //
/////////////////////////

// Iterator class for UniProcessor class.
class ProcessorIter : private DoubleLinkIter {
public:
	// Constructor with schedule argument.
	ProcessorIter(const UniProcessor& s) : DoubleLinkIter(s) {}

	// Get the next scheduled node in the processor
	NodeSchedule* next() 
		{ return (NodeSchedule*) DoubleLinkIter::nextLink(); }
	NodeSchedule* operator++() { return next(); }

	ParNode* nextNode() { NodeSchedule* ns = next();
			      return ns? ns->getNode(): 0; }

	// Reset the iter to the beginning of the schedule.
	void reset() { DoubleLinkIter::reset();}
};

#endif
