/******************************************************************
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

 Scheduler-independent Baseclass for all multi-processor code generation
 targets.

 Virtual methods may be provided for proper wormhole interface.
 If there is no wormhole interface stuff, inherited CGTarget class alone
 should be enough as the scheduler-indepedent Baseclass.

*******************************************************************/

#ifndef _MultiTarget_h
#define  _MultiTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CGTarget.h"
#include "IntState.h"
#include "IntArray.h"

class Profile;
class ParNode;
class ParProcessors;
class DataFlowStar;

class MultiTarget : public CGTarget {

public:
        MultiTarget(const char* name, const char* starclass, const char* desc);

	// Resolve the parameter conflicts based on priorities
	void initState();

	int getIters() { return iters; }

	// create Send and Receive Star.  "from" and "to" are the processor
	// ID numbers.  "num" is the number of tokens moved.
	virtual DataFlowStar* createReceive(int from, int to, int num) = 0;
	virtual DataFlowStar* createSend(int from, int to, int num) = 0;

	// create Spread/Collect star
	virtual DataFlowStar* createSpread() = 0;
	virtual DataFlowStar* createCollect() = 0;

	// pairSendReceive causes the send and receive stars to be associated
	// with each other, and should be called after they are created
	// with the above methods.  The default implementation does nothing.
	virtual void pairSendReceive(DataFlowStar* s, DataFlowStar* r);

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
	void setOSOPreq(int i) { oneStarOneProc = i; }

	// get the manualAssignment parameter
	int assignManually() { return int(manualAssignment); }

	// get the adjustSchedule parameter
	int overrideSchedule() { return int(adjustSchedule); }

        // Inherit the child targets from a given MultiTarget.
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
