/******************************************************************
Version identification:
@(#)MacroScheduler.h	1.4	02/22/99

Copyright (c) 1995 Seoul National University
Copyright (c) 1990-1999 The Regents of the University of California.
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
        ParProcessors* setUpProcs(int num);
 
	int generateSchedule(CGMacroClusterGal* g);

        // display schedule with numProcs processors.
        /* virtual */ StringList displaySchedule();
 
        // main body of the schedule
        /* virtual */ int scheduleIt();
 
	// final schedule of component clusters.
	/* virtual */ int finalSchedule();

	/* virtual */ int repetitions();

	// redefine 
	/* virtual */ int scheduleManually();

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
