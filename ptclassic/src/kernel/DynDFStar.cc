static const char file_id[] = "DynDFStar.cc";
/******************************************************************
Version identification:
$Id$

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
							COPYRIGHTENDKEY

 Programmer:  J. T. Buck

 DynDFStar (dynamic dataflow star) is the base class for dataflow stars
 that have dynamic behavior.

*******************************************************************/

#include "DynDFStar.h"
#include "DFPortHole.h"

// small DynDFStar functions.

// isA
ISA_FUNC(DynDFStar,DataFlowStar);

// constructor
DynDFStar::DynDFStar() : myWaitPort(0), myWaitCount(0), sdfFlag(0) {}

DFPortHole* DynDFStar::waitPort() const { return myWaitPort;}

int DynDFStar::waitTokens() const { return myWaitCount;}

int DynDFStar::isSDF() const { return sdfFlag;}

void DynDFStar :: initialize() {
	DataFlowStar::initialize();
	DFStarPortIter nextp(*this);
	DFPortHole *p;
	sdfFlag = TRUE;
	while ((p = nextp++) != 0) {
		if (p->isDynamic()) sdfFlag = FALSE;
	}
}

void DynDFStar :: waitFor(DFPortHole& port,int howMany) {
	myWaitPort = &port;
	myWaitCount = howMany;
}

void DynDFStar :: clearWaitPort() {
	myWaitPort = 0;
	myWaitCount = 0;
}
