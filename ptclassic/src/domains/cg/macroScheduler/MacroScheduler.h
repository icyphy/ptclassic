/******************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

 Programmer:  Soonhoi Ha

 macroScheduler for efficient and flexible parallel scheduler

*******************************************************************/

#ifndef _MacroScheduler_h
#define _MacroScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

#include "ParScheduler.h"
#include "StringList.h"
#include "MacroParProcs.h"
#include "MacroGraph.h"

class CGMacroClusterGal;
class CGMacroCluster;


/////////////////////////
// class MacroScheduler //
/////////////////////////

class MacroParSched : public ParScheduler {
public:
	MacroParSched(MultiTarget* t, const char* log = 0) :
		ParScheduler(t,log), parSched(0) {
                INC_LOG_NEW; myGraph = new MacroGraph; exGraph = myGraph; }
	MacroParSched(MultiTarget* t, ostream* l) :
		ParScheduler(t,0), parSched(0)  {
		INC_LOG_NEW; myGraph = new MacroGraph; exGraph = myGraph;
		logstrm = l;
	}

	~MacroParSched();

        // set-up Processors
        void setUpProcs(int num);
 
	int generateSchedule(CGMacroClusterGal* g);

        // display schedule with numProcs processors.
        /* virtual */ StringList displaySchedule();
 
        // main body of the schedule
        /* virtual */ int scheduleIt();
 
	// final schedule of component clusters.
	/* virtual */ int finalSchedule();

	/* virtual */ int repetitions();

	// redefine 
	/* virtual */ scheduleManually();

protected:
        // The aggregate firing that represents the schedule.
        MacroParProcs* parSched;
        MacroGraph* myGraph;

private:
	// determine the procId of Fork star: used in manual scheduling.
	int procIdOfFork(CGMacroCluster*);
};

class MacroScheduler : public MacroParSched {

public:
	MacroScheduler(MultiTarget* t, const char* log = 0) : 
		MacroParSched(t,log), mGal(0) {}

	/* virtual */ void setup();

protected:
	//
	// First step: decomposition
	// Second step: for each decomposed galaxy, do main scheduling.
	// Return TRUE and leave the invalid flag clear if and
	// only if everything went OK.
	//
	int computeSchedule(Galaxy& g);

	CGMacroClusterGal* mGal;

	// create clusterGal
	virtual CGMacroClusterGal* createClusterGal(Galaxy*, ostream*);
};


#endif
