/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha

 Scheduler indepedent Baseclass for all multi-processor code generation
 targets.
 Virtual methods may be provided for proper wormhole interface.
 If there is no wormhole interface stuff, inherited CGTarget class alone
 should be enough as the scheduler-indepedent Baseclass.

*******************************************************************/

#ifndef _BaseMultiTarget_h
#define  _BaseMultiTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CGTarget.h"
#include "IntState.h"
#include "IntArray.h"

class Profile;
class ParNode;
class ParProcessors;
class SDFStar;

class BaseMultiTarget : public CGTarget {

public:
        BaseMultiTarget(const char* name, const char* starclass, const char* desc);

	// Resolve the parameter conflicts based on priorities
	void initState();

	int getIters() { return iters; }

	// create Send and Receive Star
	virtual SDFStar* createReceive(int from, int to, int num);
	virtual SDFStar* createSend(int from, int to, int num);
	// pair them
	virtual void pairSendReceive(SDFStar* s, SDFStar* r);

	virtual SDFStar* createSpread();
	virtual SDFStar* createCollect();

	// set current child.
	void setCurChild(int i) { curChild = i; }

	// get current child index.
	int getCurChild() { return curChild; }

        // Inform how many processors are available.
        void setTargets(int);

	// Return true, if it inherits the child targets from the parent.
	int inherited() { return int(inheritProcessors); }

	// get the OSOP requirement flag : all invocations of a star
	// should be assigned to the same processor
	int getOSOPreq() { return int(oneStarOneProc); }

	// get the manualAssignment parameter
	int assignManually() { return int(manualAssignment); }

	// get the adjustSchedule parameter
	int overrideSchedule() { return int(adjustSchedule); }

        // Inherit the child targets from a given BaseMultiTarget.
        // If the number of child targets is greater than that of a given
        // target, return FALSE. Otherwise, return TRUE.
        int inheritChildTargets(Target*);

        // For CGWormhole, Profile class provides a common structure
        // for domain (or target) interface of scheduling results.
        // This method indicates where to put the scheduling result
        // of current Target in.
        virtual void setProfile(Profile*);

        // For wormhole interface, this method calculates the total Workload
        // under current Target.
        virtual int totalWorkLoad();

        // Compute the profile of the system under the current Target.
        // It returns how many processors are actually used.
        // pNum - the number of available processors.
        // resWork - work to be done under the outside Target. (for wormhole).
        // avail - pattern of processor availability.
        virtual int computeProfile(int, int, IntArray*);

        // Do scheduling job for inside galaxy -- to be called by Wormhole
	// class.
        virtual void insideSchedule();

        // Download the code for the specified processor.
        // Argument specifies the profile under the current target.
        virtual void downLoadCode(int, Profile*);

	// Add processor code to the multiprocessor target
	virtual void addProcessorCode(int pid, const char*);

	// return the array of candidate processors.
	virtual IntArray* candidateProcs(ParProcessors*);

	// resource management
	virtual void saveCommPattern();
	virtual void restoreCommPattern();
	virtual void clearCommPattern();

	// schedule communication link.
	virtual int scheduleComm(ParNode*, int, int limit = 0);

	// For a given communication node, find a comm. node scheduled
	// just before the argument node on the same communication resource.
	virtual ParNode* backComm(ParNode*);

	// prepare code generation of child targets
	// by default, do nothing.
	virtual void prepareCodeGen();

protected:
	IntState nprocs;		// number of processors
	IntState inheritProcessors;	// Inside the wormhole, use the
					// same processors as the outside.
	IntState sendTime;		// Communication to send a unit data.
					// To be more elaborated.

	// direct the scheduling procedure
	IntState manualAssignment;	// assign stars manually
	IntState adjustSchedule;	// After a scheduling is performed,
					// the use may want to adjust the
					// schedule manually.
	IntState oneStarOneProc;	// all invocations of a star should
					// be assigned to a single PE.

	int curChild;
	int nChildrenAlloc;
private:
	int iters;
};
#endif
