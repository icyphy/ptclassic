/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 12/15/89

*******************************************************************/
#include "Star.h"
#include "StringList.h"

/*******************************************************************

	class Star methods

********************************************************************/

Star :: operator StringList () {
	StringList out;
	out = "Star: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += printPorts ("star");
	return out;
}

/*******************************************************************

	class SDFStar methods

********************************************************************/


void SDFStar :: beforeGo()
{
        for(int i = numberPorts(); i>0; i--) {
                SDFPortHole& port = (SDFPortHole&)nextPort();
                port.beforeGo();
                }
}
 
void SDFStar :: afterGo()
{
        for(int i = numberPorts(); i>0; i--) {
                SDFPortHole& port = (SDFPortHole&)nextPort();
                port.afterGo();
                }
}

// Redefine method setting internal data in the Block
// so that various SDF-specific initilizations can be performed.
// If the parent pointer is not provied, it defaults to NULL
Block& SDFStar :: setBlock(char* s, Block* parent = NULL) {
	// First invoke the generic setBlock
	Block::setBlock(s,parent);

	// Then perform SDF-specific initializations
	repetitions = 0;
	noTimes = 0;

	return *this;
}
