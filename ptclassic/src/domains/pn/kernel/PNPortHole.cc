/* 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
/*  Version @(#)PNPortHole.cc	2.6 3/19/96
    Author:	T.M. Parks
    Created:	6 February 1992

    Code for domain-specific PortHole classes.
*/

static const char file_id[] = "PNPortHole.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "PNPortHole.h"

extern const Attribute P_DYNAMIC = {PB_DYNAMIC,0};
extern const Attribute P_STATIC = {0,PB_DYNAMIC};

// Class identification.
ISA_FUNC(PNPortHole,DFPortHole);

int PNPortHole::isDynamic() const
{
    return attributes() & PB_DYNAMIC;
}

// Input/output identification.
int InPNPort::isItInput() const
{
    return TRUE;
}

// Get data from the Geodesic.
void InPNPort::receiveData()
{
    getParticle();
}

// Input/output identification.
int OutPNPort::isItOutput() const
{
    return TRUE;
}

// Update buffer pointer (for % operator) and clear old Particles.
void OutPNPort::receiveData()
{
    clearParticle();
}

// Put data into the Geodesic.
void OutPNPort::sendData()
{
    putParticle();
}

// Input/output identification.
int MultiInPNPort::isItInput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInPNPort::newPort()
{
    LOG_NEW; InPNPort& p = *new InPNPort;
    Attribute a = {0, 0}; a.bitsOn = attributes();
    p.setAttributes(a);
    return installPort(p);
}

// Input/output identification.
int MultiOutPNPort::isItOutput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutPNPort::newPort()
{
    LOG_NEW; OutPNPort& p = *new OutPNPort;
    Attribute a = {0, 0}; a.bitsOn = attributes();
    p.setAttributes(a);
    return installPort(p);
}
