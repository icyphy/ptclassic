/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	28 August 1992

    Auto-forking version of MTDFGeodesic.
*/

#ifndef _MTDFForkNode_h
#define _MTDFForkNode_h

#ifdef __GNUG__
#pragma interface
#endif

#include "MTDFGeodesic.h"
#include "AutoFork.h"

class MTDFForkNode : public MTDFGeodesic
{
public:
    // Class identification.
    virtual int isA(const char*) const;

    // Constructor.
    MTDFForkNode() : autoFork(*this) {}

    // Identify node as persistent.
    int isItPersistent () const;

    // Make a new source connection.
    PortHole* setSourcePort (GenericPort &, int delay = 0);

    // Make a new destination connection.
    PortHole* setDestPort (GenericPort &);

private:
    AutoFork autoFork;
};

#endif
