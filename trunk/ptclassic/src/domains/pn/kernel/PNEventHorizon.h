/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	20 January 1992
*/

#ifndef _MTDFWormConnect_h
#define _MTDFWormConnect_h

#ifdef __GNUG__
#pragma interface
#endif

#include "WormConnect.h"

class MTDFtoUniversal : public ToEventHorizon
{
    void grabData();
};

class MTDFfromUniversal : public FromEventHorizon
{
    void sendData();
};

#endif
