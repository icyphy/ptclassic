#ifndef _ParProcessors_h
#define _ParProcessors_h

#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#include "NamedObj.h"
#include "UniProcessor.h"
#include "IntArray.h"
#include "BaseMultiTarget.h"

/////////////////////////
// class ParProcessors //
/////////////////////////

// A class for managing the parallel processor schedules.

class ParProcessors {

public:
	// constructor
	ParProcessors(int, BaseMultiTarget*);

	virtual ~ParProcessors()  {}

	// return size
	int size()	{ return numProcs; }

	// sort the processor indices with finish time.
	void sortWithFinishTime();

	// get the i-th processor. Processors are indexed from 0 to
	// numProcs-1. Should be redefined.
	virtual UniProcessor* getProc(int num);

	// get makespan
	int getMakespan(); 

	// initialize arrays
	void initialize();

	// display
	StringList display(NamedObj* gal);

protected:
	// number of processors.
	int numProcs;

	// target pointer
	BaseMultiTarget* mtarget;

  	// pattern of processor availability (PPA) is stored in indices.
  	IntArray pIndex;

	// processor indices
	IntArray pId;

	// sort the processor ids with available times if they are not unused
	// processors. All unused processors are appended after the processors
	// that are available at the specified limit and before the busy
	// processors at that time.
	void sortWithAvailTime(int);

};

#endif
