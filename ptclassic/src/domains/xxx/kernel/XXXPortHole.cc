static const char file_id[] = "XXXConnect.cc";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	5 January 1992

    Code for domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "XXXConnect.h"

// Class identification.
ISA_FUNC(XXXPortHole,PortHole);

// Input/output identification.
int InXXXPort::isItInput() const
{
    return TRUE;
}

// Input/output identification.
int OutXXXPort::isItOutput() const
{
    return TRUE;
}

// Input/output identification.
int MultiInXXXPort::isItInput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInXXXPort::newPort()
{
	LOG_NEW; PortHole& p = *new InXXXPort;
	return installPort(p);
}

// Input/output identification.
int MultiOutXXXPort::isItOutput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutXXXPort::newPort()
{
	LOG_NEW; PortHole& p = *new OutXXXPort;
	return installPort(p);
}
