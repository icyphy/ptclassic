/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha

 Base target for multiprocessors (fully-connected).

*******************************************************************/

#ifndef _CGMultiTarget_h
#define _CGMultiTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "MultiTarget.h"
#include "IntState.h"
#include "StringState.h"
#include "Profile.h"
#include "ParProcessors.h"
#include "ParNode.h"

class ParScheduler;

class CGMultiTarget : public MultiTarget {
public:
	CGMultiTarget(const char* name,const char* sClass,const char* desc);
	~CGMultiTarget();
	void setup();
	int run();
	void wrapup();

	// redefine: generateCode routine generates code if inside a wormhole
	// in this redefinition.
	void generateCode();

	void setStopTime(double);

	// compute profile
	int computeProfile(int nP, int resWork = 0, IntArray* avail = 0);

	// set the profile
	void setProfile(Profile* p);

	// download the code with a specified processor
	void downLoadCode(int, Profile* prof = 0);

	// sort the processors with finish time.
	void insideSchedule();

	// total work load
	int totalWorkLoad();

	Block* makeNew() const;

	// communication cost
	int commTime(int,int,int,int);

	// return the array of candidate processors.
	IntArray* candidateProcs(ParProcessors*);

	// resource management
	int scheduleComm(ParNode* /*comm*/, int when, int /*limit*/ = 0) 
		{ return when; }

	// For a given communication node, find a comm. node scheduled
	// just before the argument node on the same communication resource.
	ParNode* backComm (ParNode* n);

	// redefine 
	DataFlowStar* createSend(int from, int to, int num);
	DataFlowStar* createReceive(int from, int to, int num);
	DataFlowStar* createSpread();
	DataFlowStar* createCollect();

	// generate Gantt chart
	void writeSchedule();

	// redefine. Methods for generating code for reading and writing
	// wormhole ports.
	// When nChildrenAlloc = 1, we call the corresponding
	// methods of the child target.
	void wormInputCode();
	void wormInputCode(PortHole&);
	void wormOutputCode();
	void wormOutputCode(PortHole&);

protected:
	StringState childType;
	StringState filePrefix;
	IntState useCluster;
	IntState overlapComm;
	IntState ignoreIPC;
	IntState ganttChart;
	StringState logFile;

	// prepare child targets: create and initialize.
	virtual void prepareChildren();

	// reset resources
	virtual void resetResources();

	// choose scheduler
	virtual void chooseScheduler();

	// create child targets
	// By default, it clones from KnownTarget list with "chile-type" state.
	virtual Target* createChild();
 
	// redefine the top-level iterations to do nothing
	void beginIteration(int repetitions, int depth);
	void endIteration(int repetitions, int depth);

	// Redefine for sending and receiving data to a target when run
	// inside of a wormhole.
	int sendWormData();
	int receiveWormData();

	// parallel processors
	ParProcessors* parProcs;

private:
	char oldChildType[40];
	IntArray canProcs;
};

#endif
