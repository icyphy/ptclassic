#ifndef _PNEventHorizon_h
#define _PNEventHorizon_h

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
/*  Version @(#)PNEventHorizon.h	2.2 1/1/96
    Author:	T. M. Parks
    Created:	20 January 1992
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "EventHorizon.h"
#include "PNPortHole.h"

class PNtoUniversal : public ToEventHorizon, public InPNPort
{
public:
    // Constructor.
    PNtoUniversal() : ToEventHorizon(this) {}

    /*virtual*/ void initialize();
    /*virtual*/ int isItInput() const;
    /*virtual*/ int isItOutput() const;
    /*virtual*/ EventHorizon* asEH();

    /*virtual*/ void receiveData();

    /*virtual*/ Geodesic* allocateGeodesic()
    { return ToEventHorizon::allocateGeodesic(); }
};

class PNfromUniversal : public FromEventHorizon, public OutPNPort
{
public:
    // Constructor.
    PNfromUniversal() : FromEventHorizon(this) {}

    /*virtual*/ void initialize();
    /*virtual*/ int isItInput() const;
    /*virtual*/ int isItOutput() const;
    /*virtual*/ EventHorizon* asEH();

    /*virtual*/ void sendData();

    /*virtual*/ Geodesic* allocateGeodesic()
    { return FromEventHorizon::allocateGeodesic(); }
};

#endif
