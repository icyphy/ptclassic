#ifndef _ParScheduler_h
#define _ParScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

/****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                        All Rights Reserved.

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

	// redefine "compileRun"
	void compileRun();

	// reset the flag; to be called inside a wormhole.
	void ofWorm() { inUniv = FALSE; }
  
	// return the total work
	int getTotalWork() { return totalWork; }

        // prepare for Gantt chart display
        void writeGantt(ostream& o);

	UniProcessor* getProc(int n) { return parProcs->getProc(n); }
	ParProcessors* myProcs() { return parProcs; }

	// create subGals for each processor
	virtual int createSubGals();

	// set up processors
	virtual void setUpProcs(int num);

        // main body of the schedule. 
	int mainSchedule();

	virtual int scheduleManually();  // manual assignment.
        virtual int scheduleIt();		  // automatic assignment.
				// Should be redefined in the derived class.

////////// Methods for wormholes ////////////////

	// finialize the schedule of wormholes.
	void finalSchedule();

	// set the scheduled result into a designated profile
	void setProfile(Profile* profile);

	// display schedule with "numProcs" processors.
	// It displays wormhole schedules.
	// Global schedule should be added in the derived class.
	// Look at the DLSchedule for example.
	StringList displaySchedule();

	// sort Processors with finish time.
	void sortProcessors() { parProcs->sortWithFinishTime(); }

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

	// old version of runOnce();
	void oldRun();

	// use a modified critical path algorithm to schedule the graphs
	// with given number of processors.
	int computeSchedule(Galaxy& g);

	// virtual methods: prepare scheduling. By default, do nothing
	virtual int preSchedule();

private:
	// set the procId of stars after scheduling with OSOP option
	// is executed. Prepare for adjustment.
	void saveProcIds();
	
	// temporary hack to get around the CG-DDF limitation.
	int oldRoutine;
};

#endif
