/* 
Copyright (c) 1990-1993 The Regents of the University of California.
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
*/
/*  Version $Id$
    Author:	T.M. Parks
    Created:	17 January 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFWormhole.h"
#include "MTDFScheduler.h"

// Constructors.
MTDFWormhole::MTDFWormhole(Galaxy& galaxy, Target* target)
: Wormhole(*this, galaxy, target)
{
    buildEventHorizons();
}

MTDFWormhole::MTDFWormhole(Galaxy& galaxy, const char* targetName)
: Wormhole(*this, galaxy, targetName)
{
    buildEventHorizons();
}

// Destructor.
MTDFWormhole::~MTDFWormhole()
{
    freeContents();
}

/* --------------------Star methods-------------------- */

// Make an identical copy.
Block* MTDFWormhole::clone() const
{
    LOG_NEW; return new MTDFWormhole(gal.clone()->asGalaxy(), target->cloneTarget());
}

// Make a new object of the same type.
Block* MTDFWormhole::makeNew() const
{
    // Note that Target does not define a makeNew() method,
    // so it is cloned instead.
    LOG_NEW; return new MTDFWormhole(gal.makeNew()->asGalaxy(), target->cloneTarget());
}

// Identify self as Wormhole.
int MTDFWormhole::isItWormhole() const
{
    return TRUE;
}

// Print a description of the Wormhole.
StringList MTDFWormhole::print(int verbose) const
{
    return Wormhole::print(verbose);
}

// Scheduler for the inner domain.
Scheduler* MTDFWormhole::scheduler() const
{
    return target->scheduler();
}

// Beginning of simulation.
void MTDFWormhole::setup()
{
    Wormhole::setup();
}

// Run the simulation.
void MTDFWormhole::go()
{
    // Synchronize inner domain with the outer.
    target->setCurrentTime(outerSched()->now());

    // Run the inner domain.
    Wormhole::run();
}

// End of simulation.
void MTDFWormhole::wrapup()
{
    target->wrapup();
}

// Bypass Wormhole in parent chain when referring to States.
void MTDFWormhole::initState()
{
    gal.initState();
}

State* MTDFWormhole::stateWithName(const char* name)
{
    return gal.stateWithName(name);
}

/* ------------------Wormhole methods------------------ */

// Stopping condition for inner domain.
double MTDFWormhole::getStopTime()
{
    // Set inner domain to stop before next iteration of the outer domain.
    MTDFScheduler* sched = (MTDFScheduler*)outerSched();
    return sched->now() + sched->schedulePeriod;
}
