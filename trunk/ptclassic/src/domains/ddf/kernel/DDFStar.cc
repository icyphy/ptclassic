/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

*******************************************************************/

#include "DDFStar.h"

extern Error errorHandler;

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

// set the waiting condition for execution
void DDFStar :: wait(PortHole& p, int num = 1) {
	// check p is input, if not, error.
	if (p.isItInput()) {
		waitPort = &p;
		waitNum = num;
	} else {
		errorHandler.error("waiting port should be input for ", 
			readFullName());
		exit(1);
	}
}
			
		
// The following is defined in DDFDomain.cc -- this forces that module
// to be included if any DDF stars are linked in.
extern const char DDFdomainName[];

const char* DDFStar :: domain () const { return DDFdomainName;}

int DDFStar :: isItSelf() { return FALSE ;}
