static const char file_id[] = "SDFPTclTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer:  Jose Luis Pino
 Date of creation: 10/25/95

 This Target produces a PTcl graph corresponding to the SDF graph.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Scheduler.h"
#include "miscFuncs.h"
#include "SDFPTclTarget.h"
#include "GalIter.h"
#include "Star.h"
#include "DFPortHole.h"
#include "KnownTarget.h"
#include "pt_fstream.h"
#include "ProfileTimer.h"

// Constructor
SDFPTclTarget::SDFPTclTarget(const char* nam,const char* desc) :
SDFTarget(nam,desc), numIters(0) {
    addState(destDirectory.setState("directory", this,
	"$HOME/PTOLEMY_SYSTEMS/ptcl", "Directory to write to"));
}

Block* SDFPTclTarget::makeNew() const {
    LOG_NEW; return new SDFPTclTarget(name(), descriptor());
}

void  SDFPTclTarget::writeFiring(Star& s, int) {
    ProfileTimer firingTimer;
    if (! s.run()) SimControl::requestHalt();
    starProfiles.lookup(s)->addTime(firingTimer.elapsedCPUTime());
}

// FIXME: We really shouldn't have to do this.  There should not be a
// compileRun method for the scheduler, just a run.  In the compile
// mode the scheduler()->setStopTime(1) (to print out one iteration) -
// in the simulation mode the stop time should be set to the number of
// iterations.  For now, code duplication is done.
void SDFPTclTarget::setStopTime (double limit) {
    numIters = int(floor(limit + 0.001));
}

// Write out the header which resets the state of ptcl and declares the
// new galaxy name
static StringList ptclHeaderCode(Galaxy* localGalaxy) {
    StringList ptclCode = "reset\n";
    ptclCode << "newuniverse " << localGalaxy->name() << " "
			       << localGalaxy->domain() << " \n";
    return ptclCode;
}

// Generate the PTcl code describing the run-time estimates collected
// by repeated runs of the galaxy in the CGC domain 
StringList SDFPTclTarget::ptclExecTimeCode() {
    StringList ptclCode;

    // Iterate over the stars in the galaxy
    GalStarIter nextStar(*galaxy());
    Star* star;
    while ((star = nextStar++) != NULL) {
	StringList starName;
	starName << " \"" << star->fullName() << "\" ";
	StringList setstate;
	setstate << "\tsetstate" << starName;
	ptclCode << "\n\tstar" << starName << "CGCMultiInOut\n"
		 << setstate << "name " << star->className() << "\n"
		 << setstate << "execTime "
		 << starProfiles.lookup(*star)->avgTime() << "\n";
	StringList inputParams, outputParams;
	inputParams << setstate << "inputParams \"{ ";
	outputParams << setstate << "outputParams \"{ ";
	BlockPortIter nextPort(*star);
	DFPortHole* port;
	while ((port = (DFPortHole*)nextPort++) != NULL) {
	    (port->isItInput() ? inputParams: outputParams)
		<< "(" << port->numXfer() << "," << port->maxDelay() << ") ";
	}
	inputParams << "}\"\n"; 
	outputParams << "}\"\n";
	ptclCode << inputParams << outputParams;
    }
    ptclCode << '\n';

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
	    if (port->isItOutput())
		ptclCode << "\tconnect" << starName
			 << " \"output#" << portNumber[port->index()]
			 << "\" \"" << port->far()->parent()->fullName()
			 << "\" \"input#" << portNumber[port->far()->index()]
			 << "\"\n";
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

int SDFPTclTarget::run() {
    // Call haltRequested to process pending events and check for halt
    // If the user hit the DISMISS button in the run control panel,
    // then the universe referenced by galaxy() will return a null pointer
    int haltFlag = SimControl::haltRequested();
    if (! galaxy() || haltFlag) {
	Error::abortRun("SDF PTcl target has no galaxy to run");
	return FALSE;
    }

    // Generate run-time estimates by repeatedly running the galaxy
    starProfiles.set(*galaxy());
    int numItersSoFar = 0;
    while (numItersSoFar++ < numIters && !SimControl::haltRequested())
	scheduler()->compileRun();

    // Generate the PTcl code
    StringList ptclCode = ptclHeaderCode(galaxy());
    ptclCode << ptclExecTimeCode() << ptclGalaxyCode(galaxy());
    StringList ptclFileName;
    ptclFileName << destDirectory << "/" << galaxy()->name() << ".pt";
    writePtclCode(ptclCode, ptclFileName);

    return TRUE;
}

// Generate a PTcl description for galaxy without having to execute it
StringList ptclDescription(
		Galaxy* localGalaxy, int addHeader, const char* path) {
    StringList ptclCode;
    if (localGalaxy) {
	if (addHeader) {
	    // FIXME: Necessary to store returned StringList to ensure that
	    // temporary variables are handled correctly.  GNU 2.7 bug.
    	    StringList headerCode = ptclHeaderCode(localGalaxy);
    	    ptclCode << headerCode;
	}
	// FIXME: Necessary to store returned StringList to ensure that
	// temporary variables are handled correctly.  GNU 2.7 bug.
	StringList galCode = ptclGalaxyCode(localGalaxy);
	ptclCode << galCode;
    }

    if (path && *path) writePtclCode(ptclCode, path);

    return ptclCode;
}

void SDFPTclTarget::wrapup() {
};
