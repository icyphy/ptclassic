/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	2 April 1992
*/

#ifndef _MTDFTarget_h
#define _MTDFTarget_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "MTDFScheduler.h"

class Galaxy;

class MTDFTarget : public Target
{
public:
    // Constructor.
    MTDFTarget();

    // Destructor.
    ~MTDFTarget();

    // Make a new MTDFTarget object.
    virtual Block* clone() const;

    virtual void start();

    // End simulation.
    virtual void wrapup();
};

#endif
