/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY
*/
/*  Version $Id$
    Author:	T.M. Parks
    Created:	6 February 1992

    Code for domain-specific PortHole classes.
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFPortHole.h"
#include "MTDFGeodesic.h"
#include "CircularBuffer.h"
#include "Plasma.h"
#include "Error.h"
#include <strstream.h>

// Class identification.
ISA_FUNC(MTDFPortHole,DFPortHole);

// Allocate and return a MTDFGeodesic.
Geodesic* MTDFPortHole::allocateGeodesic()
{
    // Construct name for new Geodesic.
    ostrstream string;
    string << "Node_" << name();
    char* nm = string.str();

    LOG_NEW; MTDFGeodesic* g = new MTDFGeodesic;
    g->setNameParent(hashstring(nm), parent());

    LOG_DEL; delete nm;
    return g;
}

void InMTDFPort::wait()
{
    receiveData();
}

/* Because MTDFGeodsic::get() may block, care must be taken to keep the buffer
   and Plasma consistent.
*/
void MTDFPortHole::getParticle()
{
    Particle **pOld, *pNew;

    // Avoid thrashing by waiting until the Geodesic is full.
    myGeodesic->wait();

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

/* Redefining DFPortHole::putParticle() is necessary only because
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

PortHole& MTDFPortHole::setPort(const char* name, Block* parent, DataType type,
    int num, int delay)
{
    if (dynamic = (num < 0)) num = -num;
    return DFPortHole::setPort(name, parent, type, num, delay);
}

// Input/output identification.
int InMTDFPort::isItInput() const
{
    return TRUE;
}

// Get data from the Geodesic.
void InMTDFPort::receiveData()
{
    getParticle();
}

// Input/output identification.
int OutMTDFPort::isItOutput() const
{
    return TRUE;
}

// Update buffer pointer (for % operator) and clear old Particles.
void OutMTDFPort::receiveData()
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
