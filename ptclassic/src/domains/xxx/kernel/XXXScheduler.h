/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 January 1992

*/

#ifndef _XXXScheduler_h
#define _XXXScheduler_h

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "Scheduler.h"
class Galaxy;

class XXXScheduler : public Scheduler
{
public:
    // Domain identification.
    virtual const char* domain() const;

    // Set up the schedule and initialize all Blocks.
    // Return TRUE on success.
    virtual int setup(Galaxy& galaxy);

    // Run the simulation.
    virtual int run(Galaxy& galaxy);

    // Set the stopping time.
    virtual void setStopTime(float limit);
};

#endif
