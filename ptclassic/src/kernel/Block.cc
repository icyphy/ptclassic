#include "Block.h"
#include "StringList.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:

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
