static const char file_id[] = "$RCSfile$";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 February 1992

    Code for domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFConnect.h"
#include "MTDFGeodesic.h"
#include "CircularBuffer.h"
#include "Plasma.h"
#include "Error.h"

// Class identification.
ISA_FUNC(MTDFPortHole,PortHole);

// Constructor.
MTDFPortHole::MTDFPortHole() : myGeodesic(PortHole::myGeodesic)
{
}

// Allocate and return a MTDFGeodesic.
Geodesic* MTDFPortHole::allocateGeodesic()
{
    LOG_NEW; MTDFGeodesic* g = new MTDFGeodesic;
    char name[80];
    strcpy(name, "Node_");
    strcat(name, readName());
    g->setNameParent(hashstring(name),parent());
    return g;
}

/* Because MTDFGeodsic::get() may block, care must be taken to keep the buffer
   and Plasma consistent.
*/
void MTDFPortHole::getParticle()
{
    Particle **pOld, *pNew;

    // Transfer numberTokens Particles from the Geodesic to the buffer.
    for(int i = numberTokens; i>0; i--)
    {
	// Get a pointer to the next Particle in the buffer.
	pOld = myBuffer->next();

	// Get a new Particle from the Geodesic. (This may block!)
	pNew = myGeodesic->get();

	// Recycle the old particle by putting it into the Plasma.
	myPlasma->put(*pOld);

	// Put the new Particle into the buffer.
	*pOld = pNew;
    }
}

/* Redefining PortHole::putParticle() is necessary only because
   Geodesic::put() is not virtual.
*/
void MTDFPortHole::putParticle()
{
    Particle** p;

    // Fast case for one Particle.
    if (numberTokens == 1)
    {
	p = myBuffer->here();
	myGeodesic->put(*p);
	*p = myPlasma->get();
	return;
    }

    // Slow case for multiple Particles.

    // Back up to the first new Particle in the buffer.
    myBuffer->backup(numberTokens);

    // Transfer numberTokens Particles from the buffer to the Geodesic.
    for(int i = numberTokens; i>0; i--)
    {
	// Get the next Particle from the buffer.
	p = myBuffer->next();

	// Transfer the Particle to the Geodesic.
	myGeodesic->put(*p);

	// Replace the used-up Particle with one recycled from the Plasma.
	*p = myPlasma->get();
    }
}

// Input/output identification.
int InMTDFPort::isItInput() const
{
    return TRUE;
}

// Get data from the Geodesic.
void InMTDFPort::grabData()
{
    getParticle();
}

// Input/output identification.
int OutMTDFPort::isItOutput() const
{
    return TRUE;
}

// Update buffer pointer (for % operator) and clear old Particles.
void OutMTDFPort::grabData()
{
    clearParticle();
}

// Put data into the Geodesic.
void OutMTDFPort::sendData()
{
    putParticle();
}


// Input/output identification.
int MultiInMTDFPort::isItInput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInMTDFPort::newPort()
{
	LOG_NEW; PortHole& p = *new InMTDFPort;
	return installPort(p);
}

// Input/output identification.
int MultiOutMTDFPort::isItOutput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutMTDFPort::newPort()
{
	LOG_NEW; PortHole& p = *new OutMTDFPort;
	return installPort(p);
}
