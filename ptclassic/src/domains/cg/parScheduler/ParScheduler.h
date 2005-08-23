#ifndef _ParScheduler_h
#define _ParScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

/****************************************************************
Version identification:
@(#)ParScheduler.h	1.22	1/1/96

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

#include "pt_fstream.h"
#include "SDFScheduler.h"
#include "MultiTarget.h"
#include "ParProcessors.h"
#include "Profile.h"
#include "ParGraph.h"

class UniProcessor;
class Galaxy;
class ostream;

///////////////////////////
// class ParScheduler //
///////////////////////////

class ParScheduler : public SDFScheduler {
public:
  	// Constructor
  	ParScheduler(MultiTarget* t, const char* log = 0);

	virtual ~ParScheduler();

	// return my expanded graph
	virtual ParGraph* myEG() {return exGraph;}

	// redefine "compileRun"
        /*virutal*/ void compileRun();

        /*virutal*/ void setup();

	// prepare code generation for each processing element:
	// galaxy initialize, copy schedule, and simulate the schedule.
	virtual void prepareCodeGen();

	// reset the flag; to be called inside a wormhole.
	void ofWorm() { inUniv = FALSE; }
  
	// return the total work
	int getTotalWork() { return totalWork; }

        // prepare for Gantt chart display
        virtual void writeGantt(ostream& o);

	UniProcessor* getProc(int n) { return parProcs->getProc(n); }
	ParProcessors* myProcs() { return parProcs; }

	// create subGals for each processor
	int createSubGals(Galaxy&);

	// set up processors
	virtual ParProcessors* setUpProcs(int num);

	// map targets of processors
	void mapTargets() { parProcs->mapTargets(); }

        // main body of the schedule. 
	int mainSchedule();

	virtual int scheduleManually();  // manual assignment.
        virtual int scheduleIt();		  // automatic assignment.
				// Should be redefined in the derived class.

        // Return the total number of nodes in the precedence directed acyclic
        // graph (DAG)
        virtual int dagNodes() const;

	// finalize the schedule: do nothing in this base class.
	virtual int finalSchedule();

	// set the scheduled result into a designated profile
	void setProfile(Profile* profile);

protected:
	const char* logFile;
	pt_ofstream logstrm_real;	// for logging.
	ostream *logstrm;

	MultiTarget* mtarget;

	int numProcs;

	// expanded graph
	// Pointer should be set up in the constructor of the derived class.
	ParGraph* exGraph;

	// parallel scheduler -- 
	// Pointer should be set up in the "setUpProcs()" method of the derived
	// class.
	ParProcessors* parProcs;

	// (expected) total amount of scheduled work.
	int totalWork;

	// pattern of processor availability
	IntArray avail;

	// Flag to be set only when it is the scheduler of a universe.
	// If it is a scheduler of a wormhole, reseted.
	int inUniv;

	// Obtain the scheduling option from the target
	// OSOP requirement: all invocations to the same processor
	// manualAssignment:
	// adjustSchedule:
	int OSOPreq() { return mtarget->getOSOPreq(); }
	int assignManually() { return mtarget->assignManually(); }
	int overrideSchedule() { return mtarget->overrideSchedule(); }

	// use a modified critical path algorithm to schedule the graphs
	// with given number of processors.
	int computeSchedule(Galaxy& g);

	// check whether the galaxy contains a data-parallel star or not.
	int withParallelStar();

private:
	// set the procId of stars after scheduling with OSOP option
	// is executed. Prepare for adjustment.
	void saveProcIds();

	// set the flag when the expanded graph is created.
	int EGcreated;
};

#endif
