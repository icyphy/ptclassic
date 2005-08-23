static const char file_id[] = "ACSScheduler.cc";

/*  Version @(#)ACSScheduler.cc	1.6 09/08/99

@Copyright (c) 1998 The Regents of the University of California.
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

    Author:	T.M. Parks
    Created:	6 January 1992

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ACSScheduler.h"
#include "Galaxy.h"
#include "Error.h"

extern const char ACSdomainName[];

// Constructor.
ACSScheduler::ACSScheduler()
{
    setStopTime(0.0);
    setCurrentTime(0.0);

    /* Insert your own initialization code. */
}

// Domain identification.
const char* ACSScheduler::domain() const
{
    return ACSdomainName;
}

// Initialization.
void ACSScheduler::setup()
{
    if (!galaxy()) {
	Error::abortRun(domain(), " scheduler has no galaxy.");
	return;
    }

    galaxy()->initialize();

    /* Insert your own initialiazation code. */
}

// Run (or continue) the simulation.
int ACSScheduler::run()
{
    if (SimControl::haltRequested() || !galaxy()) {
	Error::abortRun(domain(), " Scheduler has no galaxy to run");
	return FALSE;
    }

    /* Insert your own code to run the simulation. */

    return !SimControl::haltRequested();
}

// Get the stopping time.
double ACSScheduler::getStopTime()
{
    /* Replace this code with your own. */
    return 0.0;
}

// Set the stopping time.
void ACSScheduler::setStopTime(double limit)
{
    /* Insert your own code to set the stopping condition. */
}

// Set the stopping time when inside a Wormhole. */
void ACSScheduler::resetStopTime(double limit)
{
    /* Insert your own code to set the stopping condition. */
}
