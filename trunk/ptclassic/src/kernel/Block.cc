#include "Block.h"
#include "StringList.h"
#include "Output.h"
#include <stream.h>
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
	5/26/90 - I. Kuroda
		  add method Block::printStates
		  add method Block::stateWithName

Routines implementing class Block methods
 
**************************************************************************/

extern Error errorHandler;
     
StringList
Block :: printPorts (const char* type) {
	StringList out;
// first the ports
	
	out += "Ports in the ";
	out += type;
	out += ":\n";
	for(int i = numberPorts(); i>0; i--)
		out += nextPort().printVerbose();
// Now do the multiports
	int nmph = multiports.size();
	if (nmph) {
		out += "MultiPortHoles in the ";
		out += type;
		out += "\n";
		for (i = nmph; i> 0; i--)
			out += multiports++.printVerbose();
	}
	return out;
}

StringList
Block :: printStates (const char* type) {
        StringList out;
	if (numberStates() == 0) return out;
        out += "States in the ";
        out += type;
        out += " ";
        out += readName();
        out += ":\n";
        for(int i = numberStates(); i>0; i--)
                out += nextState().printVerbose();
        return out;
}

StringList
Block :: printVerbose ()
{
	StringList out;
	out = "Block: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += printPorts("block");
	return out;
}

void Block :: initialize()
{
	// Call initialize() for each PortHole
	for(int i = numberPorts(); i>0; i--)
		nextPort().initialize();
        // initialize States
        initState();
}

// This method returns a PortHole corresponding to the given name.
// If the name refers to a MultiPortHole, a new PortHole is created.
// The real port is always returned (no need to check for aliases).
PortHole *
Block::portWithName (const char* name) {

	// If the MultiPortHole name matches, return that
	// (this will create a new connection)
	MultiPortHole *mpt = multiPortWithName (name);
	if (mpt) return &(mpt->newConnection());

	// Not found, search the port list
	for (int i=numberPorts(); i>0; i--) {
		PortHole& pt = ports++;
		if (strcmp (name, pt.readName()) == 0)
			return &(pt.newConnection());
	}
	// Still not found, return NULL
	return NULL;
}

// Return the matching MultiPortHole.
MultiPortHole* Block::multiPortWithName(const char* name) {
	for (int i = multiports.size(); i>0; i--) {
		MultiPortHole& mpt = multiports++;
		if (strcmp (name, mpt.readName()) == 0)
			return &mpt;
	}
	return NULL;
}

// The following function is an error catcher -- it is called if
// a star or galaxy in the known list hasn't redefined the clone
// method.
Block* Block::clone() {
	StringList msg = "The star or galaxy \"";
	msg += readName();
	msg += "\" doesn't implement the \"clone\" method!\n";
	errorHandler.error(msg);
	return 0;
}

// Return the names of the ports within the block.
int
Block::portNames (const char** names, int* io, int nMax) {
	int n = ports.size();
	if (n > nMax) n = nMax;
	for (int i = n; i>0; i--) {
		PortHole& p = ports++;
		*names++ = p.readName();
		*io++ = p.isItOutput();
	}
	return ports.size();
}

// Return the names of the multiports within the block.
int
Block::multiPortNames (const char** names, int* io, int nMax) {
	int n = multiports.size();
	if (n > nMax) n = nMax;
	for (int i = n; i>0; i--) {
		MultiPortHole& p = multiports++;
		*names++ = p.readName();
		*io++ = p.isItOutput();
	}
	return multiports.size();
}


State *
Block::stateWithName(const char* name)
        {       State* sp;
                for(int i  = states.size(); i>0; i--) {
                        sp = &nextState();
                        if(strcmp(name,sp->readName()) == 0)
                                return  sp;
                        }
                return NULL;
        }

class Star;
class Galaxy;

// Error-catchers; these should be redefined.

Star&
Block::asStar () const {
	errorHandler.error (readFullName(), " is not a Star!");
	exit (1);
}

Galaxy&
Block::asGalaxy () const {
	errorHandler.error (readFullName(), " is not a Galaxy!");
	exit (1);
}
