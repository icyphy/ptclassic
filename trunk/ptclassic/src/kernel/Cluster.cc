static const char file_id[] = "Nebula.cc";
/******************************************************************
Version identification:
 $Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmers: J. L. Pino & T. M. Parks
 Date of creation: 6/10/94

 A Nebula is an executable Star, a sort of light-weight Wormhole.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Nebula.h"
#include "Star.h"
#include "Galaxy.h"
#include "SimControl.h"
#include "Scheduler.h"
#include "Geodesic.h"
#include "Plasma.h"

extern int setPortIndices(Galaxy&);

Nebula::Nebula(Star& self) : selfStar(self), master(NULL),
sched(0) {};

/*virtual*/ Nebula::~Nebula() {
    LOG_DEL; delete sched;
}

StringList Nebula::displaySchedule() {
    StringList schedule;
    if (isNebulaAtomic()) {
	schedule = star().fullName();
	return schedule;
    }
    else if (!innerSched())
	schedule << "ERROR NO SCHEDULE MEMBER IN NEBULA:"
		 << star().fullName() << "\n";
    else
	schedule << "/* Schedule for " << star().fullName() << " */\n"
		 << innerSched()->displaySchedule() << "\n";
    // If we get here nebula is Non-atomic
    FatNebulaIter next(*this);
    Nebula* fatNebula;
    while ((fatNebula = next++) != 0)
	schedule << fatNebula->displaySchedule();
    return schedule;
}

void Nebula::setInnerSched(Scheduler* s) {
    if (s == NULL) {
	Error::abortRun(star(),
			"Nebula::setInnerSched sent a NULL scheduler pointer");
	return;
    }
    LOG_DEL; delete sched;
    sched = s;
    sched.setGalaxy(gal);
}

void Nebula::setMasterBlock(Block* m,PortHole** newPorts) {
    if (master) {
	Error::abortRun("Can not run setMaster twice on same Nebula");
	return;
    }
    master = m;
    if (master->isItAtomic()) {
	star().setName(master->name());
	// Add the star's ports to the internal galaxy,
	// but do not change their parents.
	BlockPortIter starPorts(*master);
	PortHole* port;
	while ((port = starPorts++) != NULL) {
	    if (port->far()->parent() == (Block*)&master)
                // Exclude self-loop connections.
		continue;
	    
	    else if (port->atBoundary())
		// Exclude wormhole connections.
		continue;
	    
	    else {
		PortHole* clonedPort = clonePort(port);
		star().addPort(*clonedPort);
		clonedPort->setNameParent(port->name(),
					  &star());
		if(newPorts)
		    newPorts[clonedPort.asNebulaPort()->
			     real().index()] = clonedPort;
	    }
	}
    }
    else {
	// Set the name, leave parent unchanged
	StringList name;
	name << master->name() << "_Nebula";
	const char* pname = hashstring(name);
	star().setName(pname);
	
	Galaxy* g = (Galaxy*) m;
	int isTopNebula = ! (int) newPorts;
	int nports = 0;
	if (isTopNebula) {
	    nports = setPortIndices(*g);
	    LOG_NEW; newPorts = new PortHole*[nports];
	    for (int i = 0; i < nports; i++) newPorts[i] = 0;
	}

	addGalaxy(g,newPorts);

	// now connect up the Nebula ports to match the real ports.
	// There may be fewer Nebula  ports than real ports if there
	// are self-loops, for such cases, ptable[i] will be null.
	if (isTopNebula) {
	    for (int i = 0; i < nports; i++) {
		PortHole* out = newPorts[i];
		NebulaPort* outNeb = out.asNebulaPort();
		const PortHole& outReal = outNeb->real();
		if (!out || out->isItInput()) continue;
		PortHole* in = newPorts[outReal.far()->index()];
		int numDelays = outReal.numInitDelays();
		const char* initDelays = outReal.initDelayValues();
		out->connect(*in,numDelays,initDelays);
		out->geo()->initialize();
	    }
	    LOG_DEL; delete newPorts;
	}
    }
}

