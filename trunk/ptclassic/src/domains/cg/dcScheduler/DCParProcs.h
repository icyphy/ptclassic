#ifndef _DCParProcs_h
#define _DCParProcs_h 1
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
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

Programmer: Soonhoi Ha based on G.C. Sih's code.

These schedule classes are used by the parallel scheduler

*****************************************************************/

#include "DCUniProc.h"
#include "DCNode.h"
#include "ParProcessors.h"

class DCGraph;

			//////////////////////////
			///  class DCParProcs  ///
			//////////////////////////
// Schedule for multiple processors.
class DCParProcs : public ParProcessors {

public:
	// Constructor takes number of processors as an argument
	DCParProcs(int procs, MultiTarget* mtarget);
	~DCParProcs();

	// Returns a pointer to the proper UniProc
	DCUniProc *getSchedule(int num) { return &(schedules[num]); }
	UniProcessor* getProc(int num);

	// return the amount of the IPC.
	int commAmount() { return commCount; }

	// find a schedule limiting progression (SLP)
	// return TRUE, if succeeded.
	int findSLP(DCNodeList*);

	// save best schedule results for nodes
	void saveBestResult(DCGraph*);

	// After the best schedule is obtained, we make a final version
	// of expanded graph including comm. nodes
	void finalizeGalaxy(DCGraph*);

	// Categorizes each processor as being heavily or lightly loaded.
	// It sets an integer array: 1 for heavy and -1 for light processors.
	// Initial threshold is 50% of the maxload.
	// Considers all processors heavily loaded if all processors are loaded
	//      beyond 75% of the maximum load on any processor.
	// We regard at most one idle processor as lightly loaded. Leave other
	// idle processors untouched.
	void categorizeLoads(int* procs);

protected:
	ParNode* createCommNode(int i);

private:
	// schedules
	DCUniProc* schedules;
};

#endif

