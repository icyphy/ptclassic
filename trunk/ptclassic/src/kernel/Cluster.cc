static const char file_id[] = "Cluster.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmers: J. L. Pino
 Added some stuff by P. K. Murthy (May 1996)
 Date of creation: 11/9/95


*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Cluster.h"
#include "Scheduler.h"
#include "GraphUtils.h"
#include "Scope.h"
#include <iostream.h>
#include "InfString.h"

/**********************************************************************
			 ClusterPort Methods
 **********************************************************************/
ClusterPort::~ClusterPort() {
    if (farSidePort) {
	if (alias() && alias()->parent() && !alias()->parent()->isItAtomic()) {
	    ((PortHole*)alias())->farSidePort = farSidePort;
	    farSidePort->farSidePort = (PortHole*)alias();
	}
	else
	    farSidePort->farSidePort = 0;
	farSidePort = 0;
    }
    // don't want PortHole destructor to try to disconnect from geodesic, so:
    myGeodesic = 0;
}    

void ClusterPort::initializeClusterPort() {
    if (!aliasFrom() && !far()) {
	farSidePort = &(PortHole&)aliasedPort(*((PortHole&)realPort()).far());
	farSidePort->farSidePort = this;
    }
}

void ClusterPort::update() {
    if (alias()->parent()->isItAtomic()) {
	GenericPort& farAliasPort =
	    aliasedPort(*((PortHole&)realPort()).far());
	if (!farAliasPort.parent()->isItAtomic()) {
	    farSidePort = (PortHole*)&farAliasPort;
	    farSidePort->farSidePort = this;
	}
    }
    else {
	farSidePort = ((PortHole*)alias())->farSidePort;
	if (farSidePort) farSidePort->farSidePort = this;
	if (!alias()->parent()->isItAtomic())
	    ((PortHole*)alias())->farSidePort = 0;
    }
}

ClusterPort::ClusterPort(GenericPort&p):GalPort(p) {
    setName(p.name());
}

/**********************************************************************
		     Cluster Class Identification
 **********************************************************************/
const char* Cluster::className() const {return "Cluster";}
ISA_FUNC(Cluster,Galaxy);
int Cluster::isItCluster () const { return TRUE;}

/**********************************************************************
		      Create and add a new port
 **********************************************************************/
ClusterPort* Cluster::addNewPort(PortHole& p) {
    ClusterPort* newPort = makeNewPort(p);
    addPort(*newPort);
    newPort->setParent(this);
    return newPort;
}

/**********************************************************************
		    Cluster Group, Absorb, & Merge Methods
 **********************************************************************/

// Group: This creates a new cluster that contains the argument cluster
// 			and this as its clusters.
//
// Absorb: This absorbs the argument cluster into itself, increasing the
// 			the number of clusters inside itself by 1.
//
// Merge: This merges the clusters inside the argument cluster into itself,
// 			increasing the number of clusters inside itself by the number 
//			of clusters inside of the argument cluster.

Cluster* Cluster::group(Cluster& c, int removeFlag)
{
	// first make sure that c and me have the same parent
	if (c.parent() != parent()) {
		StringList message;
		message << "Can't group " << name() << " and " << c.name()
		<< " because the clusters don't have the same parent.";
		Error::abortRun(*this,message);
		return 0;
	}
	Cluster* newC = (Cluster*)makeNew();
	// The new cluster will have my name
	newC->setName(name());
	// new cluster is added to parent galaxy's list
	(parent()->asGalaxy()).addBlock(*newC, newC->name());
	if (!newC->absorb(*this,removeFlag) || !newC->absorb(c,removeFlag)) {
		(parent()->asGalaxy()).removeBlock(*newC);
		StringList message;
		message << "Failed to absorb c into newly created empty cluster"
		<< "or failed to absorb b into newly created empty cluster";
		Error::abortRun(*this,message);
		return 0;
	}
	return newC;
}

