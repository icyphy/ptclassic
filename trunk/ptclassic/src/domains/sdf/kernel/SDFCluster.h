#ifndef _SDFCluster_h
#define _SDFCluster_h 1

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer:  J. Buck
 Date of creation: 3/17/92

This file defines all the classes for a clustering SDF scheduler.

Given a galaxy, we create a parallel hierarchy called an SDFClusterGal.
In the parallel hierarchy, objects of class SDFAtomCluster correspond
to the individual stars of the original galaxy; each SDFAtomCluster contains
a reference to an SDFStar.  The clustering algorithm transforms the
SDFClusterGal into a set of nested clusters.  class SDFClusterBag
represents a composite cluster with its own schedule; it resembles a
wormhole in some respects but is simpler (it does not use a pair of
event-horizon objects and does not have a Target; it does have a contained
galaxy and a type of SDFScheduler).

Cluster boundaries are represented by disconnected portholes;
SDFScheduler treats such portholes exactly as if they were wormhole
boundaries.

The virtual baseclass SDFCluster refers to either type of cluster.

Two specialized types of scheduler, both derived from SDFScheduler,
are used; SDFBagScheduler is used for schedules inside SDFClusterBags,
and SDFClustSched is used as the top-level scheduler.

The remaining class defined here is SDFClustPort, a porthole for use
in SDFCluster objects.

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "SDFStar.h"
#include "DynamicGalaxy.h"
#include "SDFScheduler.h"

class SDFCluster;
class SDFClusterBag;
class ostream;

// A SDFClusterGal is a galaxy made up of SDFCluster objects.  When
// constructed, it is a flat galaxy with one atomic cluster corresponding
// to each star in the original galaxy.  The cluster() function introduces
// hierarchy, using the merge/loop clustering algorithm.

class SDFClusterGal : public DynamicGalaxy {
public:
	// constructor creates flat galaxy of atomic clusters
	SDFClusterGal(Galaxy&,ostream* log = 0);
	SDFClusterGal(ostream* log = 0) : logstrm(log), bagNumber(-1) {}

	// destructor zaps all the member clusters (inherited)
	~SDFClusterGal() {}

	// how many?
	int numberClusts() const { return numberBlocks();}

	// this is it!  The main clustering routine Returns TRUE
	// if any clustering is performed.
	int cluster();

	// return TRUE if all member clusters have the same rate
	int uniformRate();

	// I would prefer for these to be protected rather than
	// public.

	// do a loop pass.  Return TRUE if a change was made
	int loopPass();

	// do a merge pass.  Return TRUE if a change was made
	int mergePass();

	// merge any "parallel loops".  Return TRUE if a change was made
	int parallelLoopMergePass();

	// set log stream member to match that of another (parent)
	// galaxy
	void dupStream(SDFClusterGal *pgal) { logstrm = pgal->logstrm;}

protected:
	ostream* logstrm;	// for logging errors

	// core of clustering routine; returns TRUE if any clustering
	// is performed and sets uniformRate if all clusters/actors have
	// the same rate.
	int clusterCore(int& uniformRate);

	// function to do an "expensive merge" -- it does the full
	// search for indirect paths.  If it finds a mergeable pair
	// it merges it and returns the result; otherwise it returns 0.
	SDFCluster* fullSearchMerge();

	// merge two clusters, returning the result.
	SDFCluster* merge(SDFCluster* c1,SDFCluster* c2);

	// generate a name for a new member ClusterBag
	const char* genBagName();

	// return true if there is an indirect path between two
	// member clusters
	int indirectPath(SDFCluster* src,SDFCluster* dst);

	// function used in finding paths by indirectPath.
	int findPath(SDFCluster* src,SDFCluster* dst);

	// mark feed-forward delays
	int markFeedForwardDelayArcs();

	// interior portions of loop pass
	int integralLoopPass(int doAnyLoop);
	int loopTwoClusts();

	// pass to do non-integral rate changes if structure is a tree.
	int tryTreeLoop();

	// predicate to determine whether cluster has a tree topology
	int isTree();
private:
	int bagNumber;		// number for generating bag names
	SequentialList stopList;// this list is used by fullSearchMerge.
};

// This is an "even baser" baseclass for all kinds of SDF clusters.
class SDFBaseCluster : public SDFStar {
public:
	// return the schedule
	virtual StringList displaySchedule(int depth) = 0;

