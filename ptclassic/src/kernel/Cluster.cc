static const char file_id[] = "Cluster.cc";
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

 A Cluster is an executable Star, a sort of light-weight Wormhole.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Cluster.h"
#include "Star.h"
#include "Galaxy.h"
#include "SimControl.h"
#include "Scheduler.h"
#include "Geodesic.h"
#include "Plasma.h"

extern int setPortIndices(Galaxy&);

Cluster::Cluster(Star&self, const char* domain):
myDomain(domain),selfStar(self),master(NULL),sched(0),scheduled(0) {};

/*virtual*/ Cluster::~Cluster() {
    if (isClusterAtomic()) {
	if (master) ((Star*)master)->setParentCluster(0);
    }
    LOG_DEL; delete sched;
}

StringList Cluster::displaySchedule() {
    StringList schedule;
    if (isClusterAtomic()) {
	schedule = star().fullName();
	return schedule;
    }
    else if (!innerSched())
	schedule << "ERROR NO SCHEDULE MEMBER IN CLUSTER:"
		 << star().fullName() << "\n";
    else
	schedule << "/* Schedule for " << star().fullName() << " */\n"
		 << innerSched()->displaySchedule() << "\n";
    // If we get here cluster is Non-atomic
    FatClusterIter next(*this);
    Cluster* fatCluster;
    while ((fatCluster = next++) != 0)
	schedule << fatCluster->displaySchedule();
    return schedule;
}

void Cluster::setInnerSched(Scheduler* s) {
    if (s == NULL) {
	Error::abortRun(star(),
			"Cluster::setInnerSched sent a NULL scheduler pointer");
	return;
    }
    LOG_DEL; delete sched;
    sched = s;
    sched->setGalaxy(gal);
}

void Cluster::setMasterBlock(Block* m,PortHole** newPorts) {
    if (master) {
	Error::abortRun("Can not run setMaster twice on same Cluster");
	return;
    }
    master = m;
    if (master->isItAtomic()) {
	((Star*)master)->setParentCluster(this);
	StringList fullname = master->fullName();
	star().setName(hashstring(fullname));
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
		PortHole* clonedPort = clonePort(port,&star());
		if(newPorts)
		    newPorts[clonedPort->asClusterPort()->
			     real().index()] = clonedPort;
	    }
	}
    }
    else {
	// Set the name, leave parent unchanged
	StringList name;
	name << master->fullName() << "_Cluster";
	const char* pname = hashstring(name);
	star().setName(pname);
	
	Galaxy* g = (Galaxy*) m;
	int isTopCluster = ! (int) newPorts;
	int nports = 0;
	if (isTopCluster) {
	    nports = setPortIndices(*g);
	    LOG_NEW; newPorts = new PortHole*[nports];
	    for (int i = 0; i < nports; i++) newPorts[i] = 0;
	}

	addGalaxy(g,newPorts);

	// now connect up the Cluster ports to match the real ports.
	// There may be fewer Cluster  ports than real ports if there
	// are self-loops, for such cases, ptable[i] will be null.
	if (isTopCluster) {
	    for (int i = 0; i < nports; i++) {
		PortHole* source = newPorts[i];
		if (!source || source->isItInput()) continue;
		PortHole* destination =
		    newPorts[source->asClusterPort()->real().far()->index()];
		connect(source,destination);
	    }
	    LOG_DEL; delete newPorts;
	}
    }
}

void Cluster::connect(PortHole* source, PortHole* destination) {
    const PortHole& realDestination = destination->asClusterPort()->real();
    int numDelays = realDestination.numInitDelays();
    const char* initDelays = realDestination.initDelayValues();
    source->connect(*destination,numDelays,initDelays);
    source->geo()->initialize();
}

