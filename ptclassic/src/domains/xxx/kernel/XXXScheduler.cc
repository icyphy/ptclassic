static const char file_id[] = "$RCSfile$";

/*  Version $Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

    Author:	T.M. Parks
    Created:	6 January 1992

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "XXXScheduler.h"
#include "Galaxy.h"
#include "Error.h"

extern const char XXXdomainName[];

// Constructor.
XXXScheduler::XXXScheduler()
{
    setStopTime(0.0);
    setCurrentTime(0.0);

    /* Insert your own initialization code. */
}

// Domain identification.
const char* XXXScheduler::domain() const
{
    return XXXdomainName;
}

// Initialization.
void XXXScheduler::setup()
{
    if (galaxy() == NULL)
    {
	Error::abortRun(domain(), "Scheduler has no galaxy.");
	return;
    }

    galaxy()->initialize();

    /* Insert your own initialiazation code. */
}

// Run (or continue) the simulation.
int XXXScheduler::run()
{
    if (SimControl::haltRequested())
    {
	Error::abortRun(*galaxy(), " cannot continue.");
	return FALSE;
    }

    /* Insert your own code to run the simulation. */

    return !SimControl::haltRequested();
}

// Get the stopping time.
double XXXScheduler::getStopTime()
{
    /* Replace this code with your own. */
    return 0.0;
}

// Set the stopping time.
void XXXScheduler::setStopTime(double limit)
{
    /* Insert your own code to set the stopping condition. */
}

// Set the stopping time when inside a Wormhole. */
void XXXScheduler::resetStopTime(double limit)
{
    /* Insert your own code to set the stopping condition. */
}
