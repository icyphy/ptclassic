/* 
Copyright (c) 1990-%Q% The Regents of the University of California.
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
    Created:	6 February 1992

    Geodesic which blocks if accessed when full or empty.
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "PNThread.h"
#include "PNGeodesic.h"
#include "Error.h"

// Class identification.
ISA_FUNC(PNGeodesic, Geodesic);

// Constructor.
PNGeodesic::PNGeodesic() : notEmpty(0), notFull(0), cap(1)
{ }

void PNGeodesic::makeLock(const PtGate& master)
{
    LOG_DEL; delete notEmpty; delete notFull;
    notEmpty = notFull = 0;
    Geodesic::makeLock(master);
    if (!isLockEnabled())
    {
	Error::abortRun(*this, "makeLock failed!");
    }
    else
    {
	LOG_NEW; notEmpty = new PNCondition(*(PNMonitor*)gate);
	LOG_NEW; notFull = new PNCondition(*(PNMonitor*)gate);
    }
}

void PNGeodesic::delLock()
{
    LOG_DEL; delete notEmpty; delete notFull;
    notEmpty = notFull = 0;
    Geodesic::delLock();
}

// Block when full.
// Notify when not empty.
void PNGeodesic::slowPut(Particle* p)
{
    // Avoid entering the gate more than once.
    CriticalSection region(gate);
    while (sz >= cap && notFull) notFull->wait();
    pstack.putTail(p); sz++;
    if (notEmpty) notEmpty->notifyAll();
}

// Block when empty.
// Notify when not full.
Particle* PNGeodesic::slowGet()
{
    // Avoid entering the gate more than once.
    CriticalSection region(gate);
    while (sz < 1 && notEmpty) notEmpty->wait();
    sz--; Particle* p = pstack.get();
    if (sz < cap && notFull) notFull->notifyAll();
    return p;
}

// Notify when not empty.
// Don't block when full.
void PNGeodesic::pushBack(Particle* p)
{
    // Avoid entering the gate more than once.
    CriticalSection region(gate);
    pstack.put(p); sz++;
    if (isLockEnabled() && notEmpty) notEmpty->notifyAll();
}

void PNGeodesic::setCapacity(int c)
{
    cap = c;
    if (sz < cap && notFull) notFull->notifyAll();
}
