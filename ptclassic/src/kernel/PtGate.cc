static const char file_id[] = "PtGate.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1993 The Regents of the University of California.
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

 Programmer:  J. T. Buck and T. M. Parks
 Date of creation: 6/14/93

This file defines classes that support multi-threading in the kernel.

**************************************************************************/

#include "PtGate.h"
#include "logNew.h"
#include "Error.h"

// dummy destructor for PtGate
PtGate :: ~PtGate() {}

// master list of all GateKeepers
GateKeeper* GateKeeper::listHead = 0;

// constructor adds object to master list.  The argument is a
// reference to a PtGate pointer; this pointer is modified when
// the GateKeeper "enables" or "disables" the PtGate pointer.
GateKeeper::GateKeeper(PtGate*& gateToKeep)
: gate(gateToKeep), next(listHead), valid(GateKeeper_cookie)
{
	listHead = this;
}

// destructor: delete gate, remove from list.
GateKeeper::~GateKeeper () {
	// since destructors are called in reverse order
	// of constructors, the following "if" is almost always
	// true, so we rarely do the expensive search.
	if (listHead == this) listHead = next;
	else {
		// code here to search for and zap "this" from the list
		// started by listHead
		GateKeeper *p = listHead;
		while (p->next && p->next != this)
			p = p->next;
		if (p->next == 0) Error::error("GateKeeper error!");
		else p->next = p->next->next;
	}
	LOG_DEL; delete gate;
	gate = 0;
}

// create a PtGate for each GateKeeper.
void GateKeeper::enableAll(const PtGate& master) {
	for (GateKeeper *gkp = listHead; gkp; gkp = gkp->next) {
		LOG_DEL; delete gkp->gate;
		gkp->gate = master.clone();
	}
}

// destroy the PtGates for all GateKeepers.
void GateKeeper::disableAll() {
	for (GateKeeper *gkp = listHead; gkp; gkp = gkp->next) {
		LOG_DEL; delete gkp->gate;
		gkp->gate = 0;
	}
}
