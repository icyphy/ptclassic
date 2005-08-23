#ifndef _BDFCluster_h
#define _BDFCluster_h 1

/**************************************************************************
Copyright (c) 1990-1996 The Regents of the University of California.
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

Author:  J. Buck
Created: 7/6/92
Version: @(#)BDFCluster.h	2.35	3/8/96

(for now, this is a modified copy of SDFCluster.h.  We will try
to merge the two later.)

This file defines all the classes for a clustering BDF scheduler.

Given a galaxy, we create a parallel hierarchy called an BDFClusterGal.
In the parallel hierarchy, objects of class BDFAtomCluster correspond
to the individual stars of the original galaxy; each BDFAtomCluster contains
a reference to a DataFlowStar.  The clustering algorithm transforms the
BDFClusterGal into a set of nested clusters.  class BDFClusterBag
represents a composite cluster with its own schedule; it resembles a
wormhole in some respects but is simpler (it does not use a pair of
event-horizon objects and does not have a Target; it does have a contained
galaxy and a type of BDFScheduler).

Cluster boundaries are represented by disconnected portholes;
BDFScheduler treats such portholes exactly as if they were wormhole
boundaries.

The virtual base class BDFCluster refers to either type of cluster.

Two specialized types of scheduler, both derived from BDFScheduler,
are used; BDFBagScheduler is used for schedules inside BDFClusterBags,
and BDFClustSched is used as the top-level scheduler.

The remaining class defined here is BDFClustPort, a porthole for use
in BDFCluster objects.

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "BDFStar.h"
#include "DynamicGalaxy.h"
#include "BDFScheduler.h"

class BDFCluster;
class BDFClustPort;
class BDFClusterBag;
class ostream;

// A BDFClusterGal is a galaxy made up of BDFCluster objects.  When
// constructed, it is a flat galaxy with one atomic cluster corresponding
// to each star in the original galaxy.  The cluster() function introduces
// hierarchy, using the merge/loop clustering algorithm.

class BDFClusterGal : public DynamicGalaxy {
public:
	// constructor creates flat galaxy of atomic clusters
	BDFClusterGal(Galaxy&,ostream* log = 0);
	BDFClusterGal(ostream* log = 0)
	: logstrm(log), bagNumber(-1), urateFlag(FALSE) {}

	// inherit virtual destructor, which zaps inherited data members
	/* virtual */ ~BDFClusterGal() {}

        // remove all blocks in this galaxy without deallocating the blocks
	void orphanBlocks();

	// how many?
	inline int numberClusts() const { return numberBlocks();}

	// this is it!  The main clustering routine Returns TRUE
	// if any clustering is performed.
	int cluster();

	// return TRUE if all member clusters have the same rate
	int uniformRate();

	// I would prefer for these to be protected rather than public.

	// do a loop pass.  Return TRUE if a change was made
	int loopPass();

	// do a merge pass.  Return TRUE if a change was made
	int mergePass();

	// merge any "parallel loops".  Return TRUE if a change was made
	int parallelLoopMergePass();

	// set log stream member to match that of another (parent) galaxy
	inline void dupStream(BDFClusterGal* pgal) { logstrm = pgal->logstrm; }

	// generate schedules for interior clusters
	void genSubScheds();

	// Note: The remaining public methods are not in SDFClusterGal

	// parent is also a BDFClusterGal
	BDFClusterGal* parentGal() { return (BDFClusterGal*)parent();}

        // remove blocks from this galaxy without deallocating the blocks
	void orphanPorts();