/* Add a galaxy, flattened at the the top-most level */
void Cluster::addGalaxy(Galaxy* g,PortHole** newPorts) {
    GalTopBlockIter nextBlock(*g);
    Block* b; 
    while ((b = nextBlock++) != 0) {
	if (b->isItAtomic()) { /* Add Star as cluster */
	    Cluster* c = newCluster();
	    c->setMasterBlock(b,newPorts);
	    addCluster(c);
	} else if (!flattenGalaxy((Galaxy*)b)) { /* Add Galaxy as cluster */
	    Cluster* c = newCluster();
	    // Clusters must first be added to their parent clusters
	    // before they built with setMasterBlock
	    addCluster(c);
	    c->setMasterBlock(b,newPorts);
	    c->myDomain = ((Galaxy*)b)->domain();

	    // Now create ports on the new cluster corresponding to
	    // the galaxy ports
	    BlockPortIter galaxyPorts(*b);
	    PortHole* port;
	    while((port = galaxyPorts++) != NULL) {
		/* Resolve all the aliases */
		PortHole& rPort = (PortHole&) port->realPort();
		
		/* Check if the realPort is still part of inside star.  If
		   we remove ports of a star at a galaxy boundary... the
		   galaxy port list will still contain the alias port in
		   its port list. */
		
		if (! rPort.parent()->portWithName(rPort.name())) continue;

		/* Now, we are can assume that the port is ok to index */
		int index = rPort.index();

		/* Get the internal cluster porthole which will correspond
		   to the new external cluster porthole */

		PortHole* clusterPort = newPorts[index];
		if (clusterPort) {
		    /* If clusterPort returned NULL, that means that the
		       original porthole was a self loop, or at a
		       boundary.  We now create a dummyPort which we'll
		       connect to the internal porthole so that it is not
		       left dangling.  This is much like even horizons.
		       The atBoundary test will return true for these
		       connections. */
		    
		    PortHole* dummyPort = clonePort(&clusterPort->asClusterPort()->real());
		    dummyPort->asClusterPort()->setClusterAlias(clusterPort);
		    if (clusterPort->isItInput()) {
			connect(dummyPort,clusterPort);
		    }
		    else {
			connect(clusterPort,dummyPort);
		    }

		    /* Finally, we create the external cluster port */
		    PortHole* fatClusterPort =
			clonePort(&clusterPort->asClusterPort()->real(),&c->star());
		    fatClusterPort->asClusterPort()->setClusterAlias(clusterPort);
		    newPorts[index] = fatClusterPort;
		}
	    }

	}
	else { /* Flatten Galaxy */
	    addGalaxy((Galaxy*)b,newPorts);
	}
    }
}

void Cluster::addCluster(Cluster* c) {
    gal.addBlock(c->star(),c->star().name());
    c->star().setParent(&gal);
    c->star().setTarget(star().target());
}

void Cluster::initMaster() {
    if (master) master->initialize();
    return;
}

int Cluster::isClusterAtomic() const {
    return master? master->isItAtomic() : FALSE;
}

int Cluster::run() {
    if (isClusterAtomic())
	// Atomic Cluster - if there is no master, then it is a
	// virtual sink or source node
	return master? master->run() : TRUE;
    else if (sched) {
	// Cluster contains a scheduled galaxy
	sched->compileRun();
	return !SimControl::haltRequested();
    }
    else {
	Error::abortRun(star(),"Cluster contains a un-scheduled galaxy");
	return FALSE;
    }
}

int Cluster::addSplicedStar(Star& s) {
    if (!isClusterAtomic()) {
	// We could handle this case, if we moved all the clusters
	// in this cluster into a new cluster -AND- move the schedule.
	// For now, we do not handle this case
	Error::abortRun(star(),
			"Cannot add a spliced star to a non-atomic cluster");
	return FALSE;
    }

    // Create a temporary galaxy, so that we can connect the new clusters
    Galaxy gal;
    int nports = 0;
    // Save parent information, so we can restore parents after addBlock
    Block* sPrnt = s.parent();
    Block* mPrnt = master->parent();
    gal.addBlock(s,s.name());
    gal.addBlock(*master,master->name());
    nports = setPortIndices(gal);
    // Restore parent information
    s.setParent(sPrnt);
    master->setParent(mPrnt);
    
    LOG_NEW; PortHole** newPorts = new PortHole* [nports];
    for (int i = 0; i < nports; i++) newPorts[i] = 0;

    // Create new clusters
    // Create cluster containing spliced star
    Cluster* starCluster = newCluster();
    starCluster->setMasterBlock(&s,newPorts);
    addCluster(starCluster);
    s.setTarget(master->target());
    // Create the cluster containing the original star
    Cluster* orgCluster = newCluster();
    orgCluster->setMasterBlock(master,newPorts);
    addCluster(orgCluster);

    // Now we connect up the Cluster ports to match the real ports.
    // There may be fewer Cluster ports than real ports if there
    // are self-loops, for such cases, ptable[i] will be null.
    for (i = 0; i < nports; i++) {
	PortHole* source = newPorts[i];
	// Make sure that the far port is one of the two stars
	// If not, we remove the port
	PortHole* far = source->asClusterPort()->real().far();
	if ( far->parent() != master && far->parent() != &s) {
	    source->parent()->removePort(*source);
	    continue;
	}

	if (!source || source->isItInput()) continue;

	PortHole* destination = newPorts[far->index()];
	connect(source,destination);
    }
    LOG_DEL; delete newPorts;

    // This cluster no longer has a master
    master = NULL;

    return TRUE;
}
	    
