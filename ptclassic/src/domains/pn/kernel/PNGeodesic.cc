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
    Author:	T.M. Parks
    Created:	6 February 1992

    Geodesic which blocks if accessed when full or empty.
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFGeodesic.h"

// Class identification.
ISA_FUNC(MTDFGeodesic, Geodesic);

// Constructor.
MTDFGeodesic::MTDFGeodesic() : notEmpty(0)
{ }

void MTDFGeodesic::makeLock(const PtGate& master)
{
    LOG_DEL; delete notEmpty;
    notEmpty = 0;
    Geodesic::makeLock(master);
    LOG_NEW; notEmpty = new LwpCondition((LwpMonitor*)gate);
}

void MTDFGeodesic::delLock()
{
    LOG_DEL; delete notEmpty;
    notEmpty = 0;
    Geodesic::delLock();
}

// Notify when not empty.
void MTDFGeodesic::slowPut(Particle* p)
{
    CriticalSection region(gate);
    Geodesic::slowPut(p);
    notEmpty->notifyAll();
}

// Block util not empty.
Particle* MTDFGeodesic::slowGet()
{
    CriticalSection region(gate);
    if (size() < 1) notEmpty->wait();
    Particle* p = Geodesic::slowGet();
    return p;
}

// Notify when not empty.
void MTDFGeodesic::pushBack(Particle* p)
{
    CriticalSection region(gate);
    Geodesic::pushBack(p);
    if (gate) notEmpty->notifyAll();
}
