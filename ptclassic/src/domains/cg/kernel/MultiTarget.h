/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

*******************************************************************/

#ifndef _MultiTarget_h
#define  _MultiTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CGTarget.h"
#include "IntState.h"
#include "IntArray.h"
#include "CGDDFCode.h"

class Profile;
class ParNode;
class ParProcessors;
class DataFlowStar;
class CGStar;

class MultiTarget : public CGTarget {

public:
        MultiTarget(const char* name, const char* starclass, const char* desc);
	~MultiTarget();

	// type identification
	/*virtual*/ int isA(const char*) const;

        // return TRUE if it is a heterogeneous target
        virtual int isHetero();

	// reset resources
	virtual void resetResources();

	// Resolve the parameter conflicts based on priorities
	void initState();

	// create Send and Receive Star.  "from" and "to" are the processor
	// ID numbers.  "num" is the number of tokens moved.
	virtual DataFlowStar* createReceive(int from, int to, int num) = 0;
	virtual DataFlowStar* createSend(int from, int to, int num) = 0;

	// create Spread/Collect star
	virtual DataFlowStar* createSpread() = 0;
	virtual DataFlowStar* createCollect() = 0;

	// create Macro actor which models a parallel task in the
	// hierarchical code generation
	virtual CGStar* createMacro(CGStar*, int, int, int) = 0;

	// pairSendReceive causes the send and receive stars to be associated
	// with each other, and should be called after they are created
	// with the above methods.  The default implementation does nothing.
	virtual void pairSendReceive(DataFlowStar* s, DataFlowStar* r);

        // create a peek/poke communication pair.  This is described in
        // detail in:
        // J.L. Pino, T.M. Parks and E.A. Lee, "Mapping Multiple Independent
        // Synchronous Dataflow Graphs onto Heterogeneous Multiprocessors," 
        // Proc. of IEEE Asilomar Conf. on Signals, Systems, and Computers, 
        // Pacific Grove, CA, Oct. 31 - Nov. 2, 1994.
	virtual int createPeekPoke(
	    PortHole& peekPort, PortHole& pokePort,
	    CGStar*& peek, CGStar*& poke);

	// get the OSOP requirement flag : all invocations of a star
	// should be assigned to the same processor
	int getOSOPreq() { return int(oneStarOneProc); }
	void setOSOPreq(int i) { oneStarOneProc = i; }

	// get the manualAssignment parameter
	int assignManually() { return int(manualAssignment); }

	// get the adjustSchedule parameter
	int overrideSchedule() { return int(adjustSchedule); }

        // return the nth child Target, null if no children or if child is
	// not a CGTarget.
        CGTarget* cgChild(int n);
	/* virtual */ Target* child(int);

	// return the galaxy parameter of the given name
	// Idea: use of parameter file
	virtual State* galParam(Galaxy* g, const char* name);

	// Add processor code to the multiprocessor target
	virtual void addProcessorCode(int pid, const char*);

	// return the array of candidate processors.
	virtual IntArray* candidateProcs(ParProcessors*, DataFlowStar*);

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

	// prepare scheduling: setting up information 
	// after galaxy is initialized and before 
	// scheduling starts.
	virtual int prepareSchedule();

	// reorder the child targets - and restore the old information
	virtual int reorderChildren(IntArray* a);
	virtual void restoreChildOrder();
 
	// to generate code for dynamic constructs
	virtual void installDDF();
	CGDDFCode* ddf() { return ddfcode; }

protected:
	IntState nprocs;		// number of processors
	IntState sendTime;		// Communication to send a unit data.
					// To be more elaborated.

	// direct the scheduling procedure
	IntState manualAssignment;	// assign stars manually
	IntState adjustSchedule;	// After a scheduling is performed,
					// the use may want to adjust the
					// schedule manually.
	IntState oneStarOneProc;	// all invocations of a star should
					// be assigned to a single PE.

	int nChildrenAlloc;
        IntArray reorder;
        IntArray restore;
 
        CGDDFCode* ddfcode;
};
#endif
