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
#include "StringArrayState.h"
#include "IntArrayState.h"

class ParNode;
class ParScheduler;
class ParProcessors;
class BooleanMatrix;
class Profile;

class CGMultiTarget : public MultiTarget {
public:
	CGMultiTarget(const char* name,const char* sClass,const char* desc);
	~CGMultiTarget();
	int run();

	// If within a WormHole, do nothing.
	// Otherwise, conditionally display, compile, load, and run the code.
	/*virtual*/ void wrapup();

	/*virtual*/ void writeCode();

	// type identification
	/*virtual*/ int isA(const char*) const;

	// redefine: generateCode routine generates code if inside a wormhole
	// in this redefinition.  It also calls all of the child targets
	// generateCode methods
	void generateCode();

	void setStopTime(double);

	// compute profile
	int computeProfile(int nP, int resWork = 0, IntArray* avail = 0);

	// set the profile
	void setProfile(Profile* p);

	// download the code with a specified processor
	int downLoadCode(int, int, Profile* prof = 0);

	// total work load
	int totalWorkLoad();

	Block* makeNew() const;

	// communication cost
	int commTime(int,int,int,int);

	// return the array of candidate processors.
	IntArray* candidateProcs(ParProcessors*, DataFlowStar*);

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
	/*virtual*/ void allWormInputCode();
	/*virtual*/ void wormInputCode(PortHole&);
	/*virtual*/ void allWormOutputCode();
	/*virtual*/ void wormOutputCode(PortHole&);

	// redefine the execution time of a star
	/* virtual */ int execTime(DataFlowStar*, CGTarget*);

	// check whether communication amortization is possible.
	// If yes, return TRUE, else return FALSE.
	int amortize(int from, int to);

	// Methods used in stages of a run, these simply call the Child
	// targets methods.
	// Return FALSE on error.
	/*virtual*/ int compileCode();
	/*virtual*/ int loadCode();
	/*virtual*/ int runCode();

	// Determine whether we have (or will have, once createChild
	// operations are complete) a child of the appropriate type.
	/*virtual*/ int childIsA(const char*) const;
protected:
        // This flag keeps track if we have modified the galaxy.  We
        // Should only modify the galaxy ONCE per time the graph is 
        // compiled.  In this target we flatten wormholes.  If we
	// modify the galaxy after flattening the wormholes, we may
	// end up with stars from other domains in galaxies that the
	// user never intended.
        int modifiedGalaxy;

	void setup();

	// parameters to set the child targets
	StringArrayState childType;
	StringArrayState resources;
	IntArrayState relTimeScales;

	// parameters to set the scheduling option
	IntState useCluster;
	IntState overlapComm;
	IntState ignoreIPC;
	IntState useMultipleSchedulers;

	IntState ganttChart;
	StringState logFile;
	IntState amortizedComm;

	// prepare child targets: create and initialize.
	virtual void prepareChildren();

	// pass resource infomation to the child target.
	// and append the "resources" state of the child target.
	// If no "resources" in the child target, create one.
	virtual void resourceInfo();
	
	// reset resources
	virtual void resetResources();

	// choose scheduler
	virtual void chooseScheduler();

	// create child targets
	// By default, it clones from KnownTarget list with "chile-type" state.
	virtual Target* createChild(int);
 
	// flatten wormholes if heterogeneous targets
	void flattenWorm();

	// redefine the top-level iterations to do nothing
	void beginIteration(int repetitions, int depth);
	void endIteration(int repetitions, int depth);

	// Redefine for sending and receiving data to a target when run
	// inside of a wormhole.
	/* virtual */ int allSendWormData();
	/* virtual */ int allReceiveWormData();

	// update the reachability matrix for communication amortization.
	// This method is supposed to be called in createSend() method.
	void updateRM(int from, int to);

	// parallel processors
	IntArray canProcs;

	// Does the child target support the given star?
	virtual int childSupport(Target*,Star*);
private:
	// state list added dynamically
	StateList addedStates;

	// reachability matrix
	BooleanMatrix* rm;
};

#endif
