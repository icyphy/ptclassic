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
    at run-time.  The capacity of the Geodesic may be limited to throttle the
    source.
*/

#ifndef _MTDFGeodesic_h
#define _MTDFGeodesic_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Geodesic.h"
#include "Monitor.h"
#include "Condition.h"

class MTDFGeodesic : public Geodesic
{
public:
    // Class identification.
    /* virtual */ int isA(const char*) const;

    // Constructor.
    MTDFGeodesic();

    // put with mutual exclusion.
    void put(Particle*);

    // get with mutual exclusion.
    Particle* get();

    // pushBack with mutual exclusion.
    void pushBack(Particle*);

    // size with mutual exclusion.
    int size();

    void setCapacity(int);
    void wait(int);

protected:
    Monitor geoMon;
    Condition notEmpty;
    Condition notFull;
    int capacity;
    int threshold;
};

#endif
