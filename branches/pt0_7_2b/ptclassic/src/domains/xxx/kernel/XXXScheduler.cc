static const char file_id[] = "XXXScheduler.cc";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 January 1992

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "XXXScheduler.h"

extern const char XXXdomainName[];

// Domain identification.
const char* XXXScheduler::domain() const
{
    return XXXdomainName;
}

// Set up the schedule.
int XXXScheduler::setup(Galaxy& galaxy)
{
}

// Run the simulation.
int XXXScheduler::run(Galaxy& galaxy)
{
}

// Set the stopping time.
void XXXScheduler::setStopTime(float limit)
{
}

