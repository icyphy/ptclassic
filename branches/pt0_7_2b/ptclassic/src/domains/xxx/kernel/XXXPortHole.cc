/*  Version $Id$

    Copyright 1991 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	5 January 1991

    Code for domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "XXXConnect.h"

// Class identification.
ISA_FUNC(XXXPortHole,PortHole);

// Domain-specific initialization.
PortHole& XXXPortHole::setPort(const char* name, Block* parent, DataType type)
{
    // Initialize PortHole.
    PortHole::setPort(name, parent, type);

    return *this;
}

// Input/output identification.
int InXXXPort::isItInput() const
{
    return TRUE;
}

// Get Particles from input Geodesic.
void InXXXPort::grabData()
{
    getParticle();
}

// Input/output identification.
int OutXXXPort::isItOutput() const
{
    return TRUE;
}

// Put Particles into the output Geodesic.
void OutXXXPort::sendData()
{
    putParticle();
}

// Domain-specific initialization.
MultiPortHole& MultiXXXPort::setPort(const char* name, Block* parent, DataType type)
{
    // Initialize MultiPortHole.
    MultiPortHole::setPort(name, parent, type);

    return *this;
}

// Input/output identification.
int MultiInXXXPort::isItInput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInXXXPort::newPort()
{
	PortHole& p = *new InXXXPort;
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
	PortHole& p = *new OutXXXPort;
	return installPort(p);
}
