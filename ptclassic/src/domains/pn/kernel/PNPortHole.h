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

    Definitions of domain-specific PortHole classes.
*/

#ifndef _PNPortHole_h
#define _PNPortHole_h

#ifdef __GNUG__
#pragma interface
#endif

#include "SDFPortHole.h"

const bitWord PB_DYNAMIC = 0x20;
extern const Attribute P_DYNAMIC;
extern const Attribute P_STATIC;

class PNGeodesic;

class PNPortHole : public DFPortHole
{
public:
    // Class identification.
    /*virtual*/ int isA(const char* className) const;
    /*virtual*/ int isDynamic() const;
};

class InPNPort : public PNPortHole
{
public:
    // Input/output identification.
    /*virtual*/ int isItInput() const;

    // Get data from the Geodesic.
    /*virtual*/ void receiveData();
};


class OutPNPort : public PNPortHole
{
public:
    // Input/output identification.
    /*virtual*/ int isItOutput() const;

    // Update buffer pointer (for % operator) and clear old Particles.
    /*virtual*/ void receiveData();

    // Put data into the Geodesic.
    /*virtual*/ void sendData();
};

// MultiPNPort is identical to MultiDFPort.
typedef MultiDFPort MultiPNPort;

class MultiInPNPort : public MultiPNPort
{
public:
    // Input/output identification.
    /*virtual*/ int isItInput() const;
 
    // Add a new physical port to the MultiPortHole list.
    /*virtual*/ PortHole& newPort();
};
 
class MultiOutPNPort : public MultiPNPort
{     
public:
    // Input/output identification.
    /*virtual*/ int isItOutput() const;

    // Add a new physical port to the MultiPortHole list.
    /*virtual*/ PortHole& newPort();
};

#endif
