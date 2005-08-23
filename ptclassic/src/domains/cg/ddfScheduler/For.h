#ifndef _For_h
#define _For_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "DyConstruct.h"

/**************************************************************************
Version identification:
@(#)For.h	1.2	01/01/96

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

 Programmer:  Soonhoi Ha

 For construct
 -------------
 
 Topology for the For domain...

	for(i, , ) {
	}

 input -->  UpSample -->	   --> DownSample --> output
 input -->  UpSample -->	   --> DownSample --> output
		.	  cluster 
		.
 input -->  UpSample -->	   --> DownSample --> output
		^			   ^
 control -------|--------------------------|

 
 UpSample : Repeater, DownCounter, LastN, UpSampler, etc...
 DownSample : LastOfN, etc...

**************************************************************************/

class For : public DyConstruct {
public:
	// redefine
	int checkTopology(CGMacroClusterGal*);
	void clearSchedules();

	// create schedules
	void createSchedule() { INC_LOG_NEW; schedules = new SequentialList[3]; }
protected:
	int noUp;
};

#endif