void Nebula::addGalaxy(Galaxy* g,PortHole** newPorts) {
    GalTopBlockIter nextBlock(*g);
    Block* b; 
    while ((b = nextBlock++) != 0) {
	if (b->isItAtomic()) {
	    Nebula* c = newNebula();
	    c->setMasterBlock(b,newPorts);
	    addNebula(c);
	} else if (!flattenGalaxy((Galaxy*)b)) {
	    Nebula* c = newNebula();
	    c->setMasterBlock(b,newPorts);
	    BlockPortIter galaxyPorts(*b);
	    PortHole* port;
	    while((port = galaxyPorts++) != NULL) {
		PortHole* clonedPort = newPorts[((PortHole&)port->realPort()).index()];
		if (clonedPort) {
		    clonedPort->parent()->removePort(*clonedPort);
		    c->star().addPort(*clonedPort);
		    clonedPort->setParent(&c->star());
		}
	    }
	    addNebula(c);
	}
	else {
	    addGalaxy((Galaxy*)b,newPorts);
	}
    }
}

void Nebula::addNebula(Nebula* c) {
    gal.addBlock(c->star(),c->star().name());
    c->star().setParent(&star());
}


inline void Nebula::initMaster() {
    if (master) master->initialize();
    return;
}

int Nebula::run() {
    if (isNebulaAtomic())
	// Atomic Nebula
	return master->run();
    else if (sched)
	// Nebula contains a scheduled galaxy
	return sched->run();
    else
	// Nebula contains a un-scheduled galaxy
	return FALSE;
}

int Nebula::generateSchedule() {
    if (isNebulaAtomic()) {
	return TRUE;
    }
    else {
	NebulaIter nebula(*this);
	Nebula* n;
	while ((n = nebula++) !=0)
	    n->generateSchedule();
	sched->setup();
	return ! SimControl::haltRequested();
    }
}

NebulaPort::NebulaPort(PortHole& self, const PortHole& myMaster,Nebula* parentN)
:selfPort(self),master(myMaster),nebAliasedTo(0) {
    selfPort.setPort(real().name(),&(parentN->star()),INT);
    selfPort.myPlasma = Plasma::getPlasma(INT);
    selfPort.numberTokens = real().numXfer();
    selfPort.indexValue = real().index();
}

////////////////// ITERATORS ////////////////////////

// This private class is a stack of AllNebulaIter iterators.

class NebulaIterContext {
friend AllNebulaIter;
    NebulaIter* iter;
    NebulaIterContext* link;
    NebulaIterContext(NebulaIter* ii,NebulaIterContext* l)
    : iter(ii), link(l) {}
};

// Constructor.  Clear stack, create an iterator for this level.
AllNebulaIter::AllNebulaIter(Nebula& n) {
    stack = 0;
    LOG_NEW; thisLevelIter = new NebulaIter(n);
}

// The reset method pops up to the top level, then resets the top iterator
void AllNebulaIter::reset() {
    while (stack) pop();
    thisLevelIter->reset();
}

// Destructor.
AllNebulaIter::~AllNebulaIter() {
    reset();
    LOG_DEL; delete thisLevelIter;
}

// push current iterator onto stack, enter a new galaxy g.
inline void AllNebulaIter::push(Nebula& n) {
    LOG_NEW; stack = new NebulaIterContext(thisLevelIter, stack);
    LOG_NEW; thisLevelIter = new NebulaIter(n);
}

// pop an iterator off of the stack.
void AllNebulaIter::pop() {
    if (stack == 0) return;
    NebulaIterContext* t = stack;
    LOG_DEL; delete thisLevelIter;
    thisLevelIter = t->iter;
    stack = t->link;
    LOG_DEL; delete t;
}

// This method returns the next block.
Nebula* AllNebulaIter::next() {
    Nebula* n = thisLevelIter->next();
    while (!n) {
	// current level exhausted.  Try popping to proceed.
	// if stack is empty we are done.
	if (stack == 0) return 0;
	pop();
	n = thisLevelIter->next();
    }
    // have a block.  If it's a galaxy, we will need to process
    // it; this is pushed so we'll do it next time.
    if (!n->isNebulaAtomic()) push (*n);
    return n;
}		

FatNebulaIter::FatNebulaIter(Nebula& n) : AllNebulaIter(n) {}	

Nebula* FatNebulaIter::next() {	
    while (1) {			
	Nebula* n = AllNebulaIter::next();	
	if (!n) return 0;
	if (!n->isNebulaAtomic()) return n;
    }			
}			