protected:
	ostream* logstrm;	// for logging errors

	// core of clustering routine; returns TRUE if any clustering
	// is performed and sets uniformRate if all clusters/actors have
	// the same rate.
	int clusterCore(int& uniformRate);

	// function to do an "expensive merge" -- it does the full
	// search for indirect paths.  If it finds a mergeable pair
	// it merges it and returns the result; otherwise it returns 0.
	BDFCluster* fullSearchMerge();

	// merge two clusters, returning the result.
	BDFCluster* merge(BDFCluster* c1,BDFCluster* c2);

	// generate a name for a new member ClusterBag
	const char* genBagName();

	// return true if there is an indirect path between two
	// member clusters
	int indirectPath(BDFCluster* src, BDFCluster* dst,
			 int ignoreDelayArcs = FALSE);

	// function used in finding paths by indirectPath.
	int findPath(BDFCluster* src,BDFCluster* dst,
		     int ignoreDelayArcs = FALSE);

	// mark feed-forward delays
	int markFeedForwardDelayArcs();

	// Note: The remaining protected methods are not in SDFClusterGal

	// make feedback arcs for while loops
	int markWhileFeedbackArcs();

	// function to attempt a combination merge and while-loop
	// 0 is returned on failure, the new merged cluster on success.
	BDFCluster* tryLoopMerge(BDFCluster*,BDFCluster*);

	// check needed by fullSearchMerge
	int buriedCtlArcs(BDFCluster*, BDFCluster*);

	// do non-integral rate changes, if any
	int nonIntegralRateChange();

	// create a while loop
	void makeWhile(BDFClustPort*, BDFRelation);

	// create a new arc to pass boolean information
	BDFClustPort* connectBoolean(BDFCluster* c,BDFClustPort* cond,
				     BDFRelation& rel);

private:
	int bagNumber;			// number for generating bag names
	SequentialList stopList;	// list used by fullSearchMerge.
	int urateFlag;			// uniform rate flag (not in SDF)
};

enum BDFLoopType {
	DO_ITER, DO_IFTRUE, DO_IFFALSE, DO_UNTILTRUE, DO_UNTILFALSE
};

// This is the top-level cluster.  Just like BDFClusterGal but it has
// a pointer to a scheduler (for dynamic execution).

class BDFTopGal : public BDFClusterGal {
public:
	// constructor
	BDFTopGal(Galaxy& g, ostream* log = 0) :
		BDFClusterGal(g,log), sched(0) {}

	// destructor
	/* virtual */ ~BDFTopGal() {}

	Scheduler* scheduler() const { return sched;}
	inline void setSched(Scheduler* s) { sched = s;}

private:
	// not allocated by TopGal
	Scheduler* sched;
};

// This is the baseclass for BDF cluster objects.  A cluster may have
// an internal galaxy (if it is an BDFClusterBag); it always has a loop
// count, indicating how many times it is to be looped.

class BDFCluster : public BDFStar {
protected:
	int pLoop;		// loop count
	int visitFlag;		// visit flag
	BDFClustPort* pCond;	// condition
	BDFLoopType pType;	// type of "loop"
	int dataIndependent();

public:
	// constructor: looping is 1 by default
	BDFCluster() : pLoop(1), visitFlag(0), pCond(0), pType(DO_ITER) {}

	// destructor is virtual so that delete p works properly when p is
	// either a BDFCluster* or a pointer to derived class of BDFCluster
	/* virtual */ ~BDFCluster() { deleteAllGenPorts(); }

	inline void setVisit(int i) { visitFlag = i; }
	inline int  visited() { return visitFlag; }

	// return true if my sample rate matches that of all my neighbors
	int matchNeighborRates();

	// return loop count
	int loop() const { return pLoop;}

	// return true if two clusters have the same looping
	// condition
	int sameLoopCondition(const BDFCluster& arg) const;

	// return true if I am looped (by iteration or if-ing)
	int looped() const { return pLoop > 1 || pType != DO_ITER;}

	// cast to a bag (return null if not a bag)
	virtual BDFClusterBag* asBag() { return 0;}

	// return some other cluster that can merge with me
	BDFCluster* mergeCandidate();

	// if this cluster can be turned into a while loop, return the
	// port that produces the control signal for the loop.  The
	// reference argument returns the termination condition.
	BDFClustPort* canBeWhiled(BDFRelation& rel);

	// return an appropriate loop factor: 0 if none.
	int loopFactor();

	// change the loop value of the cluster, changing repetitions,
	// token numbers
	void loopBy(int);

	// return an appropriate if factor.  Return null if none, otherwise
	// return the boolean condition and relation (BDF_TRUE or BDF_FALSE).
	BDFClustPort* ifFactor(BDFRelation&);

