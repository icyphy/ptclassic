/*  Version $Id$
    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 February 1992

*/

#ifndef _MTDFStar_h
#define _MTDFStar_h

#ifdef __GNUG__
#pragma interface
#endif

#include "SDFStar.h"
#include "MTDFConnect.h"

class MTDFStar : public DataFlowStar
{
public:
    // Class identification.
    virtual int isA(const char* className) const;

    // Domain identification.
    virtual const char* domain() const;

    // Domain-specific initialization.
    virtual void prepareForScheduling();

    // Identify Star as a data source.
    int isItSource() const;
};

#endif
