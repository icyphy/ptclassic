#ifndef _HuScheduler_h
#define _HuScheduler_h
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

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#include "ParScheduler.h"
#include "HuParProcs.h"


///////////////////////////
// class HuScheduler //
///////////////////////////

class HuScheduler : public ParScheduler {

public:
  	// Constructor
  	HuScheduler(MultiTarget* t, const char* log) : 
		ParScheduler(t, log), parSched(0) {
		INC_LOG_NEW; myGraph = new HuGraph; exGraph = myGraph; }
	~HuScheduler();

  	// set-up Processors
  	void setUpProcs(int num);

  	// display schedule with "numProcs" processors.
  	StringList displaySchedule();

  	// main body of the schedule
  	int scheduleIt();

protected:
   	// The aggregate firing that represents the
   	// schedule.
   	HuParProcs* parSched;

private:
	HuGraph* myGraph;
};

#endif
