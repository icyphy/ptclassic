/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 February 1992

    Geodesic with mutual exclusion for all methods which access data members
    at run-time.
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

protected:
    Monitor geoMon;
    Condition notEmpty;
};

#endif
