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
    Created:	6 April 1992
*/

static const char file_id[] = "$RCSfile$";

#include "Domain.h"
#include "KnownTarget.h"
#include "MTDFTarget.h"
#include "MTDFForkNode.h"
#include "MTDFWormhole.h"
#include "MTDFEventHorizon.h"

extern const char MTDFdomainName[] = "MTDF";

class MTDFDomain : public Domain
{
public:
    // Constructor.
    MTDFDomain() : Domain(MTDFdomainName) {}

    /*virtual*/ Geodesic& newNode()
	{ LOG_NEW; return *new MTDFForkNode; }

    /*virtual*/ Star& newWorm(Galaxy& galaxy, Target* target=0)
	{ LOG_NEW; return *new MTDFWormhole(galaxy, target); }
    /*virtual*/ EventHorizon& newTo()
	{ LOG_NEW; return *new MTDFtoUniversal; }
    /*virtual*/ EventHorizon& newFrom()
	{ LOG_NEW; return *new MTDFfromUniversal; }
};

// Declare a prototype.
static MTDFDomain protoDomain;

// Add a prototype Target to the KnownTarget list.
static MTDFTarget protoTarget;
static KnownTarget entry(protoTarget,"default-MTDF");