int Cluster::absorb(Cluster& cluster, int removeFlag) {
    makeNonAtomic();
    Block* clusterParent =  cluster.parent();
    // cout << "Absorbing " << cluster.name() << " into "
    //	 << name() << ".\n";
    
    // Make sure we are not absorbing ourselves
    if (this == &cluster) {
	StringList message;
	message << "Can't absorb a cluster into itself";
	Error::abortRun(*this, message);
	return FALSE;
    }
    // First we must make sure that both clusters share the
    // same parent
    if (clusterParent != parent()) {
	StringList message;
	message << "Can't absorb " << cluster.name() << " into " << name()
		<< " because the clusters don't have the same parent.";
	Error::abortRun(*this,message);
	return FALSE;
    }

    // Now, remove the cluster from the parent galaxy
    if (removeFlag && clusterParent)
	clusterParent->asGalaxy().removeBlock(cluster);

    // Add the cluster to this cluster
    addBlock(cluster,cluster.name());

    // Now generate cluster ports & setup aliases
    BlockPortIter myPorts(*this);
    BlockPortIter otherPorts(cluster);
    PortHole *myPort, *otherPort;
    while ((otherPort = otherPorts++) != NULL) {
	myPorts.reset();
	while ((myPort = myPorts++) != NULL
	       && myPort->far() != otherPort);
	if (myPort && myPort->far() == otherPort) {
	    // This cluster and block connect via myPort/otherPort.
	    // Remove myPort - no need to create a new cluster port
	    myPorts.remove();
	    delete myPort;
	}
	else {
	    // This cluster and cluster do not connect via myPort/otherPort.
	    // We must create a new cluster port and setup the aliases.
	    ClusterPort* newPort = addNewPort(*otherPort);
	    newPort->update();
	}
	
    }

    return TRUE;
}    

int Cluster::merge(Cluster& clusterToEmpty, int removeFlag) {
    makeNonAtomic();
    clusterToEmpty.makeNonAtomic();
    // cout << "Merging " << clusterToEmpty.name() << " into "
    //	 << name() << ".\n";

    // Make sure we are not merging ourselves
    if (this == &clusterToEmpty) {
	StringList message;
	message << "Can't merge a cluster with itself";
	Error::abortRun(*this, message);
	return FALSE;
    }
    // First make sure that both clusters share the same parent
    if (parent() != clusterToEmpty.parent()) return FALSE;

    // Ports should be moved from the clusterToEmpty to this cluster
    // if the port is not part of a direct connection between the
    // two galaxies.  If there is a direct connection, we remove
    // the corresponding port from this cluster.

    // First we move the ports to the new galaxy or remove them,
    // as necessary
    BlockPortIter myPorts(*this);
    BlockPortIter otherPorts(clusterToEmpty);
    PortHole *myPort, *otherPort;
    while ((otherPort = otherPorts++) != NULL) {
	myPorts.reset();
	while ((myPort = myPorts++) != NULL
	       && myPort->far() != otherPort);
	if (myPort && myPort->far()==otherPort) {
	    // The galaxies do connect using this port, remove it
	    myPorts.remove();
	    delete myPort;
	}
	else {
	    // The galaxies do not connect using this port, move it
	    addPort(*otherPort);
	    otherPort->setParent(this);
 	    otherPorts.remove();
	}
    }

    // Now remove the clusterToEmpty from its parent galaxy
    if (removeFlag && clusterToEmpty.parent())
	clusterToEmpty.parent()->asGalaxy().removeBlock(clusterToEmpty);

    clusterToEmpty.flatten(this,FALSE);
    return TRUE;
}		

void Cluster::makeNonAtomic(){
    if (numberBlocks() == 1) {
	GalTopBlockIter clusterBlocks(*this);
	Block* block = clusterBlocks++;
	if (block->isItAtomic()) {
	    clusterBlocks.remove();
	    Cluster* cluster = convertStar(block->asStar());
	    addBlock(*cluster,cluster->name());
	    ClusterPortIter nextPort(*cluster);
	    ClusterPort* port;
	    while ((port = nextPort++) != NULL) {		
		port->initializeClusterPort();
	    }
	}
    }
}	