	// convert the cluster to a conditional cluster, to be executed
	// only on the given condition.
	void ifIze(BDFClustPort* condition,BDFRelation rel,ostream*);

	// undo looping of a cluster, changing repetitions, token numbers
	BDFClustPort* unloop(int& reps,BDFLoopType& lcond);

	// redo looping of a cluster -- reverse unloop.
	void reloop(int reps,BDFLoopType lcond,BDFClustPort* pcond);

	// optionally do additional clustering on internal cluster
	virtual int internalClustering() { return FALSE;}

	// print me
	virtual ostream& printOn(ostream&) = 0;

	// print just name and looping
	ostream& printBrief(ostream&);

	// print ports
	ostream& printPorts(ostream&);

	// generate the schedule (does nothing except for bags)
	virtual int genSched() { return TRUE;}

	// generate buffer sizes.  Arg is #times cluster is executed
	// (for constant repetitions)
	virtual void fixBufferSizes(int) = 0;

	// execute the cluster
	void go();

	// inside-run of cluster
	virtual void runInner() = 0;

	// return the schedule
	virtual StringList displaySchedule(int depth) = 0;

	// generate code
	virtual void genCode(Target&, int depth) = 0;

	// generate code for loop beginning and ending
	// by default, no code.
	virtual void genLoopInit(Target&, int) {}
	virtual void genLoopEnd(Target&) {}

	// static function to make temporary names
	static const char* mungeName(NamedObj&);
};

// define << operator to use virtual printing function.
inline ostream& operator<<(ostream& o, BDFCluster& cl) {
	return cl.printOn(o);
}

// An atomic cluster surrounds a DataFlowStar, allowing a parallel hierarchy
// to be built up.
class BDFAtomCluster : public BDFCluster {
public:
	// The constructor turns the DataFlowStar into an atomic cluster.
	BDFAtomCluster(DataFlowStar& s,Galaxy* parent = 0);

	// destructor
	/* virtual */ ~BDFAtomCluster();

	// return my insides
	inline DataFlowStar& real() { return pStar; }

	// execute the cluster's "inside" (called by BDFCluster::go)
	// the number of times indicated by loop().
	void runInner();

	// wrapup: only used for dynamic execution
	void wrapup();

	// time reqd
	int myExecTime();

	// print me
	ostream& printOn(ostream&);

	// print my schedule
	StringList displaySchedule(int depth);

	// code generation
	void genCode(Target&, int depth);

	// code generation for loop beginning and end.
	void genLoopInit(Target&, int reps);
	void genLoopEnd(Target&);

	// set buffer sizes
	void fixBufferSizes(int nReps);

private:
	DataFlowStar& pStar;
};

// An BDFBagScheduler is a modified BDFScheduler that lives in
// a BDFClusterBag.

class BDFBagScheduler : public SDFScheduler {
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
};

// An BDFClusterBag is a composite object.  In some senses it is like
// a wormhole, but it's done in a simpler way to avoid all the overhead.
// Its Galaxy member contains a list of member BDFCluster objects.
class BDFClusterBag : public BDFCluster {
	friend class BDFClusterBagIter;
public:
	// constructor: makes an empty bag
	BDFClusterBag();

	// destructor
	/* virtual */ ~BDFClusterBag();

	// how many clusters
	int size() const { return gal ? gal->numberBlocks() : 0;}

	// createScheduler
	virtual void createScheduler();

	// absorb adds the BDFCluster argument to the bag
	void absorb(BDFCluster*,BDFClusterGal*);

	// merge merges the argument bag with me and then deletes the
	// shell of the argument.
	void merge(BDFClusterBag*,BDFClusterGal*);

	// asBag returns myself, since I am a bag
	inline BDFClusterBag* asBag() { return this; }

	// return my galaxy.
	inline BDFClusterGal& myGal() { return *gal; }

	// print me
	ostream& printOn(ostream&);

	// generate my schedule
	int genSched();

	// compute buffer sizes.
	void fixBufferSizes(int nReps);

	// print my schedule
	StringList displaySchedule(int depth);

	// code generation
	void genCode(Target&, int depth);

	// execute the cluster's "inside" (called by BDFCluster::go)
	// the number of times indicated by loop().
	void runInner();

