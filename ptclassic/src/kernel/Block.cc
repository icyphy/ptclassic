static const char file_id[] = "Block.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "Block.h"
#include "StringList.h"
#include "ConstIters.h"
#include "Error.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.
		       
 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90

Routines implementing class Block methods
 
**************************************************************************/

// constructor

Block :: Block() {}

// alternate constructor
Block :: Block(const char* n,Block* p,const char* d) : NamedObj(n,p,d) {}

// print all port names in the block, omitting hidden ports

StringList
Block :: printPorts (const char* type, int verbose) const {
	StringList out;
// first the ports
	if (numberPorts()) {
		if (verbose)
			out << "Ports in the " << type << ":\n";
		else out << "Ports:\n";
		CBlockPortIter next(*this);
		const PortHole* p;
		while ((p = next++) != 0)
			if (!hidden(*p)) out += p->print(verbose);
	}
// next the multiports
	if (numberMPHs()) {
		if (verbose)
			out << "MultiPortHoles in the " << type << "\n";
		else
			out << "MultiPorts:\n";
		CBlockMPHIter next(*this);
		const MultiPortHole* mp;
		while ((mp = next++) != 0)
			if (!hidden(*mp)) out += mp->print(verbose);
	}
	return out;
}

StringList
Block :: printStates (const char* type, int verbose) const {
        StringList out;
	if (numberStates() == 0) return out;
	if (verbose) 
		out << "States in the " << type << ":\n";
	else
		out << "States:\n";
	const State* s;
	CBlockStateIter next(*this);
	while ((s = next++) != 0)
		out += s->print(verbose);
        return out;
}

StringList
Block :: print(int verbose) const
{
	StringList out;
	return out << "Block: " << fullName() << "\n"
		   << "Descriptor: " << descriptor() << "\n"
		   << printPorts("block",verbose);
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
	setup();
}

// This method returns a GenericPort corresponding to the given name.
GenericPort *
Block::genPortWithName (const char* name) {
	GenericPort* g;
	BlockGenPortIter gpi(*this);
	while ((g = gpi++) != 0) {
		if (strcmp (name, g->name()) == 0)
			return g;
	}
	// Not found, return NULL
	return NULL;
}

// This method returns a PortHole corresponding to the given name.
// If the name refers to a MultiPortHole, a new PortHole is created.
// The real port is always returned (no need to check for aliases).
PortHole *
Block::portWithName (const char* name) {
	GenericPort* g;
	BlockGenPortIter gpi(*this);
	while ((g = gpi++) != 0) {
		if (strcmp (name, g->name()) == 0)
			return &(g->newConnection());
	}
	// Not found, return NULL
	return NULL;
}

// Return the matching MultiPortHole.
MultiPortHole* Block::multiPortWithName(const char* name) {
	MultiPortHole* m;
	BlockMPHIter mpi(*this);
	while ((m = mpi++) != 0) {
		if (strcmp (name, m->name()) == 0)
			return m;
	}
	return NULL;
}

// The following function is an error catcher -- it is called if
// a star or galaxy in the known list hasn't redefined the makeNew
// method.  It would be nice if that could be caught at compile time.
Block* Block::makeNew() const {
	Error::abortRun ("The star or galaxy \"", name(),
		      "\" doesn't implement the makeNew method");
	return 0;
}

// The following function is also an error catcher.
int Block::run() {
	Error::abortRun(*this, "Objects of class ", className(),
			" are not runnable!");
	return FALSE;
}

Block* Block::clone() const {
	Block* b = makeNew();
	return b ? b->copyStates(*this) : 0;
}

Block* Block::copyStates(const Block& src) {
	CBlockStateIter nexts(src);
	BlockStateIter nextd(*this);
	const State* srcStatePtr;
	State *destStatePtr;
	while ((srcStatePtr = nexts++) != 0 && (destStatePtr = nextd++) != 0)
		destStatePtr->setInitValue(srcStatePtr->initValue());
	return this;
}

// This one matters only for galaxies... try the plasma type first;
// if not set, use the defined type.

static DataType trueType(const PortHole* p) {
	DataType d = p->resolvedType();
	return d ? d : p->type();
}

// Return the names of the ports within the block.  Omit hidden ports.
int
Block::portNames (const char** names, const char** types,
		  int* io, int nMax) const {
	int n = numberPorts();
	int count = 0;
	if (n > nMax) n = nMax;
	CBlockPortIter next(*this);
	for (int i = n; i>0; i--) {
		const PortHole* p = next++;
		if (hidden(*p)) continue;
		*names++ = p->name();
		*types++ = trueType(p);
		*io++ = p->isItOutput();
		count++;
	}
	return count;
}

// Return the names of the multiports within the block.  Omit hidden ports.
int
Block::multiPortNames (const char** names, const char** types,
		       int* io, int nMax) const {
	int n = numberMPHs();
	int count = 0;
	if (n > nMax) n = nMax;
	CBlockMPHIter next(*this);
	for (int i = n; i>0; i--) {
		const MultiPortHole* p = next++;
		if (hidden(*p)) continue;
		*names++ = p->name();
		*types++ = p->type();
		*io++ = p->isItOutput();
		count++;
	}
	return count;
}

State *
Block::stateWithName(const char* name) {
	State* s;
	BlockStateIter next(*this);
	while ((s = next++) != 0) {
		if(strcmp(name,s->name()) == 0)
			return s;
	}
	return NULL;
}

int Block :: setState(const char* stateName, const char* expression) {
	State* s = stateWithName(stateName);
	if (!s) return FALSE;
	s->setInitValue(expression);
	return TRUE;
}

ISA_FUNC(Block,NamedObj);

class Star;
class Galaxy;

// Error-catchers; these should be redefined.
// return types are dummies to shut up compiler warnings.
static void* bomb(const Block* b, const char* type) {
	Error::error (*b, "is not a", type);
	exit (1);
	return 0;
}

const Star& Block::asStar () const {
	return *(const Star*)bomb(this, "Star");
}

Star& Block::asStar () {
	return *(Star*)bomb(this, "Star");
}

const Galaxy& Block::asGalaxy () const {
	return *(const Galaxy*)bomb(this, "Galaxy");
}

Galaxy& Block::asGalaxy() {
	return *(Galaxy*)bomb(this, "Galaxy");
}

// small virtual functions
void Block::setup () {}

void Block::wrapup () {}

const char* Block::className() const { return "Block";}

Block& Block::setBlock(const char* s, Block* parent) {
	setNameParent (s, parent);
	return *this;
}

int Block::isItAtomic () const { return TRUE;}

int Block::isItWormhole () const { return FALSE;}

void Block::initState () { states.initElements();}

Scheduler* Block :: scheduler() const { return parent()->scheduler() ;}

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