/**********************************************************************
		      Cluster scheduler methods
 **********************************************************************/
void Cluster::setClusterScheduler(Scheduler* scheduler) {
    delete sched;
    sched = scheduler;
}

Scheduler* Cluster :: scheduler() const {
    // if clusterScheduler() returns NULL (meaning that sched is NULL),
    // then we call Block::scheduler; this will call the parents scheduler()
    // method if the parent is defined. In a cluster hierarchy, this will
    // ensure that the first parent in the hierarchy for which the scheduler
    // is defined will be the one returned.
    Scheduler* sch = clusterScheduler();
    if (sch) return sch;
    else return Block::scheduler();
}

Scheduler* Cluster :: clusterScheduler() const { return sched; }

int Cluster::setTarget(Target*t) {
    if (sched && t) sched->setTarget(*t);
    return Galaxy::setTarget(t);
}

/**********************************************************************
		 Methods to convert a user-specified
	       galaxy hierarchy to a cluster hierarchy
 **********************************************************************/
void Cluster::initTopBlocksForClustering(Galaxy& list, Galaxy& g) {
    GalTopBlockIter nextBlock(g);
    Block* b;
    while ((b = nextBlock++) != NULL) {
	if (b->isItAtomic()) {
	    Cluster* cluster = convertStar(b->asStar());
	    list.addBlock(*cluster,b->name());
	}
	else if (flattenTest(b->asGalaxy())) { 
	    initTopBlocksForClustering(list,b->asGalaxy());
	    b->asGalaxy().empty();
	    delete b;
	}
	else {
	    Cluster* cluster = convertGalaxy(b->asGalaxy());
	    list.addBlock(*cluster,b->name());
	    b->asGalaxy().empty();
	    delete b;
	}
    }
}

void Cluster::initializeForClustering(Galaxy& galaxy) {
    // Create the scoping hierarchy.  This scheduler destroys the user
    // specified hierarchy by doing clustering.  The scoping hierarchy
    // allows us to do this without losing inherited states.
    Scope::createScope(galaxy);

    Galaxy blocksToMove;

    initTopBlocksForClustering(blocksToMove,galaxy);

    galaxy.empty();
    
    blocksToMove.flatten(&galaxy);
    
    // Fix far ports, we only need to do this here - from now on,
    // merge and absorb will update this
    GalStarIter nextStar(galaxy);
    Star* star;
    while ((star = nextStar++) != NULL) {
	BlockPortIter nextPort(*star);
	PortHole* starPort;
	while ((starPort = nextPort++) != NULL) {
	    ClusterPort* port = (ClusterPort*)starPort->aliasFrom();
	    while (port != NULL) {
		port->initializeClusterPort();
		port = (ClusterPort*)port->aliasFrom();
	    }
	}
    }
}

Cluster* Cluster::convertStar(Star& s) {
    Cluster* cluster = (Cluster*) makeNew();
    cluster->setName(s.name());
    cluster->addBlock(s,s.name());
    BlockPortIter nextPort(s);
    PortHole* port;
    while ((port = nextPort++) != NULL) {
	cluster->addNewPort(*port);
    }
    return cluster;	
}

Cluster* Cluster::convertGalaxy(Galaxy& g) {
    Cluster* cluster = (Cluster*) makeNew();
    cluster->setName(g.name());

    BlockPortIter nextPort(g);
    PortHole* galPort;
    while ((galPort = nextPort++) != NULL) {
	GenericPort* portToAlias = galPort->alias();
	galPort->clearAliases();
	cluster->addNewPort((PortHole&)*portToAlias);
    }

    Galaxy dummy;
    initTopBlocksForClustering(dummy,g);
    dummy.flatten(cluster);
    return cluster;
}

