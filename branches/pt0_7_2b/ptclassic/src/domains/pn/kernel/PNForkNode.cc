/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	28 August 1992

    Auto-forking version of MTDFGeodesic.
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFForkNode.h"

// Class identification.
ISA_FUNC(MTDFForkNode, MTDFGeodesic);

// Identify node as persistent.
int MTDFForkNode::isItPersistent() const
{
    return TRUE;
}

// Make a new source connection.
PortHole* MTDFForkNode::setSourcePort(GenericPort &port, int delay)
{
    return autoFork.setSource(port, delay);
}

// Make a new destination connection.
PortHole* MTDFForkNode::setDestPort(GenericPort &port)
{
    // Make fork Star only after first connection.
    return autoFork.setDest(port);
}
