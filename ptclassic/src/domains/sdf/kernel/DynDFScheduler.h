#ifndef _DynDFScheduler_h
#define _DynDFScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"
#include "Galaxy.h"
#include "DataFlowStar.h"

/**************************************************************************
Version identification:
@(#)DynDFScheduler.h	1.5	7/19/95

Copyright (c) 1990-1995 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha, J. Buck

**************************************************************************/


	////////////////////////////
	// DynDFScheduler
	////////////////////////////

class DynDFScheduler : public Scheduler {
public:
	// set parameters for scheduler
	void setParams(int numOver, double pd, int maxbsize) {
		numOverlapped = numOver;
		schedulePeriod = pd;
		maxToken = maxbsize;
	}

	// The setup function prepares the scheduler
	// and initializes all the blocks.
	void setup();

	// The run function resumes the run where it was left off.
	int run();

	StringList displaySchedule() ;

	// Constructor sets default options
	DynDFScheduler ();

	// Timing control funcs
	void setStopTime (double limit);
	void resetStopTime (double v);
	double getStopTime() { return double(stopTime);}

	// scheduler Period : interface with timed domain.
	double schedulePeriod;
protected:
	virtual int checkBlocks();
	virtual void initStructures();
	virtual int isSource(Star& s);
	void resetFlags();
	
	void reportArcOverflow(PortHole*, int);
	PortHole* checkInputOverflow(Star&, int);

	// explicit stopping condition
	int stopTime;

	// list of source blocks.
	SequentialList sourceBlocks;

	// list of non-source blocks.
	SequentialList nonSourceBlocks;

	// the number of firing all sources so far
	int numFiring;

	// capacity of arc
	int maxToken;

	// number of stars in the galaxy
	int galSize;

	// recursion depth for lazy evaluation
	int lazyDepth;

	// arc that exceeds the limit.
	PortHole* overFlowArc;

private:
	// check whether a star is runnable. Also enable lazy evaluation.
	virtual int isRunnable(DataFlowStar&, int enableLazyEvalForDynPorts = TRUE);

	// lazy evaluation
	virtual int lazyEval(DataFlowStar*);
	virtual int checkLazyEval(DataFlowStar *);

	// number of overlapped execution allowed.
	int numOverlapped;
};

#endif
