/*  Version $Id$

    Copyright 1991 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	5 January 1991

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "XXXStar.h"
#include "XXXConnect.h"

// The following is defined in XXXDomain.cc -- this forces that module
// to be included if any XXX stars are linked in.
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
{}

// Firing definition.
void XXXStar::fire()
{
    BlockPortIter port(*this);

    // Get data for all PortHoles.
    for(int i = numberPorts(); i > 0; i--)
	(port++)->grabData();

    // Fire the Star.
    Star::fire();

    // Send data for all PortHoles.
    port.reset();
    for(i = numberPorts(); i > 0; i--)
	(port++)->sendData();
}
