#include "Block.h"
#include "StringList.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90
 Revisions:
	3/19/90 - J. Buck
		  add method Block::portWithName
		  returns a pointer to the PortHole with the given name

 Routines implementing class Block methods

**************************************************************************/

char* Block :: readFullName ()
{
	StringList out;
	if(blockIamIn != NULL) {
	   out = blockIamIn->readFullName();
	   out += ".";
	   out += name;
	   return out;
	} else {
	   return name;
	}
}


Block :: operator char* ()
{
	StringList out;
	out = "Block: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";

	out += "Ports in the block:\n";
	for(int i = numberPorts(); i>0; i--)
		out += nextPort().operator char* ();

	return out;
}

void Block :: initialize()
{
	// Call initialize() for each PortHole
	for(int i = numberPorts(); i>0; i--)
		nextPort().initialize();
}

// This method returns a PortHole corresponding to the given name.
// If the name refers to a MultiPortHole, a new PortHole is created.
// The real port is always returned (no need to check for aliases).
PortHole *
Block::portWithName (char *name) {
	// If the MultiPortHole name matches, return that
	// (this will create a new connection)
	if (saveMPH && strcmp (name,saveMPH->readName()) == 0)
		return &saveMPH->newConnection();
	// Not found, search the port list
	else for (int i=numberPorts(); i>0; i--) {
		PortHole *pt;
		pt = &nextPort();
		if (strcmp (name, pt->readName()) == 0)
			return &(pt->newConnection());
	}
	// Still not found, return NULL
	return NULL;
}
