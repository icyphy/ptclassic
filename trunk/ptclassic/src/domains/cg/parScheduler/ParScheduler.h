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

#include "SDFScheduler.h"
#include "BaseMultiTarget.h"
#include "ParProcessors.h"
#include "Profile.h"
#include "ParGraph.h"

class UniProcessor;
class UserOutput;
class Galaxy;
class ostream;

///////////////////////////
// class ParScheduler //
///////////////////////////

class ParScheduler : public SDFScheduler {
private:
	// set the procId of stars after scheduling with OSOP option
	// is executed. Prepare for adjustment.
	void saveProcIds(Galaxy& g);
	
protected:
	const char* logFile;
	ostream* logstrm;	// for logging.

	BaseMultiTarget* mtarget;
	Galaxy* myGal;

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

	// redefine "run once"
	void runOnce();

	// use a modified critical path algorithm to schedule the graphs
	// with given number of processors.
	int computeSchedule(Galaxy&);

	// virtual methods: prepare scheduling. By default, do nothing
	virtual int preSchedule();

public:
  	// Constructor
  	ParScheduler(BaseMultiTarget* t, const char* log = 0) : mtarget(t),
		logFile(log), exGraph(0) { inUniv = TRUE; }

	virtual ~ParScheduler() {}

	// reset the flag; to be called inside a wormhole.
	void ofWorm() { inUniv = FALSE; }
  
	// return the total work
	int getTotalWork() { return totalWork; }

        // prepare for Gantt chart display
        void writeGantt(UserOutput& o);

	UniProcessor* getProc(int n) { return parProcs->getProc(n); }
	ParProcessors* myProcs() { return parProcs; }

	// create subGals for each processor
	virtual void createSubGals();

	// set up processors
	virtual void setUpProcs(int num);

        // main body of the schedule. 
	int mainSchedule(Galaxy& g);

	virtual int scheduleManually(Galaxy& g);  // manual assignment.
        virtual int scheduleIt();		  // automatic assignment.
				// Should be redefined in the derived class.

////////// Methods for wormholes ////////////////

	// finialize the schedule of wormholes.
	void finalSchedule(Galaxy&);

	// set the scheduled result into a designated profile
	void setProfile(Profile* profile);

	// display schedule with "numProcs" processors.
	// It displays wormhole schedules.
	// Global schedule should be added in the derived class.
	// Look at the DLSchedule for example.
	StringList displaySchedule();

	// sort Processors with finish time.
	void sortProcessors() { parProcs->sortWithFinishTime(); }
};

#endif
