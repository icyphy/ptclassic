/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha

 Base target for multiprocessors (fully-connected).

*******************************************************************/

#ifndef _CGFullConnect_h
#define _CGFullConnect_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "BaseMultiTarget.h"
#include "IntState.h"
#include "StringState.h"
#include "Profile.h"
#include "ParProcessors.h"
#include "ParNode.h"

class ParScheduler;

class CGFullConnect : public BaseMultiTarget {
public:
	CGFullConnect(const char* name,const char* sClass,const char* desc);
	~CGFullConnect();
	void start();
	int setup(Galaxy&);
	void wrapup();

	void setStopTime(float);

	void addCode(const char*);

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

	Block* clone() const;

	// communication cost
	int commTime(int,int,int,int);

	// return the array of candidate processors.
	IntArray* candidateProcs(ParProcessors*);

	// resource management
	int scheduleComm(ParNode* comm, int when, int limit = 0) 
		{ return when; }

	// For a given communication node, find a comm. node scheduled
	// just before the argument node on the same communication resource.
	ParNode* backComm (ParNode* n);

	// redefine 
	SDFStar* createSend(int from, int to, int num);
	SDFStar* createReceive(int from, int to, int num);
	SDFStar* createSpread();
	SDFStar* createCollect();

	// display Gantt chart
	void displaySchedule(ParScheduler* );

protected:
	StringState childType;
	StringState filePrefix;
	IntState useCluster;
	IntState overlapComm;
	IntState ignoreIPC;
	IntState ganttChart;
	StringState logFile;

	// reset resources
	virtual void resetResources();

	// choose scheduler
	virtual void chooseScheduler();

	// create child targets
	// By default, it clones from KnownTarget list with "chile-type" state.
	virtual Target* createChild();
 
	// parallel processors
	ParProcessors* parProcs;

private:
	char oldChildType[40];
	IntArray canProcs;
};

#endif
