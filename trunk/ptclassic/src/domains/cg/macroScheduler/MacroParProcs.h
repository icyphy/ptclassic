#ifndef _MacroParProcs_h
#define _MacroParProcs_h

#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#include "DLParProcs.h"
#include "MacroGraph.h"

class CGMacroClusterBag;

/////////////////////////
// class MacroParProcs //
/////////////////////////

// A class for managing the parallel processor schedules.

class MacroParProcs : public DLParProcs {

public:
	MacroParProcs(int n, MultiTarget* t): DLParProcs(n,t) {}
	~MacroParProcs() {}

//////////  main scheduling routine.

	// redefine the main scheduling routine for sequential task.
	/* virtual */ void scheduleSmall(MacroNode*);		// atomic node
	void scheduleBig(MacroNode*, int, IntArray&); // large node

	// in case of a macro node, we schedule the same profile to
	// the same processor set for all invocations.
	// This routine is called for invocationNumber of node is 
	// greater than 1.
	void copyBigSchedule(MacroNode*, IntArray&); 

  	// observe the pattern of processor availability before scheduling
  	// a dynamic construct (or non-atomic node).
	// First, schedule the communication nodes.
	// Second, compute the pattern. Return the proposed schedule time.
  	virtual int determinePPA(MacroNode*, IntArray&);

	// candidateProc method for a macro actor
	// currently, does NOT check resource constraints.
	// return more than one empty processors
	IntArray* candidateProcsForMacro(CGMacroClusterBag* s);

protected:
	// schedule a node of a parallel star in the final list schedule
	/* virtual */ int scheduleParNode(ParNode*);

private:

//////////////////  schedule LARGE node  ///////////////////////

	// Among candidate processors, choose a processor that can execute
	// the node earliest.
	virtual int decideStartingProc(MacroNode*, int*);
};

#endif
