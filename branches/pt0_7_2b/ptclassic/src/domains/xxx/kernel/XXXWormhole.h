/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	17 January 1992
*/

#ifndef _XXXWormhole_h
#define _XXXWormhole_h

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "Wormhole.h"
#include "XXXStar.h"

class XXXWormhole : public Wormhole, public XXXStar
{
public:
    // Constructor.
    XXXWormhole(Galaxy& g, Target* t=0);

};
	
#endif
