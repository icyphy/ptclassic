static const char file_id[] = "DFCluster.cc";
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

 Programmer: Jose Luis Pino
 Date of creation: 6/10/94

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DFCluster.h"

int DFCluster::run() {
    return Cluster::run();
}

// Constructors
DFCluster::DFCluster(const char* domain):DataFlowStar(),Cluster(*this,domain)
{};

DFClusterPort::DFClusterPort(const PortHole* master, Star* parent)
: DFPortHole(), ClusterPort(*this,*master,parent) {
    const DFPortHole& dfmaster = *(const DFPortHole*)master;
    DFPortHole::maxBackValue = dfmaster.maxDelay();
}

PortHole* DFCluster::clonePort(const PortHole* master, Star* parent) {
    return new DFClusterPort(master,parent);
}

Cluster* DFCluster::newCluster(Block* master,const char* domain) const {
    LOG_NEW; Cluster* cluster = new DFCluster(domain);
    if (master) cluster->setMasterBlock(master);
    if (domain)
	cluster->myDomain = domain;
    else
	cluster->myDomain = myDomain;
    return cluster;
}

int DFCluster::isSDFinContext() const {
    
    if (isClusterAtomic())
	return Cluster::master?((DataFlowStar*)Cluster::master)->isSDFinContext():TRUE;
    GalStarIter nextStar(gal);
    DataFlowStar* s;
    while ((s = (DataFlowStar*) nextStar++) != 0)
	if (!s->isSDFinContext()) return FALSE;
    return TRUE;
}

/*virtual*/ void DFCluster::setMasterBlock(Block*master,PortHole**newPorts) {
    Cluster::setMasterBlock(master,newPorts);
    if (master->isItAtomic())
	this->repetitions = ((DataFlowStar*)master)->repetitions;
}

/*virtual*/ int DFCluster::generateSchedule() {
    if (isClusterAtomic()) return TRUE;
    if (!Cluster::generateSchedule()) return FALSE;
    // Now we must adjust all external Cluster porthole parameters
    BlockPortIter clusterPorts(*this);
    DFClusterPort* port;
    while((port = (DFClusterPort*) clusterPorts++) != NULL) {
	DFClusterPort* realPort=(DFClusterPort*)port->asClusterPort()->clusterAlias();
	DFCluster* star = (DFCluster*) realPort->parent();
	int reps = star->reps();
	port->setSDFParams(realPort->numXfer()*reps,realPort->maxDelay()*reps);
    }
    return TRUE;
}

