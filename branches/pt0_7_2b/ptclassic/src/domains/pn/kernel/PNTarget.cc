static const char file_id[] = "$RCSfile$";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	2 April 1992

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFTarget.h"
#include "MTDFStar.h"

#include "GalIter.h"

// Constructor.
MTDFTarget::MTDFTarget() : Target("default-MTDF", "MTDFStar",
	"Schedule MTDF systems using Sun's Lightweight Process library.")
{
}

// Destructor.
MTDFTarget::~MTDFTarget()
{
    delSched();
}

// Make a new MTDFTarget object.
Block* MTDFTarget::clone() const
{
    LOG_NEW; return new MTDFTarget;
}

// Initialization.
void MTDFTarget::start()
{
    if (!mySched()) { LOG_NEW; setSched(new MTDFScheduler); }
}

// End simulation.
void MTDFTarget::wrapup()
{
    ((MTDFScheduler*)mySched())->deleteThreads();
    Target::wrapup();
}
