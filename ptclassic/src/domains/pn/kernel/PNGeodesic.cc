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
    Date of creation:	6 February 1992

    Geodesic with mutual exclusion for all methods which access data members
    at run-time.
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFGeodesic.h"
#include "CriticalSection.h"

// Class identification.
ISA_FUNC(MTDFGeodesic, Geodesic);

// Constructor.
MTDFGeodesic::MTDFGeodesic() : geoMon(), notEmpty(geoMon), notFull(geoMon)
{
    // Non-positive capacity indicates that there is no limit.
    capacity = 0;

    // By default, only one Particle is required for notEmpty.
    threshold = 1;
}

// put with mutual exclusion.
void MTDFGeodesic::put(Particle* p)
{
    CriticalSection x(geoMon);
    if (capacity > 0 && Geodesic::size() >= capacity) notFull.wait();
    Geodesic::put(p);
    if (Geodesic::size() == threshold) notEmpty.notifyAll();
}

// get with mutual exclusion.
Particle* MTDFGeodesic::get()
{
    CriticalSection x(geoMon);
    if (Geodesic::size() < threshold) notEmpty.wait();
    threshold = 1;	// reset threshold
    Particle* p = Geodesic::get();
    if (Geodesic::size() == capacity-1) notFull.notifyAll();
    return p;
}

// pushBack with mutual exclusion.
void MTDFGeodesic::pushBack(Particle* p)
{
    CriticalSection x(geoMon);
    Geodesic::pushBack(p);
    if (Geodesic::size() == threshold) notEmpty.notifyAll();
}

// size with mutual exclusion.
int MTDFGeodesic::size()
{
    CriticalSection x(geoMon);
    return Geodesic::size();
}

// Limit the capacity of the Geodesic.
void MTDFGeodesic::setCapacity(int c)
{
    CriticalSection x(geoMon);
    capacity = c;
    if (capacity > 0 && threshold > capacity) threshold = capacity;
}

/* Set threshold and wait until the Geodesic is full.  Capacity is
   temporarily limited to automatically block the source as soon as the
   Geodesic is full.
*/
void MTDFGeodesic::wait(int t)
{
    CriticalSection x(geoMon);
    threshold = t;
    if (capacity > 0 && threshold > capacity) threshold = capacity;
    if (Geodesic::size() < threshold)
    {
	int c = capacity;	// save current capacity
	capacity = threshold;
	notEmpty.wait();
	capacity = c;		// restore original capacity
	if (capacity <= 0 || Geodesic::size() < capacity)
	    notFull.notifyAll();
    }
}
