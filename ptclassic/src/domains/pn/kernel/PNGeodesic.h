/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 February 1992

*/

#ifndef _MTDFGeodesic_h
#define _MTDFGeodesic_h

#ifdef __GNUG__
#pragma interface
#endif

#include "AutoForkNode.h"
#include "Monitor.h"
#include "Condition.h"

// Mutual exclusion must be provided for all methods which access data
// members at run-time.
class MTDFGeodesic : public AutoForkNode
{
public:
    // Class identification.
    virtual int isA(const char* className) const;

    // Constructor.
    MTDFGeodesic();

    // put with mutual exclusion.
    void put(Particle* p);

    // get with mutual exclusion.
    Particle* get();

    // pushBack with mutual exclusion.
    void pushBack(Particle* p);

    // size with mutual exclusion.
    int size();

protected:
    Monitor geoMon;
    Condition notEmpty;
};

#endif
