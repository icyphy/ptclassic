#ifndef _UniProcessor_h
#define _UniProcessor_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
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

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#include "ParNode.h"
#include "DataStruct.h"
#include "DynamicGalaxy.h"
#include "MultiTarget.h"
#include "StringList.h"

class ostream;
class SDFScheduler;

////////////////////////
// class NodeSchedule //
////////////////////////

// This class shows the information of a scheduled node.
class NodeSchedule : public DoubleLink {
friend class UniProcessor;
public:
	// reset members
	void resetMembers() { nextFree = 0; idleFlag = 0; }

	// Constructor
	NodeSchedule() : DoubleLink(0), myNode(0) { resetMembers(); }

	// Set the members
	void setMembers(ParNode* n, int val) {
		if (n->getType() > 0) idleFlag = TRUE;
		myNode = n; duration = val; }

	// get the members
	int getDuration() { return duration; }
	ParNode* getNode() { return myNode; }

	// idle indication
	int isIdleTime() { return idleFlag; }

	// get next and previous link.
	NodeSchedule* nextLink() { return (NodeSchedule*) next; }
	NodeSchedule* previousLink() { return (NodeSchedule*) prev; }

private:
	int idleFlag;		// set if idle time
	int duration;
	ParNode* myNode;
	NodeSchedule* nextFree;	// free list management.
};

////////////////////////
// class UniProcessor //
////////////////////////

// This class simulates a single processor.

class UniProcessor : private DoubleLinkList {

friend class ParProcessors;
friend class ProcessorIter;
public:
	// constructor
	UniProcessor() : availTime(0), curSchedule(0), numFree(0), parent(0),
			 freeNodeSched(0), subGal(0), targetPtr(0) {
		specialStars.initialize();
	}
	~UniProcessor();

	// return the galaxy
	Galaxy* myGalaxy() { return subGal; }

	// set the target pointer
	void setTarget(MultiTarget* t, ParProcessors* p) 
		{ mtarget = t; parent = p; }

	// return my target
	CGTarget* target() { return targetPtr; }

	// create the galaxy
	void createSubGal();

	// return the size of the node (export baseclass fn)
	DoubleLinkList::size;

	int myId() { return index; }

	// schedule a Communication node
	void scheduleCommNode(ParNode*, int start);

	// add node to the processor
	void addNode(ParNode* node, int start);

	// schedule a node at the end
	// return the completion time.
	int schedAtEnd(ParNode*, int, int);
	
	// schedule in the middle (during an idle slot)
	// return the completion time if done, "-1" otherwise.
	int schedInMiddle(ParNode*, int, int);

	// check whether the node can be filled in an idle slot.
	// return -1 if failed. Otherwise, return the schedule time.
	// "limit", if given, indicates the upper bound the node
	// should be scheduled.
	int filledInIdleSlot(ParNode*, int start, int limit = 0);

	// append a Node to the end of the schedule
	void appendNode(ParNode* n, int val);

  	// display the schedule
	StringList display(int makespan);
	StringList displaySubUniv() { return subGal->print(1); }

	// write Gantt chart
	int writeGantt(ostream&);

	// get the total idle time including the idle time at the end.
	// This method should be called after "display" method.
	int getSumIdle() { return sumIdle; }

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

	// prepare code-generation: galaxy initialization,
	// copy schedule and simulate the schedule.
	void prepareCodeGen();

	// generate code
	StringList& generateCode(); 

	// generate code to the given target
	int genCodeTo(Target* t);

protected:
	// let derived classes remove links
	void removeLink(NodeSchedule* x) {
		DoubleLinkList::removeLink(x);
	}

	// The time when the processor available
	int availTime;

	// The schedule of the currently executed node.
	NodeSchedule* curSchedule;

	// target pointer
	MultiTarget* mtarget;	// multi-target
	CGTarget* targetPtr;		// my target processor

	// sum of idle time
	int sumIdle;

	NodeSchedule* getFree();	// get a free NodeSchedule
	void putFree(NodeSchedule*);	// put into the pool of free NodeSched.
	void clearFree();		// remove all created NodeSchedules.
	
	// sub-universe creation
	void makeConnection(ParNode* dN, ParNode* sN, PortHole* ref, ParNode*);
	void makeBoundary(ParNode* sN, PortHole* ref);

	// Simulate the schedule: obtain the buffer requirement.
	void simRunSchedule();

private:
	// Memory management for NodeSchedules: list of free NodeSchedules.
	NodeSchedule* freeNodeSched;
	int numFree;		// debugging purpose.

	// galaxy of blocks assigned to this processor
	DynamicGalaxy* subGal;

	// list of special stars (CGWormStar)  to be deleted.
	BlockList specialStars;

	// list of portholes connected to the wormholes.
	PortList wormPartners;

	// Multiprocessor object of which I am a part of
	ParProcessors* parent;

	// my id
	int index;

	// clone a star
	DataFlowStar* cloneStar(ParNode*);

	// create special stars and connect them
	DataFlowStar* makeSpread(PortHole* srcP, ParNode* sN, int);
	DataFlowStar* makeCollect(PortHole* destP, ParNode* dN, int);
	void makeReceive(int pindex, PortHole* rP,
		int delay, EGGate*, PortHole* orgP = 0);
	void makeSend(int pindex, PortHole* sP, EGGate*, PortHole* orgP = 0);

	// Depending on OSOPReq(), make connections
	void makeOSOPConnect(PortHole* p, DataFlowStar* org, DataFlowStar* far,
			     SequentialList&);
	void makeGenConnect(PortHole* p, ParNode*, DataFlowStar* org,
			    DataFlowStar* far, SequentialList&);

        // Check whether the user want to assign all invocations of a star
        // into the same processor or not.
        int OSOPreq() { return mtarget->getOSOPreq(); }

	// Convert a processor schedule to an SDF schedule for child target.
	void convertSchedule(SDFScheduler* optS = 0);

	// replace wormstars with wormholes before preparing code gen.
	// and restrore the original wormhole connection after code gen.
	void replaceWorms();
	void restoreWorms();
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
	NodeSchedule* operator++(POSTFIX_OP) { return next(); }

	ParNode* nextNode() { NodeSchedule* ns = next();
			      return ns? ns->getNode(): 0; }

	// Reset the iter to the beginning of the schedule.
	void reset() { DoubleLinkIter::reset();}
};

#endif
