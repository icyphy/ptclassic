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

#include "miscFuncs.h"
#include "SDFPTclTarget.h"
#include "GalIter.h"
#include "Star.h"
#include "DFPortHole.h"
#include "KnownTarget.h"
#include "pt_fstream.h"

// Constructor
SDFPTclTarget::SDFPTclTarget(const char* nam,const char* desc)
: SDFTarget(nam,desc)
{
}

Block* SDFPTclTarget::makeNew() const {
	LOG_NEW; return new SDFPTclTarget(name(), descriptor());
}

// do not call the begin methods
// this prevents Tcl/Tk windows from popping up twice
void SDFPTclTarget::begin() {
}

int SDFPTclTarget::run() {
    StringList ptclCode;
    ptclCode << "reset\nnewuniverse " << galaxy()->name() << " SDF\n";
    // We don't specify the target
    // << "target Scheduler-Tester\n\ttargetparam foo \"\"\n";
    GalStarIter nextStar(*galaxy());
    Star* star;
    int nports = setPortIndices(*galaxy());
    int* portNumber = new int[nports];
    while ((star = nextStar++) != NULL) {
	StringList starName;
        starName << " \"" << star->fullName() << "\" ";
	StringList setstate;
	setstate << "\tsetstate" << starName;
	ptclCode << "\n\tstar" << starName << "CGCMultiInOut\n"
		 << setstate << "name " << star->className() << "\n"
		 << setstate << "execTime 1\n";
	int input=1;
	int output=1;
	StringList inputParams, outputParams;
	inputParams << setstate << "inputParams \"{ ";
	outputParams << setstate << "outputParams \"{ ";
	BlockPortIter nextPort(*star);
	DFPortHole* port;
	while ((port = (DFPortHole*)nextPort++) != NULL) {
	    portNumber[port->index()] = (port->isItInput()?input++:output++);
	    (port->isItInput()? inputParams: outputParams)
		<< "(" << port->numXfer() << "," << port->maxDelay() << ") ";
	}
	inputParams << "}\"\n"; 
	outputParams << "}\"\n";
	ptclCode << inputParams << outputParams;
    }
    ptclCode << '\n';
    nextStar.reset();
    while ((star = nextStar++) != NULL) {
	StringList starName;
	starName << " \"" << star->fullName() << "\" ";
	StringList setstate;
	setstate << "\tsetstate  " << starName << " ";
	BlockPortIter nextPort(*star);
	PortHole* port;
	while ((port = nextPort++) != NULL) {
	    if (port->isItOutput())
		ptclCode << "\tconnect" <<  starName
			 << " \"output#" << portNumber[port->index()]
			 << "\" \"" << port->far()->parent()->fullName()
			 << "\" \"input#" << portNumber[port->far()->index()]
			 << "\"\n";
	}
    }
    StringList ptclFileName;
    const char* path = expandPathName("~/PTOLEMY_SYSTEMS/ptcl/");
    delete [] path;
    ptclFileName << path << galaxy()->name() << ".pt";
    pt_ofstream ptclFile(ptclFileName);
    ptclFile << ptclCode;
    return TRUE;
}

void SDFPTclTarget::wrapup() {
};

static SDFPTclTarget SDFPTclTargetProto("SDF-to-PTcl",
					"Generate CG PTcl Equivalent");

static KnownTarget entry(SDFPTclTargetProto,"SDF-to-PTcl");