/**********************************************************************
		    Cluster Iterator Next Methods

   It would be nice to automatically cleanup the Galaxy that we are
   iterating over - but it would be dangerous.  This could cause a
   core dump if given a nested Cluster Iterator inside of another -
   where both are iterating over the same galaxy and one iterator
   removes a bogus entry which the other is currently pointing to.
 **********************************************************************/
Cluster* ClusterIter::next() {
    Block* b;
    while ((b = GalTopBlockIter::next()) != NULL) {
	if (!b->isItAtomic() && isValidCluster(*(Cluster*)b,&prnt)) break;
    }
    return (Cluster*)b;
}

// The following is the same except that it returns the next cluster
// where the block has flags[flag_loc]=flag_val

Cluster* ClusterIter::next(int flag_loc, int flag_val) {
    Block* b;
    while ((b = GalTopBlockIter::next()) != NULL) {
	if (b->flags[flag_loc]==flag_val && !b->isItAtomic() && isValidCluster(*(Cluster*)b,&prnt)) break;
    }
    return (Cluster*)b;
}
Cluster* SuccessorClusterIter::next() {
    Cluster* cluster;
    while ((cluster = (Cluster*)SuccessorIter::next()) != NULL &&
	   !isValidCluster(*cluster,prnt));
    return cluster;
}

Cluster* SuccessorClusterIter::next(int flag_loc, int flag_val) {
    Cluster* cluster;
    while ((cluster = (Cluster*)SuccessorIter::next(flag_loc,flag_val)) != NULL
    		&& !isValidCluster(*cluster,prnt));
    return cluster;
}


Cluster* PredecessorClusterIter::next() {
    Cluster* cluster;
    while ((cluster = (Cluster*)PredecessorIter::next()) != NULL &&
	   !isValidCluster(*cluster,prnt));
    return cluster;
}

Cluster* PredecessorClusterIter::next(int flag_loc, int flag_val) {
    Cluster* cluster;
    while ((cluster = (Cluster*)PredecessorIter::next(flag_loc,flag_val)) 
		!= NULL && !isValidCluster(*cluster,prnt));
    return cluster;
}

/**********************************************************************
		   Miscellaneous Cluster Functions
 **********************************************************************/

// A cluster is defined as valid if is both nonempty and the cluster's
// parent is equal to that specified in this function's prnt argument.
int isValidCluster(Cluster& b, Block* prnt) {
    return b.parent()==prnt && b.numberBlocks();
}

// Remove all bogus clusters - optionally do this recursively
void cleanupAfterCluster(Galaxy& g, int recursive) {
    GalTopBlockIter nextBlock(g);
    Block* block;
	
    while ((block = nextBlock++) != NULL) {
	if (recursive && !block->isItAtomic())
	    cleanupAfterCluster(block->asGalaxy(),TRUE);
	if (! block->isItAtomic() &&
	    ! block->asGalaxy().numberBlocks()) {
	    delete block;
	    nextBlock.remove();
	}
	else if (block->parent() != &g)
	    nextBlock.remove();
    }
}

// Return a dotty cluster interconnect representation
StringList printClusterDot(Galaxy& galaxy) {
    numberAllBlocks(galaxy);
    StringList dot;
    dot << "digraph " << galaxy.name() << " {\n"
	<< "  node [shape=record,width=.1,height=.1];\n";
    ClusterIter nextCluster(galaxy);
    Cluster* cluster;
    while ((cluster = nextCluster++) != NULL) {	
	InfString sourceName;
	sourceName << dotName(*cluster);
	dot << "  " << sourceName << " [label=\""
	    << cluster->name() << "\\nBlocks:"
	    << cluster->numberBlocks() << "\"];\n";
    }
    nextCluster.reset();
    while ((cluster = nextCluster++) != NULL) {	
	InfString sourceName;
	sourceName << dotName(*cluster);
 	SuccessorIter nextSuccessor(*cluster);
	Block* successor;
	while ((successor = nextSuccessor++) != NULL)
	    dot << "  " << sourceName << " -> " << dotName(*successor) <<";\n";
    }
    dot << "}\n";
    return dot;
}
