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

class CGFullConnect : public BaseMultiTarget {
public:
	CGFullConnect(const char* name,const char* sClass,const char* desc);
	~CGFullConnect();
	void start();
	int setup(Galaxy&);
	void addCode(const char* code);
	void wrapup();

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
	int scheduleComm(ParNode* comm, int when) { return when; }

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

private:
	char oldChildType[40];
	IntArray canProcs;
};

#endif