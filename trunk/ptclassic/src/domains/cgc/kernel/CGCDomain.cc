static const char file_id[] = "CGCDomain.cc";
/**********************************************************************
Version identification:
$Id$

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

 Programmer:  J. T. Buck
 Date of creation: 1/2/92

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the CGC domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "CGCTarget.h"
#include "KnownTarget.h"
#include "CGCPortHole.h"
#include "CGCForkNode.h"
#include "CGCWormhole.h"

// for error messages (temporary, until wormholes exist):
#include "Error.h"

extern const char CGCdomainName[] = "CGC";

class CGCDomain : public Domain {
public:
	// no XXXinCGC wormholes yet, so no newWorm

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		LOG_NEW; return *(new CGCfromUniversal);
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		LOG_NEW; return *(new CGCtoUniversal);
	}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new CGCForkNode;}

	// constructor
	CGCDomain() : Domain("CGC") {}
};

// declare a prototype
static CGCDomain proto;

// declare the default Target object

static CGCTarget defaultCGCtarget("default-CGC","CGCStar",
"Generate stand-alone C programs and compile them.  The program\n"
"and associated makefile is written to a directory given as a Target param.\n"
"Can use either the default SDF scheduler or Joe's loop scheduler.");

static KnownTarget entry(defaultCGCtarget,"default-CGC");