int Cluster::generateSchedule() {
    if (isClusterAtomic()) {
	return scheduled = TRUE;
    }
    else {
	if (!generateSubSchedules()) return scheduled = FALSE;
	sched->setup();
	return scheduled = !SimControl::haltRequested();
    }
}

/*virtual*/ int Cluster::generateSubSchedules() {
    if (!isClusterAtomic()) {
	ClusterIter cluster(*this);
	Cluster* n;
	while ((n = cluster++) !=0)
	    if (!n->generateSchedule()) return FALSE;
    }
    return TRUE;
}

ClusterPort::ClusterPort(PortHole& self, PortHole& myMaster, Star* parent)
:selfPort(self),clusterAliasedTo(0),master(myMaster) {
    selfPort.setPort(real().name(),parent,INT);
    selfPort.myPlasma = Plasma::getPlasma(INT);
    selfPort.numberTokens = real().numXfer();
    selfPort.indexValue = real().index();
    if (parent) parent->addPort(selfPort);
}

PortHole* ClusterPort::realClusterPort() {
    if (clusterAlias())
     	return clusterAlias()->asClusterPort()->realClusterPort();
    else
	return &this->asPort();
}
////////////////// ITERATORS ////////////////////////

// This private class is a stack of AllClusterIter iterators.

class ClusterIterContext {
friend AllClusterIter;
    ClusterIter* iter;
    ClusterIterContext* link;
    ClusterIterContext(ClusterIter* ii,ClusterIterContext* l)
    : iter(ii), link(l) {}
};

// Constructor.  Clear stack, create an iterator for this level.
AllClusterIter::AllClusterIter(Cluster& n) {
    stack = 0;
    LOG_NEW; thisLevelIter = new ClusterIter(n);
}

// The reset method pops up to the top level, then resets the top iterator
void AllClusterIter::reset() {
    while (stack) pop();
    thisLevelIter->reset();
}

// Destructor.
AllClusterIter::~AllClusterIter() {
    reset();
    LOG_DEL; delete thisLevelIter;
}

// push current iterator onto stack, enter a new galaxy g.
inline void AllClusterIter::push(Cluster& n) {
    LOG_NEW; stack = new ClusterIterContext(thisLevelIter, stack);
    LOG_NEW; thisLevelIter = new ClusterIter(n);
}

// pop an iterator off of the stack.
void AllClusterIter::pop() {
    if (stack == 0) return;
    ClusterIterContext* t = stack;
    LOG_DEL; delete thisLevelIter;
    thisLevelIter = t->iter;
    stack = t->link;
    LOG_DEL; delete t;
}

// This method returns the next block.
Cluster* AllClusterIter::next() {
    Cluster* n = thisLevelIter->next();
    while (!n) {
	// current level exhausted.  Try popping to proceed.
	// if stack is empty we are done.
	if (stack == 0) return 0;
	pop();
	n = thisLevelIter->next();
    }
    // have a block.  If it's a galaxy, we will need to process
    // it; this is pushed so we'll do it next time.
    if (!n->isClusterAtomic()) push (*n);
    return n;
}		

FatClusterIter::FatClusterIter(Cluster& n) : AllClusterIter(n) {}	

Cluster* FatClusterIter::next() {	
    while (1) {			
	Cluster* n = AllClusterIter::next();	
	if (!n) return 0;
	if (!n->isClusterAtomic()) return n;
    }			
}			

Block* Cluster::cloneCluster() const {
    Star* s = (Star*) star().makeNew();
    Cluster* cluster = s->asCluster();
    if (!cluster) {
	Error::abortRun
	    (star(),"cloneCluster: makeNew not defined for the cluster class");
	return NULL;
    }
    cluster->setMasterBlock(&this->star());
    return s;
}
