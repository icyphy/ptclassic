static const char file_id[] = "SDFPTclTarget.cc";
/******************************************************************
Version identification:
@(#)SDFPTclTarget.cc	1.16 9/2/96

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

 Programmer:  Jose Luis Pino
 Date of creation: 10/25/95

 This Target produces a PTcl graph corresponding to the SDF graph.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

// Ptolemy kernel files
#include "miscFuncs.h"
#include "Scheduler.h"
#include "Star.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "DFPortHole.h"
#include "KnownTarget.h"
#include "ProfileTimer.h"

// Ptolemy ptcl files
#include "ptclDescription.h"

// Ptolemy domain files
#include "SDFTarget.h"
#include "SDFPTclTarget.h"

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
    StringList execTimeCode = ptclExecTimeCode();
    StringList ptclFileName;
    ptclFileName << destDirectory << "/" << galaxy()->name() << ".pt";
    ptclDescription(galaxy(), ptclFileName, TRUE, execTimeCode);

    return TRUE;
}

void SDFPTclTarget::wrapup() {
};
