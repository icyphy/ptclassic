static const char file_id[] = "$RCSfile$";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	17 January 1992
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFWormhole.h"

// Constructor
MTDFWormhole :: MTDFWormhole(Galaxy& g, Target* t) : Wormhole(*this, g, t)
{
}
