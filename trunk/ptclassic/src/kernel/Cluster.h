#ifndef _Cluster_h
#define _Cluster_h 1
#ifdef __GNUG__
#pragma interface
#endif


/*******************************************************************
 SCCS Version identification :
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

 Programmers : Jose Luis Pino & T. M. Parks
 Date of Creation : 6/10/90
	
********************************************************************/

#include "Block.h"
#include "GalIter.h"
#include "DynamicGalaxy.h"
#include "Scheduler.h"
class Star;
class ClusterPort;

class Cluster {
friend class ClusterIter;
friend class CClusterIter;
friend class ClusterPortIter;
public:

    Cluster(Star& self,const char* domain);

    void clearMaster() { master = NULL; }
    
    virtual ~Cluster();
    // set the master and build cluster
    virtual void setMasterBlock(Block* master,PortHole** newPorts = NULL);
    
    // Set the scheduler of the Cluster
    void setInnerSched(Scheduler* s);

    void setStopTime(double limit) {
	sched->setStopTime(limit);
    }

    double getStopTime() {
	return sched->getStopTime();
    }
    
    Galaxy* galaxy() { return &gal;}

    int isClusterAtomic() const;

    // Add a cluster into the internal galaxy of this cluster.
    void addCluster(Cluster*);

    void addGalaxy(Galaxy*,PortHole**);

    void initMaster();

    void compileRun() { innerSched()->compileRun(); }

    Scheduler* outerSched() {
	return star().scheduler()?star().scheduler():master->scheduler();
    }
    
    Scheduler* innerSched() { return sched;} 

//	void merge(Cluster*);
//	void absorb(Cluster*);

    int run();

    virtual PortHole* clonePort(const PortHole*,Star* parent=NULL) = 0;

    // By default the domain is set to the same as this cluster.
    virtual Cluster* newCluster(Block* s = NULL,const char* domain = NULL)
	 const = 0;

    virtual int flattenGalaxy(Galaxy*) {return FALSE;}
    
    Star& star() const { return selfStar; }
    StringList displaySchedule();
    
    /* Generate the schedules of the nested clusters recursively.
       It first calls generateSubSchedules() to generate the internal
       cluster schedules, then schedules itself. */
    
    virtual int generateSchedule();

    virtual Block* cloneCluster()  const;

    const char* myDomain;

    Block* masterBlock() const { return master; }

    // Add a spliced star to this cluster
    virtual int addSplicedStar(Star&);

protected:
    // The Star part of the Cluster.
    Star& selfStar;

    Block* master;

    DynamicGalaxy gal;
    Scheduler* sched;

    // Generate the schedules of all the internal clusters.
    virtual int generateSubSchedules();

    // Has this cluster been scheduled?
    int scheduled;

private:
    // Connect two Cluster PortHoles together
    void connect(PortHole* source, PortHole* destination);
};

// An iterator for ClusterList.
class ClusterIter : private GalStarIter {
public:
    ClusterIter(Cluster& n):GalStarIter(n.gal) {};

    Cluster* next() {
	Star* star = GalStarIter::next();
	return star? star->asCluster(): NULL;
    }

    Cluster* operator++(POSTFIX_OP) { return next();}
    GalStarIter::reset;
};

class CClusterIter : private CGalStarIter {
public:
    CClusterIter(const Cluster& n):CGalStarIter(n.gal) {};

    const Cluster* next() {
	const Star* star = CGalStarIter::next();
	return star? star->asCluster(): NULL;
    }

    const Cluster* operator++(POSTFIX_OP) { return next();}
    CGalStarIter::reset;
};

class ClusterPort {
public:
    ClusterPort(PortHole& self, const PortHole& p, Star* parent);
    PortHole& real() const { return master; }
    PortHole& asPort() const { return selfPort;}
    int isItInput() const {
	return real().isItInput();
    }
    int isItOutput() const {
	return real().isItOutput();
    }

    void setClusterAlias(PortHole* np) { clusterAliasedTo = np;} 

    PortHole* clusterAlias() const { return clusterAliasedTo; }

    PortHole* realClusterPort();
private:
    
    // selfPort is a reference to the PortHole side of a ClusterPort
    PortHole& selfPort;

    // clusterAliased much like aliases used in Galaxy ports, points to
    // the actual clusterPort.
    PortHole* clusterAliasedTo;

    // master is a reference to the original PortHole that this
    // porthole represents.
    PortHole& master;
};

class ClusterPortIter : private BlockPortIter {
public:
    ClusterPortIter(Cluster& n):BlockPortIter(n.star()) {};

    ClusterPort* next() {
	PortHole* port = BlockPortIter::next();
	return port? port->asClusterPort(): NULL;
    }

    ClusterPort* operator++(POSTFIX_OP) { return next();}
    BlockPortIter::reset;
};

class ClusterIterContext;

class AllClusterIter {
public:
    AllClusterIter(Cluster&);
    ~AllClusterIter();
    Cluster* next();
    Cluster* operator++(POSTFIX_OP) { return next();}
    void reset();
protected:
    ClusterIter *thisLevelIter;
    ClusterIterContext *stack;
    void push(Cluster&);
    void pop();
};

class FatClusterIter : private AllClusterIter {
public:
    FatClusterIter(Cluster&);
    Cluster* next();
    Cluster* operator++(POSTFIX_OP) { return next();}
    void reset() { AllClusterIter::reset();}

    // need a public destructor because of private derivation
    ~FatClusterIter(){}
};

#endif






