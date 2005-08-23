/* 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
/*  Version @(#)PNGeodesic.h	2.4 3/7/96
    Author:	T.M. Parks
    Created:	6 February 1992

    Geodesic with mutual exclusion for all methods which access data members
    at run-time.  The capacity of the Geodesic may be limited to throttle the
    source.
*/

#ifndef _PNGeodesic_h
#define _PNGeodesic_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Geodesic.h"
#include "PtGate.h"

class PtCondition;

class PNGeodesic : public Geodesic
{
public:
    // Class identification.
    /*virtual*/ int isA(const char*) const;

    // Constructor.
    PNGeodesic();

    // Notify when not empty.
    /*virtual*/ void slowPut(Particle*);

    // Block until not empty.
    /*virtual*/ Particle* slowGet();

    // Notify when not empty.
    void pushBack(Particle*);

    /*virtual*/ void makeLock(const PtGate& master);
    /*virtual*/ void delLock();

    // Limit the number of Particles on the Geodesic.
    int capacity() const { return cap; }
    void setCapacity(int c);

    // Number of full geodesics.
    static int blockedOnFull();
    static void resetFull();

protected:
    PtCondition* notEmpty;
    PtCondition* notFull;

    // capacity limit
    int cap;

    static PtGate* fullGate;
    static GateKeeper fullKeeper;
    static int numFull;
};

#endif
