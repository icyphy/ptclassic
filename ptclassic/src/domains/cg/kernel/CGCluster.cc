static const char file_id[] = "CGCluster.cc";
/******************************************************************
Version identification:
 $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

#include "CGCluster.h"
#include "Geodesic.h"
#include "SDFScheduler.h"
#include "LoopScheduler.h"
#include "SDFCluster.h"

void CGCluster::fixBufferSizes(int nReps) {
    if (isClusterAtomic()) {
	DFStarPortIter nextPort(*this);
	DFPortHole* p;
	while((p = nextPort++) != 0) {
	    DFPortHole* realPort =
		(DFPortHole*)&(p->asClusterPort()->real());
	    int maxArcCount = p->geo()->maxNumParticles();
	    realPort->setMaxArcCount(nReps*maxArcCount);
	}
    } 
    else {
	CGClusterIter nextCluster(*this);
	CGCluster* c;
	while((c = nextCluster++) != 0) {
	    c->fixBufferSizes(nReps * ((DataFlowStar&)star()).reps());
	}
    }
}

int CGCluster::isA(const char* type) const{
    if (Cluster::master && Cluster::master->isItAtomic())
	return Cluster::master->isA(type);
    else if (Cluster::master) {
	GalStarIter nextStar (*(Galaxy*)Cluster::master);
	Star* star = nextStar++;
	return star->isA(type);
    }
    else {
	Error::abortRun("Failed to resolve isA from CGCluster");
	return FALSE;
    }
}

int CGCluster::run() {
    return Cluster::run();
}

void CGCluster::setMasterBlock(Block*m,PortHole**newPorts) {
    myDomain = m->domain();

    DFClusterBase::setMasterBlock(m,newPorts);
    StringState* scheduler =
	(StringState*) Cluster::master->stateWithName("Scheduler");
    if (scheduler) {
	const char* schedType = scheduler->initValue();
	IntState* processor = (IntState*) Cluster::master->stateWithName("procId");
	if (processor) setProcId((int) (*processor));
	Scheduler* newSched;
	if (strcmp(schedType,"Cluster") == 0)
	    newSched = new SDFClustSched("");
	else if (strcmp(schedType,"Loop") == 0)
	    newSched = new LoopScheduler("");
	else
	    newSched = new SDFScheduler;
	setInnerSched(newSched);
    }
}	    

int CGCluster::addSplicedStar(Star&s) {
    Block* orgMaster = Cluster::master;
    if (!DFClusterBase::addSplicedStar(s)) return FALSE;
    if (orgMaster->isA("DataFlowStar")) {
	// Must set repetitions manually, MultiScheduler does this
	// for the actual galaxy, the hier-schedulers do not figure
    	// this out -- maybe this is a mistake?  FIXME
	DataFlowStar& dfs = (DataFlowStar&)s;
    	DataFlowStar& dfm = (DataFlowStar&)*orgMaster;
	dfs.repetitions = dfm.repetitions;
    }
    Scheduler* newSched = new SDFScheduler;
    setInnerSched(newSched);
    newSched->setTarget(*s.target());
    newSched->setup();
    return !SimControl::haltRequested();
}

int CGCluster::setTarget(Target* t) {
    if (Cluster::master)
	if (!Cluster::master->setTarget(t)) return FALSE;
    if (!CGStar::setTarget(t)) return FALSE;
    GalStarIter nextStar(*galaxy());
    Star* star;
    while ((star=nextStar++) != NULL)
	if (!star->setTarget(t)) return FALSE;
    return TRUE;
}
    
int CGCluster::myExecTime() {
    if (isClusterAtomic())
	return ((DataFlowStar*) Cluster::master)->myExecTime();
    int execTime = 0;
    DFClusterStarIter next(*this);
    DataFlowStar* s;
    while ((s = next++) != 0) {
	execTime += s->myExecTime() * s->reps();
    }
    return execTime;
}

CGCluster::CGCluster(const char* domain):
CGStar(),DFClusterBase(*this,domain)
{};

CGClusterPort::CGClusterPort(PortHole* master, Star* parent)
: CGPortHole(), ClusterPort(*this,*master,parent) {
    const DFPortHole& dfmaster = *(const DFPortHole*)master;
    DFPortHole::maxBackValue = dfmaster.maxDelay();
}

PortHole* CGCluster::clonePort(PortHole* master, Star* parent) {
    return new CGClusterPort(master,parent);
}

Cluster* CGCluster::newCluster(Block* master,const char* domain) const {
    LOG_NEW; Cluster* cluster = new CGCluster(domain);
    if (master) cluster->setMasterBlock(master);
    if (domain)
	cluster->myDomain = domain;
    else
	cluster->myDomain = myDomain;
    return cluster;
}

