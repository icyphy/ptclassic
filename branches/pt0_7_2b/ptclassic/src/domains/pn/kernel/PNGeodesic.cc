static const char file_id[] = "$RCSfile$";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 February 1992

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFGeodesic.h"
#include "CriticalSection.h"

// Class identification.
ISA_FUNC(MTDFGeodesic,AutoForkNode);

// Constructor.
MTDFGeodesic::MTDFGeodesic() : geoMon(), notEmpty(geoMon)
{
}

// put with mutual exclusion.
void MTDFGeodesic::put(Particle* p)
{
    CriticalSection x(geoMon);
    Geodesic::put(p);
    if (size() == 1) notEmpty.notifyAll();	// just became not empty
}

// get with mutual exclusion.
Particle* MTDFGeodesic::get()
{
    CriticalSection x(geoMon);
    while (size() == 0) notEmpty.wait();	// wait for another Particle
    return Geodesic::get();
}


// pushBack with mutual exclusion.
void MTDFGeodesic::pushBack(Particle* p)
{
    CriticalSection x(geoMon);
    Geodesic::pushBack(p);
    if (size() == 1) notEmpty.notifyAll();	// just became not empty
}

// size with mutual exclusion.
int MTDFGeodesic::size()
{
    CriticalSection x(geoMon);
    return Geodesic::size();
}


