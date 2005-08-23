#ifndef _MacroParProcs_h
#define _MacroParProcs_h

#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
@(#)MacroParProcs.h	1.2	1/1/96

Copyright (c) 1995 Seoul National University
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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
