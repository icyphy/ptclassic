/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	5 January 1992

*/

#ifndef _XXXStar_h
#define _XXXStar_h

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "Star.h"

class XXXStar : public Star
{
public:
    // Class identification.
    virtual int isA(const char* className) const;

    // Domain identification.
    virtual const char* domain() const;

    // Domain-specific initialization.
    virtual void prepareForScheduling();
};

#endif