	// generate code using target methods
	virtual void genCode(Target&, int depth) = 0;

	// hook for doing simulated execution of the real stars.
	virtual void simRunRealStars() = 0;

	// run/go
	int run() = 0;
	void go() { run();}
};

// This is the baseclass for SDF cluster objects.  A cluster may have
// an internal galaxy (if it is an SDFClusterBag); it always has a loop
// count, indicating how many times it is to be looped.

class SDFCluster : public SDFBaseCluster {
protected:
	int pLoop;		// loop count
	int visitFlag;		// visit flag
public:
	// constructor: looping is 1 by default
	SDFCluster() : pLoop(1) {}

	// make destructor virtual
	virtual ~SDFCluster() {}

	void setVisit(int i) { visitFlag = i; }
	int  visited() { return visitFlag; }

	// repetitions value
	int reps() const { return repetitions.num();}

	// return loop count
	int loop() const { return pLoop;}

	// cast to a bag (return null if not a bag)
	virtual SDFClusterBag* asBag() { return 0;}
	// cast to a special bag (return null even in the base bag class)
	virtual SDFClusterBag* asSpecialBag() { return 0;}

	// return some other cluster that can merge with me
	SDFCluster* mergeCandidate();

	// return an appropriate loop factor.  If doAnyLoop is 0,
	// it will apply heuristics to avoid large loop factors so
	// that loops will nest better.  If doAnyLoop is 1, it
	// will perform any legal looping.
	int loopFactor(int doAnyLoop);

	// change the loop value of the cluster, changing repetitions,
	// token numbers
	void loopBy(int);

	// undo looping of a cluster, changing repetitions, token numbers
	int unloop();

	// optionally do additional clustering on internal cluster
	virtual int internalClustering() { return FALSE;}

	// print me
	virtual ostream& printOn(ostream&) = 0;

	// print just name and looping
	ostream& printBrief(ostream&);

	// print ports
	ostream& printPorts(ostream&);

	// return the schedule
	virtual StringList displaySchedule(int depth) = 0;

	// generate code using target methods
	virtual void genCode(Target&, int depth) = 0;
};

// define << operator to use virt fn.
inline ostream& operator<<(ostream& o, SDFCluster& cl) {
	return cl.printOn(o);
}

// An atomic cluster surrounds an SDFStar, allowing a parallel hierarchy
// to be built up.
class SDFAtomCluster : public SDFCluster {
public:
	// The constructor turns the SDFStar into an atomic cluster.
	SDFAtomCluster(DataFlowStar& s,Galaxy* parent = 0);
	// destructor
	~SDFAtomCluster();

	// return my insides
	DataFlowStar& real() { return pStar;}

	// "pass-along" functions
	int run();
	void go() { run();}
	int myExecTime();

	// simulate execution of underlying real stars.
	// call simRunStar on the "real" star loop() times,
	// so info like the maximum # of tokens on each arc
	// will get set right.
	void simRunRealStars();

	// print me
	ostream& printOn(ostream&);

	// print my schedule
	StringList displaySchedule(int depth);

	// code generation
	void genCode(Target&, int depth);
private:
	DataFlowStar& pStar;
};

// An SDFBagScheduler is a modified SDFScheduler that lives in
// a SDFClusterBag.

class SDFBagScheduler : public SDFScheduler {
public:
	// return the schedule
	virtual StringList displaySchedule(int depth);

	// default version (since SDFScheduler has a no-argument version)
	StringList displaySchedule() { return displaySchedule(0);}

	// code generation
	virtual void genCode(Target&, int depth);
protected:
	// we permit disconnected galaxies.
	int checkConnectivity() { return TRUE;}

	// simulate execution of "real" stars to set geodesics right.
	void simRunRealStars();
};

// An SDFClusterBag is a composite object.  In some senses it is like
// a wormhole, but it's done in a simpler way to avoid all the overhead.
// Its Galaxy member contains a list of member SDFCluster objects.
class SDFClusterBag : public SDFCluster {
	friend class SDFClusterBagIter;
public:
	// constructor: makes an empty bag
	SDFClusterBag();
	// destructor deletes contents if owner is set
	~SDFClusterBag();

	// how many clusters
	int size() const { return gal ? gal->numberBlocks() : 0;}

