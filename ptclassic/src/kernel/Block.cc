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

Routines implementing class Block methods
 
**************************************************************************/

extern Error errorHandler;

StringList
Block :: printPorts (const char* type) const {
	StringList out;
// first the ports
	if (numberPorts()) {
		out += "Ports in the ";
		out += type;
		out += ":\n";
		BlockPortIter next(*this);
		PortHole* p;
		while ((p = next++) != 0)
			out += p->printVerbose();
	}
// next the multiports
	if (numberMPHs()) {
		out += "MultiPortHoles in the ";
		out += type;
		out += "\n";
		BlockMPHIter next(*this);
		MultiPortHole* mp;
		while ((mp = next++) != 0)
			out += mp->printVerbose();
	}
	return out;
}

StringList
Block :: printStates (const char* type) const {
        StringList out;
	if (numberStates() == 0) return out;
        out += "States in the ";
        out += type;
        out += ":\n";
	State* s;
	BlockStateIter next(*this);
	while ((s = next++) != 0)
		out += s->printVerbose();
        return out;
}

StringList
Block :: printVerbose () const
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
	// Call initialize() for each PortHole, if not of Galaxy
	// also do MultiPortHoles
	if (isItAtomic()) {
		GenericPort* g;
		BlockGenPortIter next(*this);
		while ((g = next++) != 0) g->initialize();
	}
        // initialize States
        initState();
	// call user-specified initialization
	start();
}

// This method returns a PortHole corresponding to the given name.
// If the name refers to a MultiPortHole, a new PortHole is created.
// The real port is always returned (no need to check for aliases).
PortHole *
Block::portWithName (const char* name) const {
	GenericPort* g;
	BlockGenPortIter gpi(*this);
	while ((g = gpi++) != 0) {
		if (strcmp (name, g->readName()) == 0)
			return &(g->newConnection());
	}
	// Not found, return NULL
	return NULL;
}

// Return the matching MultiPortHole.
MultiPortHole* Block::multiPortWithName(const char* name) const {
	MultiPortHole* m;
	BlockMPHIter mpi(*this);
	while ((m = mpi++) != 0) {
		if (strcmp (name, m->readName()) == 0)
			return m;
	}
	return NULL;
}

// The following function is an error catcher -- it is called if
// a star or galaxy in the known list hasn't redefined the clone
// method.
Block* Block::clone() const {
	StringList msg = "The star or galaxy \"";
	msg += readName();
	msg += "\" doesn't implement the \"clone\" method!\n";
	errorHandler.error(msg);
	return 0;
}

// Return the names of the ports within the block.
int
Block::portNames (const char** names, int* io, int nMax) const {
	int n = numberPorts();
	if (n > nMax) n = nMax;
	BlockPortIter next(*this);
	for (int i = n; i>0; i--) {
		PortHole& p = *next++;
		*names++ = p.readName();
		*io++ = p.isItOutput();
	}
	return ports.size();
}

// Return the names of the multiports within the block.
int
Block::multiPortNames (const char** names, int* io, int nMax) const {
	int n = numberMPHs();
	if (n > nMax) n = nMax;
	BlockMPHIter next(*this);
	for (int i = n; i>0; i--) {
		MultiPortHole& p = *next++;
		*names++ = p.readName();
		*io++ = p.isItOutput();
	}
	return multiports.size();
}

State *
Block::stateWithName(const char* name) const {
	State* s;
	BlockStateIter next(*this);
	while ((s = next++) != 0) {
		if(strcmp(name,s->readName()) == 0)
			return s;
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

// small virtual functions
void Block::start () {}

void Block::wrapup () {}

Block& Block::setBlock(const char* s, Block* parent) {
	setNameParent (s, parent);
	return *this;
}

int Block::isItAtomic () const { return TRUE;}

int Block::isItWormhole () const { return FALSE;}

void Block::initState () { states.initElements();}

Scheduler* Block :: mySched() const { return parent()->mySched() ;}

// destructor isn't really do-nothing because it calls destructors
// for members
Block::~Block () {}


// return my domain.  Should this be an error (if noone has redefined
// this?)
const char* Block :: domain () const { return "UNKNOWN";}

// "next" method for GenericPort iterator.  First return portholes,
// then multiportholes.

GenericPort* BlockGenPortIter :: next () {
	GenericPort* g = (GenericPort*)ListIter::next();
	if (g == 0 && !usedP) {
		reconnect(myBlock.multiports);
		g = (GenericPort*)ListIter::next();
		usedP = 1;
	}
	return g;
}
