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
	farSidePort->farSidePort = this;
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
		    Cluster Absorb & Merge Methods
 **********************************************************************/
int Cluster::absorb(Cluster& cluster, int removeFlag) {
    makeNonAtomic();
    Block* clusterParent =  cluster.parent();
    cout << "Absorbing " << cluster.name() << " into "
	 << name() << ".\n";
    
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
    cout << "Merging " << clusterToEmpty.name() << " into "
	 << name() << ".\n";

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
void Cluster::setScheduler(Scheduler* scheduler) {
    delete sched;
    sched = scheduler;
}

int Cluster::setTarget(Target*t) {
    if (sched && t) sched->setTarget(*t);
    return Galaxy::setTarget(t);
}

/**********************************************************************
		 Methods to convert a user-specified
	       galaxy hierarchy to a cluster hierarchy
 **********************************************************************/
void Cluster::initTopBlocksForClustering(BlockList& list, Galaxy& g) {
    GalTopBlockIter nextBlock(g);
    Block* b;
    while ((b = nextBlock++) != NULL) {
	if (b->isItAtomic()) {
	    Cluster* cluster = convertStar(b->asStar());
	    list.put(*cluster);
	}
	else if (flattenTest(b->asGalaxy())) { 
	    initTopBlocksForClustering(list,b->asGalaxy());
	    b->asGalaxy().empty();
	    delete b;
	}
	else {
	    Cluster* cluster = convertGalaxy(b->asGalaxy());
	    list.put(*cluster);
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

    BlockList blocksToMove;

    initTopBlocksForClustering(blocksToMove,galaxy);

    galaxy.empty();
    
    // Fix far ports, we only need to do this here - from now on,
    // merge and absorb will update this
    BlockListIter nextBlock(blocksToMove);
    Block *block;
    while ((block = nextBlock++) != NULL) {
	galaxy.addBlock(*block,block->name());
    }
    
    GalStarIter nextStar(galaxy);
    Star* star;
    while ((star = nextStar++) != NULL) {
	BlockPortIter nextPort(*star);
	PortHole* starPort;
	while ((starPort = nextPort++) != NULL) {
	    ClusterPort* port = (ClusterPort*)starPort;
	    while ((port = (ClusterPort*)port->aliasFrom()) != NULL)
		port->initializeClusterPort();
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

    BlockList list;
    initTopBlocksForClustering(list,g);
    BlockListIter nextBlock(list);
    Block *block;
    while ((block = nextBlock++) != NULL) {
	cluster->addBlock(*block,block->name());
    }
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

Cluster* SuccessorClusterIter::next() {
    Cluster* cluster;
    while ((cluster = (Cluster*)SuccessorIter::next()) != NULL &&
	   !isValidCluster(*cluster,prnt));
    return cluster;
}

Cluster* PredecessorClusterIter::next() {
    Cluster* cluster;
    while ((cluster = (Cluster*)PredecessorIter::next()) != NULL &&
	   !isValidCluster(*cluster,prnt));
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
    nextCluster.reset();
    while ((cluster = nextCluster++) != NULL) {	
	InfString sourceName;
	sourceName << dotName(*cluster);
 	PredecessorIter nextSuccessor(*cluster);
	Block* successor;
	while ((successor = nextSuccessor++) != NULL)
	    dot << "  " << sourceName << " -> " << dotName(*successor) <<";\n";
    }
    dot << "}\n";
    return dot;
}
