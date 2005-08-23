static const char file_id[] = "Block.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "Block.h"
#include "StringList.h"
#include "ConstIters.h"
#include "Error.h"
#include "SimControl.h"
#include "StringList.h"
#include "Scope.h"

/**************************************************************************
Version identification:
@(#)Block.cc	2.44	1997

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
		       
 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90

Routines implementing class Block methods
 
**************************************************************************/

#include "KnownBlock.h"

// constructor

Block :: Block() : pTarget(0), scp(0){}

// alternate constructor
Block::Block(const char* n,Block* p,const char* d):NamedObj(n,p,d),
    pTarget(0),scp(0){}

int Block :: setTarget(Target* t) {
    if (t) {
	pTarget = t;
	return TRUE;
    }
    else {
	Error::abortRun(*this,"Target pointer set to null");
	return FALSE;
    }
}

Target* Block :: target() const {
    return pTarget;
}

// print all port names in the block, omitting hidden ports

StringList Block :: printPorts (const char* type, int verbose) const {
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

// Return the block which defines the lexical scope of our states
Scope* Block::scope() const{
    return scp;
}
 
// Set the lexical scope for our states
void Block::setScope(Scope* s) { scp = s; }
 
// Return full name using scoping hierarchy.

/*virtual*/ StringList Block :: fullName () const
{
    StringList out;
    if(scope() != NULL) {
        out << scope()->fullName() << "." << name();
    } else {
        out << NamedObj::fullName();
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
		   << printPorts("block",verbose)
		   << printStates("block",verbose);
}

void Block :: initialize()
{
	// Call initialize() for each PortHole, if not of Galaxy
	// also do MultiPortHoles
	if (isItAtomic()) {
		ports.initElements();
		multiports.initElements();
	}
        // initialize States
	states.initElements();
	// call user-specified initialization (unless error occurred)
	if (!SimControl::haltRequested())
		setup();
}

// This method returns a GenericPort corresponding to the given name.
GenericPort* Block::genPortWithName (const char* name) {
	GenericPort* g = ports.portWithName(name);
	if (!g) g = multiports.multiPortWithName(name);
	return g;
}

// Const version
const GenericPort* Block::genPortWithName (const char* name) const {
	const GenericPort* g = ports.portWithName(name);
	if (!g) g = multiports.multiPortWithName(name);
	return g;
}

// This method returns a PortHole corresponding to the given name.
// If the name refers to a MultiPortHole, a new PortHole is created.
// The real port is always returned (no need to check for aliases).
// This one can't have a const version because if the name specifies
// a multiporthole, then a new port is created.
PortHole* Block::portWithName (const char* name) {
	GenericPort* g = genPortWithName(name);
	if (!g) return 0;
	return &(g->newConnection());
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
	LOG_NEW; Block* b = makeNew();
	if (b) {
		if(target()) b->setTarget(target());
		b->setNameParent(hashstring(name()),parent());
		b->setDescriptor(hashstring(descriptor()));
		b->copyStates(*this);
		if (numberMPHs() > 0) {
			CBlockMPHIter piter(*this);
			const MultiPortHole* p;
			while ((p = piter++) != 0) {
				MultiPortHole* cP = b->multiPortWithName(p->name());
				for (int i = p->numberPorts(); i > 0; i--) {
					cP->newPort();
				}	
			}
		}
		b->flags = flags;
	}
	return b;
}

Block* Block::copyStates(const Block& src) {
	CBlockStateIter nexts(src);
	BlockStateIter nextd(*this);
	const State* srcStatePtr;
	State *destStatePtr = (State *)NULL;
	while ((srcStatePtr = nexts++) != 0 && (destStatePtr = nextd++) != 0) {
		destStatePtr->setInitValue(srcStatePtr->initValue());
		StringList cVal;
		cVal << srcStatePtr->currentValue();
		destStatePtr->setCurrentValue(hashstring(cVal));
	}
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
	return states.stateWithName(name);
}

int Block :: setState(const char* stateName, const char* expression) {
	State* s = stateWithName(stateName);
	if (!s) return FALSE;
	s->setInitValue(hashstring(expression));
	return TRUE;
}

ISA_FUNC(Block,NamedObj);

class Star;
class Galaxy;

// Error-catchers; these should be redefined.
// return types are dummies to shut up compiler warnings.
static void* bomb(const Block* b, const char* type) {
	Error::error (*b, "is not a ", type);
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

void Block::preinitialize () {}

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

int Block::isItCluster () const { return FALSE;}

void Block::initState () { states.initElements();}

void Block::initPorts () {
  if(isItAtomic()) {
    ports.initElements();
    multiports.initElements();
  }
}

// Reset the buffers in the PortHoles but still keep the
// infrastructure of the PortHoles.
void Block::resetPortBuffers()  {
	PortListIter nextp(ports);
	PortHole* p;
	while ((p = nextp++) != NULL) p->resetBufferValues();
}

// return the scheduler for the Block.  Block derived classes that
// contain schedulers will redefine this method.
Scheduler* Block :: scheduler() const {
	Block* p = parent();
	if (p) return p->scheduler();
	else return 0;
}

// destructor isn't really do-nothing because it calls destructors
// for members
Block::~Block () {
    if (scope()) scope()->remove(*this);
}


// return my domain.  Should this be an error (if no one has redefined
// this?)
const char* Block :: domain () const { return "UNKNOWN";}

RegisterBlock:: RegisterBlock(Block& prototype, const char* classname) {
        KnownBlock::addEntry (prototype, classname, 0, NULL);
}