	// wrapup: only used for dynamic execution
	void wrapup();

	// do additional clustering on internal cluster (merge parallel
	// loops, for example)
	int internalClustering();

	// return my scheduler
	Scheduler* scheduler() const { return sched;}

protected:
	// createInnerGal
	virtual void createInnerGal();

	// adjust associated booleans after merging
	void adjustAssociations();

	BDFBagScheduler* sched;
	BDFClusterGal* gal;
	int exCount;

	void moveClusterInto(BDFCluster* c, BDFClusterGal* par);
};

// a BDFWhileLoop is a cluster that represents data-dependent iteration.
// It contains either one or two subclusters.  The execution of the cluster
// always looks like the C pseudocode
//	boolean dummy;
//	do {
//		execute cluster A;
//		dummy = token from (some output port of A);
//		execute cluster B; // if there is no B, ignore this line
//	} while (dummy has the correct value);
// pCond has the conditional port, and the code is DO_UNTILTRUE or
// DO_UNTILFALSE.

class BDFWhileLoop : public BDFCluster {
public:
	// constructor
	BDFWhileLoop(BDFRelation t, BDFClustPort* cond,
		     BDFCluster* first,BDFCluster* second=0);

	// overrides of virtual functions
	ostream& printOn(ostream&);
	void runInner();
	StringList displaySchedule(int depth);
	int genSched();
	void genCode(Target&, int depth);
	void fixBufferSizes(int);

protected:
	void setup();

private:
	// fix arcs of member clusters.
	void fixArcs(BDFCluster*,BDFCluster*);
	BDFCluster* a;		// source of boolean
	BDFCluster* b;		// sink of boolean, or null
};

class DynDFScheduler;

// This is the clustering BDF scheduler class.

class BDFClustSched : public BDFScheduler {
public:
	// constructor and destructor
	BDFClustSched(const char* log = 0, int canDoDyn = TRUE,
		      int constCheck = FALSE);

	/* virtual */ ~BDFClustSched();

	// return the schedule
	StringList displaySchedule();

	// run the schedule
	int run();

	// Generate code using the Target to produce the right language.
	void compileRun();

	// we redefine the timing control funcs to pass the stop time
	// along to the (optional) inner dynamic scheduler.
	void setStopTime (double);
	void resetStopTime (double);

protected:
	// this one does the main work.
	int computeSchedule (Galaxy&);
	// run schedule
	void runOnce();
	// handle creation of dynamic scheduler if needed and allowed
	int handleDynamic (BDFTopGal&);
	// compute repetitions: skip if no check.
	int repetitions() {
		return strongConstCheck ? BDFScheduler::repetitions()
			: TRUE;
	}

private:
	// The clustered galaxy.
	BDFTopGal* cgal;
	// The dynamic scheduler
	DynDFScheduler* dynSched;
	// The log file name
	const char* logFile;
	// True if dynamic scheduling allowed.
	short dynamicAllowed;
	// True if "strongly consistent" check performed.
	short strongConstCheck;
};

// standard iterators
class BDFClustPortIter : public BlockPortIter {
public:
	BDFClustPortIter(BDFCluster& s) : BlockPortIter(s) {}
	BDFClustPort* next() { return (BDFClustPort*)BlockPortIter::next();}
	BDFClustPort* operator++(POSTFIX_OP) { return next();}
};

class BDFClusterGalIter : public GalTopBlockIter {
public:
	BDFClusterGalIter(BDFClusterGal& g) : GalTopBlockIter(g) {}
	BDFCluster* next() { return (BDFCluster*)GalTopBlockIter::next();}
	BDFCluster* operator++(POSTFIX_OP) { return next();}
	GalTopBlockIter::reset;
};

class BDFClusterBagIter : public GalTopBlockIter {
public:
	BDFClusterBagIter(BDFClusterBag& b) : GalTopBlockIter(*(b.gal)) {}
	BDFCluster* next() { return (BDFCluster*)GalTopBlockIter::next();}
	BDFCluster* operator++(POSTFIX_OP) { return next();}
	GalTopBlockIter::reset;
};

#endif
