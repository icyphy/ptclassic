/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 April 1992

*/

static const char file_id[] = "$RCSfile$";

#include "Domain.h"
#include "KnownTarget.h"
#include "MTDFTarget.h"
#include "MTDFForkNode.h"

extern const char MTDFdomainName[] = "MTDF";

class MTDFDomain : public Domain
{
public:
    // Constructor.
    MTDFDomain() : Domain(MTDFdomainName) {}

    /* virtual */ Geodesic& newNode() { LOG_NEW; return *new MTDFForkNode; }
};

// Declare a prototype.
static MTDFDomain protoDomain;

// Add a prototype Target to the KnownTarget list.
static MTDFTarget protoTarget;
static KnownTarget entry(protoTarget,"default-MTDF");
