/* 
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
*/
/*  Version $Id$
    Programmer:		T.M. Parks
    Date of creation:	2 April 1992

*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFTarget.h"
#include "MTDFScheduler.h"
#include "RTDFScheduler.h"

// Constructor.
MTDFTarget::MTDFTarget() : Target("default-MTDF", "MTDFStar",
	"Schedule dataflow systems using Sun's lightweight process library.")
{
    addState(realTime.setState("realTime", this, "NO",
		"specify whether to run in real time."));
}

// Destructor.
MTDFTarget::~MTDFTarget()
{
    delSched();
}

// Make a new MTDFTarget object.
Block* MTDFTarget::makeNew() const
{
    LOG_NEW; return new MTDFTarget;
}

// Initialization.
void MTDFTarget::setup()
{
    delSched();
    if (realTime) { LOG_NEW; setSched(new RTDFScheduler); }
    else { LOG_NEW; setSched(new MTDFScheduler); }
    Target::setup();
}

// End simulation.
void MTDFTarget::wrapup()
{
    MTDFScheduler* sched = (MTDFScheduler*)scheduler();
    sched->deleteThreads();
    Target::wrapup();
}
