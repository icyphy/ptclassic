static const char file_id[] = "$RCSfile$";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	7 February 1992
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFStar.h"
#include "ConstIters.h"

extern const char MTDFdomainName[];

// Class identification.
ISA_FUNC(MTDFStar,DataFlowStar);

// Domain identification.
const char* MTDFStar::domain() const
{
    return MTDFdomainName;
}

// Domain-specific initialization.
void MTDFStar::prepareForScheduling()
{
}

// Identify Star as a source.
int MTDFStar::isItSource() const
{
    CBlockPortIter port(*this);
    const PortHole* p;

    while ((p = port++) != NULL)
    {
	if (p->isItInput()) return FALSE;
    }
    return TRUE;
}
