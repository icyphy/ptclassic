static const char file_id[] = "VHDLBPortHole.cc";
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

 Programmer: Edward A. Lee and Soonhoi Ha

 These classes are portholes for stars that generate VHDLB code.  

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLBPortHole.h"
#include "VHDLBGeodesic.h"
#include "Error.h"

// setup ForkDests
void VHDLBPortHole :: setupForkDests() {
	SequentialList temp;
	temp.initialize();

	VHDLBForkDestIter next(this);
	VHDLBPortHole *outp, *inp;
	while ((outp = next++) != 0) {
		inp = outp->realFarPort();
		if (inp->fork()) temp.put(inp);
	}

	ListIter nextPort(temp);
	while ((inp = (VHDLBPortHole*) nextPort++) != 0) {
		inp->setupForkDests();
		forkDests.remove(inp->far());
		VHDLBForkDestIter realNext(inp);
		while ((outp = realNext++) != 0)
			forkDests.put(outp);
	}
}

// Need modification if we allow wormholes!!
VHDLBPortHole* VHDLBPortHole :: realFarPort() {
	VHDLBPortHole* p = (VHDLBPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

// const version: hate this duplication.
const VHDLBPortHole* VHDLBPortHole :: realFarPort() const {
	const VHDLBPortHole* p = (const VHDLBPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

void VHDLBPortHole :: setGeoName(char* n) {
	geo().setBufName(n);
}

StringList VHDLBPortHole :: getGeoReference() const {
	GenericPort *terminal = aliasFrom();
	StringList out;
	int delays;
	if(terminal) {
		// porthole is aliased to a galaxy porthole.
		out = terminal->name();
	} else {
		// Translate aliases
		VHDLBGeodesic& geodesic = ((VHDLBPortHole&)realPort()).geo();
		out = geodesic.getBufName();
		// Figure out delays for references to input portholes only
		if(isItInput()) {
		    delays = geodesic.numInit();
		    if (delays > 0) {
			out += "@";
			out += delays;
		    }
		}
	}
	return out;
}

const char* VHDLBPortHole :: getGeoName() const {
	return ((VHDLBPortHole&)realPort()).geo().getBufName();
}

// Dummy
int MultiVHDLBPort :: someFunc() { return 1; }

int InVHDLBPort :: isItInput() const { return TRUE; }
int OutVHDLBPort :: isItOutput() const { return TRUE; }
int MultiInVHDLBPort :: isItInput() const { return TRUE; }
int MultiOutVHDLBPort :: isItOutput() const { return TRUE; }

PortHole& MultiInVHDLBPort :: newPort () {
        LOG_NEW; InVHDLBPort* p = new InVHDLBPort;
        p->setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(*p);
        return installPort(*p);
}

PortHole& MultiOutVHDLBPort :: newPort () {
	LOG_NEW; OutVHDLBPort* p = new OutVHDLBPort;
        p->setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(*p);
        return installPort(*p);
}
