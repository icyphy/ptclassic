#ifndef _PNWormhole_h
#define _PNWormhole_h

/* 
Copyright (c) 1990-1993 The Regents of the University of California.
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
*/
/*  Version $Id$
    Author:	T. M. Parks
    Created:	17 January 1992

    Wormhole for the PN domain.
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "Wormhole.h"
#include "PNStar.h"

class Scheduler;

class PNWormhole : public Wormhole, public PNStar
{
public:
    // Constructors.
    PNWormhole(Galaxy& galaxy, Target* target=0);
    PNWormhole(Galaxy& galaxy, const char* targetName = 0);

    // Destructor.
    /*virtual*/ ~PNWormhole();

// Star methods.
public:
    // Make an identical copy.
    /*virtual*/ Block* clone() const;

    // Make a new object of the same type.
    /*virtual*/ Block* makeNew() const;

    // Identify self as Wormhole.
    /*virtual*/ int isItWormhole() const;

    // Print a description of the Wormhole.
    /*virtual*/ StringList print(int) const;

    // Scheduler for the inner domain.
    /*virtual*/ Scheduler* scheduler() const;

    /*virtual*/ void begin() { Wormhole::begin(); }

    // End of simulation.
    /*virtual*/ void wrapup();

    // Bypass Wormhole in parent chain when referring to States.
    /*virtual*/ void initState();
    /*virtual*/ State* stateWithName(const char*);

protected:
    // Beginning of simulation.
    /*virtual*/ void setup();

    // Run the simulation.
    /*virtual*/ void go();

// Wormhole methods.
public:
protected:
    // Stopping condition for the inner domain.
    /*virtual*/ double getStopTime();
};
	
#endif
