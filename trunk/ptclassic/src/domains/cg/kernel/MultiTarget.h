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
class DLNode;
class ParProcessors;

class BaseMultiTarget : public CGTarget {

public:
        BaseMultiTarget(const char* name, const char* starclass, const char* desc);

	// set current child.
	void setCurChild(int i) { curChild = i; }

	// get current child index.
	int getCurChild() { return curChild; }

        // Inform how many processors are available.
        void setTargets(int);

	// Return true, if it inherits the child targets from the parent.
	int inherited() { return int(inheritProcessors); }

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

	// return the array of candidate processors.
	virtual IntArray* candidateProcs(DLNode*, ParProcessors*);

	// resource management
	virtual void saveCommPattern();
	virtual void restoreCommPattern();
	virtual void clearCommPattern();

	// reserve resource
	// arguments (1) src (2) dest (3) time (4) # tokens.
	virtual int reserveComm(int, int, int, int);
	virtual int scheduleComm(DLNode*, int, int);

protected:
	IntState nprocs;
	IntState inheritProcessors;
	IntState sendTime;
	int curChild;
	int nChildrenAlloc;
};
#endif
