/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	17 January 1992
*/

#ifndef _MTDFWormhole_h
#define _MTDFWormhole_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Wormhole.h"
#include "MTDFStar.h"

class MTDFWormhole : public Wormhole, public MTDFStar
{
public:
    // Constructor.
    MTDFWormhole(Galaxy& g, Target* t=0);

    // Destructor.
    ~MTDFWormhole();

    // Inherited Star methods.
    virtual void start();
    virtual void go();
    virtual void wrapup();
};
	
#endif
