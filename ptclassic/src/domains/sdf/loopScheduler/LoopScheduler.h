/******************************************************************
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

 Programmer:  Shuvra Bhattacharyya
 Modifier: Soonhoi Ha (5/92)
 Date of creation: 5/9/91

Loop scheduler

*******************************************************************/

#ifndef _LoopScheduler_h
#define _LoopScheduler_h
#ifdef __GNUG__
#pragma interface
#endif

#include "SDFCluster.h"
#include "StringList.h"

/////////////////////////
// class LoopScheduler //
/////////////////////////

class LoopScheduler : public SDFClustSched {

public:
	LoopScheduler(const char* log) : SDFClustSched(log) {}

	// Display the schedule
	StringList displaySchedule();

	// Generate code using the Target to produce the right language.
	void compileRun();

protected:
	//
	// First step: decomposition
	// Second step: for each decomposed galaxy, do main scheduling.
	// Return TRUE and leave the invalid flag clear if and
	// only if everything went OK.
	//
	int computeSchedule(Galaxy& g);
};

#endif
