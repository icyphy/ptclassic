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
    g->makeLock(LwpMonitor::prototype);

    LOG_DEL; delete nm;
    return g;
}

int MTDFPortHole::allocatePlasma()
{
    if (DFPortHole::allocatePlasma())
    {
	myPlasma->makeLock(LwpMonitor::prototype);
	return TRUE;
    }
    return FALSE;
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
