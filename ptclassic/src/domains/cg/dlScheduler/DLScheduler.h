#ifndef _DLScheduler_h
#define _DLScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

/****************************************************************
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

Programmer: Soonhoi Ha
Date of last revision:

*****************************************************************/

#include "ParScheduler.h"
#include "DLParProcs.h"

///////////////////////
// class DLScheduler //
///////////////////////

class DLScheduler : public ParScheduler {
public:
  	DLScheduler(MultiTarget* t, const char* log, int i) : 
		ParScheduler(t, log), parSched(0), noOverlap(i) {
		INC_LOG_NEW; myGraph = new DLGraph; exGraph = myGraph; }
	~DLScheduler();

  	// set-up Processors
  	void setUpProcs(int num);

  	// display schedule with "numProcs" processors.
  	StringList displaySchedule();

  	// main body of the schedule
  	int scheduleIt();

	// Need to redefine this function for now.
	// Will be removed later.
	int createSubGals();

protected:
   	// The aggregate firing that represents the
   	// schedule.
   	DLParProcs* parSched;

private:
	//temporal hack to prevent createSubGals it wormhole exists
	int wormFlag;
	int noOverlap;	// set if overlapComm. is disallowed.

	DLGraph* myGraph;
};

#endif
