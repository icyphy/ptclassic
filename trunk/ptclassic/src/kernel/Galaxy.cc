/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:
 	3/19/90 - J. Buck
	Add method BlockList::blockWithName
	Use newConnection method in Galaxy::connect: we no longer have
	to know at compile time whether an argument is a PortHole or
	MultiPortHole.

	5/26/90 - I. Kuroda 
	Add method Galaxy::initState

	5/29/90 - J. Buck
	Change operator StringList to printVerbose.  Galaxy::connect
	is gone; it's now done by GenericPort class instead.

Methods for class Galaxy
***************************************************************************/
#include "Star.h"
#include "StringList.h"
#include "Galaxy.h"
#include "Block.h"


	////////////////////////////////////
	// Galaxy::printVerbose()
	////////////////////////////////////

StringList
Galaxy :: printVerbose () {
	StringList out;
	out = "GALAXY: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "Number of blocks: ";
	out += numberBlocks();
	out += "\n";
	out += printPorts("Galaxy");
	out += printStates("Galaxy");
	out += "Blocks in the Galaxy:----------------------------------\n";
	for(int i = numberBlocks(); i>0; i--)
		out += nextBlock().printVerbose();
	return out;
}


        ////////////////////////////////////
        // initState()
        ////////////////////////////////////

void Galaxy :: initState() {
        Block::initState();
        for(int i=blocks.size(); i>0; i--) {
                Block& b = blocks++;
                b.initState();}
}

        ////////////////////////////////////
        // Block* blockWithName()
        ////////////////////////////////////

Block *
BlockList::blockWithName (const char* ident) {
	Block *p;
	for (int i = size();i>0;i--) {
		p = (Block *)next();
		if (strcmp(ident,p->readName()) == 0)
			return p;
	}
	return NULL;
}
