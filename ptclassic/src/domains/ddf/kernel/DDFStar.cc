static const char file_id[] = "DDFStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DDFStar.h"
#include "Scheduler.h"
#include "DDFWormhole.h"

/*******************************************************************

	class DDFStar methods

********************************************************************/


// initialize DDF specific members
void DDFStar :: initialize() {
	DataFlowStar::initialize();
	waitPort = NULL;
	waitNum = 0;
	// special care for DDFSelf star (recursion)
	// adjust numberTokens
	if (isItSelf()) {
		// If error happens (from setup call), return
		if (Scheduler :: haltRequested()) return;
		BlockPortIter nextp(*this);
		for (int i = numberPorts(); i > 0; i--) {
			DDFPortHole& dp = *(DDFPortHole*) nextp++;
			int nTok = dp.imagePort->numberTokens;
			if (nTok > 1)
				dp.setDDFParams(nTok);
		}
	}
}

// set the waiting condition for execution
void DDFStar :: waitFor(PortHole& p, int num) {
	// check p is input, if not, error.
	if (p.isItInput()) {
		waitPort = &p;
		waitNum = num;
	} else {
		Error::mark(*this);
		Error::abortRun("waiting port should be input for ", 
			fullName());
	}
}
			
		
// The following is defined in DDFDomain.cc -- this forces that module
// to be included if any DDF stars are linked in.
extern const char DDFdomainName[];

const char* DDFStar :: domain () const { return DDFdomainName;}

int DDFStar :: isItSelf() { return FALSE ;}

// return NULL
DDFWormhole* DDFStar :: myWorm() { return NULL; }

const char* DDFStar :: readTypeName () { return "unspecified method";}

ISA_FUNC(DDFStar,DataFlowStar);
