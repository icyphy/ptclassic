/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

*******************************************************************/

#include "DDFStar.h"
#include "Scheduler.h"

/*******************************************************************

	class DDFStar methods

********************************************************************/



// Redefine method setting internal data in the Block
// so that various DDF-specific initilizations can be performed.
// If the parent pointer is not provied, it defaults to NULL
Block& DDFStar :: setBlock(char* s, Block* parent = NULL) {
	// First invoke the generic setBlock
	Block::setBlock(s,parent);

	// Then perform DDF-specific initializations
	waitPort = NULL;
	waitNum = 0;

	return *this;
}

// initialize DDF specific members
void DDFStar :: prepareForScheduling() {
	waitPort = NULL;
	waitNum = 0;
	// special care for DDFSelf star (recursion)
	// adjust numberTokens
	if (isItSelf()) {
		start();	// define inside Galaxy
		// If error happens, return
		if (Scheduler :: haltRequested()) return;
		BlockPortIter nextp(*this);
		for (int i = numberPorts(); i > 0; i--) {
			DDFPortHole& dp = (DDFPortHole&) *nextp++;
			int nTok = dp.imagePort->numberTokens;
			if (nTok > 1)
				dp.setDDFParams(nTok);
		}
	}
}

// set the waiting condition for execution
void DDFStar :: waitFor(PortHole& p, int num = 1) {
	// check p is input, if not, error.
	if (p.isItInput()) {
		waitPort = &p;
		waitNum = num;
	} else {
		Error::mark(*this);
		Error::abortRun("waiting port should be input for ", 
			readFullName());
	}
}
			
		
// The following is defined in DDFDomain.cc -- this forces that module
// to be included if any DDF stars are linked in.
extern const char DDFdomainName[];

const char* DDFStar :: domain () const { return DDFdomainName;}

int DDFStar :: isItSelf() { return FALSE ;}

const char* DDFStar :: readClassName () { return "unspecified method";}
