static const char file_id[] = "ptclDescription.cc";
/******************************************************************
Version identification:
@(#)ptclDescription.cc	1.3 02/11/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  Brian L. Evans and Jose Luis Pino
 Date of creation: 9/2/96

 This file contains the ptclDescription function which generates
 PTcl code for a given galaxy.
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "PortHole.h"
#include "Star.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "pt_fstream.h"

// Supporting routines

// Write out the header which resets the state of ptcl and declares the
// new galaxy name
static StringList ptclHeaderCode(Galaxy* localGalaxy) {
    StringList ptclCode = "reset\n";
    ptclCode << "newuniverse " << localGalaxy->name() << " "
			       << localGalaxy->domain() << " \n";
    return ptclCode;
}

// Generate PTcl code for a galaxy independent of the domain of the galaxy
static StringList ptclGalaxyCode(Galaxy* localGalaxy) {
    StringList ptclCode;

    // Iterator over the stars in the galaxy
    GalStarIter nextStar(*localGalaxy);
    Star* star;

    // Create table of connections between ports of different stars
    int nports = setPortIndices(*localGalaxy);
    int* portNumber = new int[nports];
    while ((star = nextStar++) != NULL) {
	int input = 1;
	int output = 1;
	BlockPortIter nextPort(*star);
	PortHole* port;
	while ((port = (PortHole*)nextPort++) != NULL) {
	    portNumber[port->index()] = (port->isItInput()?input++:output++);
	}
    }
    nextStar.reset();

    // Generate the Ptcl code for the galaxy
    while ((star = nextStar++) != NULL) {
	StringList starName;
	starName << " \"" << star->fullName() << "\" ";
	StringList setstate;
	setstate << "\tsetstate  " << starName << " ";
	BlockPortIter nextPort(*star);
	PortHole* port;
	while ((port = nextPort++) != NULL) {
	    if (port->isItOutput()) {
	        if (port->far() == NULL) {
	           ptclCode << "\t# Warning " << starName
		            << " \"output#" << portNumber[port->index()]
		            << "\" is disconnected\n";
                } else {
		   ptclCode << "\tconnect" << starName
			    << " \"output#" << portNumber[port->index()]
			    << "\" \"" << port->far()->parent()->fullName()
			    << "\" \"input#" << portNumber[port->far()->index()]
			    << "\"\n";
		}
	    }
	}
    }

    delete [] portNumber;

    return ptclCode;
}

static void writePtclCode(StringList& ptclCode, const char* path) {
    // The pt_ofstream constructor automatically expands environment variables
    pt_ofstream ptclFile(path);
    ptclFile << ptclCode;
}

// Generate a PTcl description for galaxy without having to execute it
// If the galaxy is null, then a null StringList will be returned.
StringList ptclDescription(Galaxy* localGalaxy, const char* path,
			   int addHeader, const char* preamble,
			   const char* epilog) {
    StringList ptclCode;

    if (localGalaxy) {
	if (addHeader) {
	    // FIXME: Necessary to store returned StringList to ensure that
	    // temporary variables are handled correctly.  GNU 2.7 bug.
    	    StringList headerCode = ptclHeaderCode(localGalaxy);
    	    ptclCode << headerCode << "\n";
	}

	// Conditionally add the preamble
	if (preamble) ptclCode << preamble << "\n";

	// FIXME: Necessary to store returned StringList to ensure that
	// temporary variables are handled correctly.  GNU 2.7 bug.
	StringList galCode = ptclGalaxyCode(localGalaxy);
	ptclCode << galCode << "\n";

	// Conditionally add the epilog
	if (epilog) ptclCode << epilog << "\n";

	// Conditionally write the ptcl code to the file called path
	if (path && *path) writePtclCode(ptclCode, path);
    }

    return ptclCode;
}
