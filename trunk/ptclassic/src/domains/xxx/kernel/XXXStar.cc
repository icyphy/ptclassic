static const char file_id[] = "XXXStar.cc";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	5 January 1992

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "XXXStar.h"

extern const char XXXdomainName[];

// Class identification.
ISA_FUNC(XXXStar,Star);

// Domain identification.
const char* XXXStar::domain() const
{
    return XXXdomainName;
}

// Domain-specific initialization.
void XXXStar::prepareForScheduling()
{
}
