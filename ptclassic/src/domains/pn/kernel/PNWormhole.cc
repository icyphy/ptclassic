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

#include "PNWormhole.h"
#include "PNScheduler.h"

// Constructors.
PNWormhole::PNWormhole(Galaxy& galaxy, Target* target)
: Wormhole(*this, galaxy, target)
{
    buildEventHorizons();
}

PNWormhole::PNWormhole(Galaxy& galaxy, const char* targetName)
: Wormhole(*this, galaxy, targetName)
{
    buildEventHorizons();
}

// Destructor.
PNWormhole::~PNWormhole()
{
    freeContents();
}

/* --------------------Star methods-------------------- */

// Make an identical copy.
Block* PNWormhole::clone() const
{
    LOG_NEW; return new PNWormhole(gal.clone()->asGalaxy(), myTarget()->cloneTarget());
}

// Make a new object of the same type.
Block* PNWormhole::makeNew() const
{
    // Note that Target does not define a makeNew() method,
    // so it is cloned instead.
    LOG_NEW; return new PNWormhole(gal.makeNew()->asGalaxy(), myTarget()->cloneTarget());
}

// Identify self as Wormhole.
int PNWormhole::isItWormhole() const
{
    return TRUE;
}

// Print a description of the Wormhole.
StringList PNWormhole::print(int verbose) const
{
    return Wormhole::print(verbose);
}

// Scheduler for the inner domain.
Scheduler* PNWormhole::scheduler() const
{
    return myTarget()->scheduler();
}

// Beginning of simulation.
void PNWormhole::setup()
{
    Wormhole::setup();
}

// Run the simulation.
void PNWormhole::go()
{
    // Synchronize inner domain with the outer.
    myTarget()->setCurrentTime(outerSched()->now());

    // Run the inner domain.
    Wormhole::run();
}

// End of simulation.
void PNWormhole::wrapup()
{
    myTarget()->wrapup();
}

// Bypass Wormhole in parent chain when referring to States.
void PNWormhole::initState()
{
    gal.initState();
}

State* PNWormhole::stateWithName(const char* name)
{
    return gal.stateWithName(name);
}

/* ------------------Wormhole methods------------------ */

// Stopping condition for inner domain.
double PNWormhole::getStopTime()
{
    // Set inner domain to stop before next iteration of the outer domain.
    PNScheduler* sched = (PNScheduler*)outerSched();
    return sched->now() + sched->schedulePeriod;
}