	// simulate execution for schedule generation.  When executed
	// for the first time, my internal schedule is generated.
	// The effect is to call simRunStar on the "real" stars,
	// so info like the maximum # of tokens on each arc will
	// get set right.
	void simRunRealStars();

	// absorb adds the SDFCluster argument to the bag
	void absorb(SDFCluster*,SDFClusterGal*);

	// merge merges the argument bag with me and then deletes the
	// shell of the argument.
	void merge(SDFClusterBag*,SDFClusterGal*);

	// asBag returns myself, since I am a bag
	SDFClusterBag* asBag() { return this;}

	// return my galaxy.
	SDFClusterGal& myGal() { return *gal; }

	// print me
	ostream& printOn(ostream&);

	// print my schedule
	StringList displaySchedule(int depth);

	// code generation
	void genCode(Target&, int depth);

	// run the cluster, the number of times indicated by the loop
	// factor.
	int run();

	// a synonym
	void go() { run();}

	// do additional clustering on internal cluster (merge parallel
	// loops, for example)
	int internalClustering();

protected:
	virtual int genSched();

	// createInnerGal
	virtual void createInnerGal();

	SDFBagScheduler* sched;
	SDFClusterGal* gal;
	int exCount;
private:
	int owner;
};

class SDFClustPort : public SDFPortHole {
	friend class SDFCluster;
public:
	SDFClustPort(DFPortHole& p,SDFCluster* parent = 0,int bagp = 0);
	~SDFClustPort() {}
	DFPortHole& real() { return pPort;}
	int isItInput() const { return pPort.isItInput();}
	int isItOutput() const { return pPort.isItOutput();}
	void initGeo();
	SDFCluster* parentClust() { return (SDFCluster*)parent();}
	SDFClustPort* far() { return (SDFClustPort*)PortHole::far();}
	int numIO() const { return numberTokens;}
	SDFClustPort* outPtr() {
		return far() ? 0 : pOutPtr;
	}
	int feedForward() const { return feedForwardFlag;}
	void markFeedForward() { feedForwardFlag = 1;}

	// return TRUE if there is delay on the arc that may be a
	// problem for merging, and FALSE otherwise.
	int fbDelay() const { return (numTokens() > 0 && !feedForward());}

	// return the real far port alised to bagPorts.
	// If the bagPort is a port of the outsideCluster, return zero.
	SDFClustPort* realFarPort(SDFCluster* outsideCluster);

	void makeExternLink(SDFClustPort* val);

	SDFClustPort* inPtr() {
		return bagPortFlag ? (SDFClustPort*)&pPort : 0;
	}
private:
	DFPortHole& pPort;
	SDFClustPort* pOutPtr;
	unsigned char bagPortFlag;
	unsigned char feedForwardFlag;
	void loopBy(int fac) { numberTokens *= fac;}
	void unloopBy(int fac) { numberTokens /= fac;}
};

class SDFClustSched : public SDFBagScheduler {
public:
	// constructor and destructor
	SDFClustSched(const char* log = 0) : cgal(0),logFile(log) {}
	~SDFClustSched();

	// SDFBagScheduler allows disconnections, we do not.
	int checkConnectivity() {
		return SDFScheduler::checkConnectivity();
	}

	// Generate code using the Target to produce the right language.
	void compileRun();
protected:
	SDFClusterGal* cgal;
	const char* logFile;
	// this one does the main work.
	int computeSchedule (Galaxy& g);
};

class SDFClustPortIter : public BlockPortIter {
public:
	SDFClustPortIter(SDFCluster& s) : BlockPortIter(s) {}
	SDFClustPort* next() { return (SDFClustPort*)BlockPortIter::next();}
	SDFClustPort* operator++() { return next();}
};

class SDFClusterGalIter : public GalTopBlockIter {
public:
	SDFClusterGalIter(SDFClusterGal& g) : GalTopBlockIter(g) {}
	SDFCluster* next() { return (SDFCluster*)GalTopBlockIter::next();}
	SDFCluster* operator++() { return next();}
	GalTopBlockIter::reset;
};

class SDFClusterBagIter : public GalTopBlockIter {
public:
	SDFClusterBagIter(SDFClusterBag& b) : GalTopBlockIter(*(b.gal)) {}
	SDFCluster* next() { return (SDFCluster*)GalTopBlockIter::next();}
	SDFCluster* operator++() { return next();}
	GalTopBlockIter::reset;
};

#endif
