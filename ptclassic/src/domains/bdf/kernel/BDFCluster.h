// for now, this is a modified copy of SDFCluster.h.  We will try
// to merge the two later.
#ifndef _BDFCluster_h
#define _BDFCluster_h 1

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 7/6/92

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

The virtual baseclass BDFCluster refers to either type of cluster.

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

	// destructor zaps all the member clusters (inherited)
	~BDFClusterGal() {}

	// how many?
	int numberClusts() const { return numberBlocks();}

	// this is it!  The main clustering routine Returns TRUE
	// if any clustering is performed.
	int cluster();

	// return TRUE if all member clusters have the same rate
	int uniformRate();

	// generate schedules for clusters
	void genSubScheds();

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
	void dupStream(BDFClusterGal *pgal) { logstrm = pgal->logstrm;}

	// parent is also a BDFClusterGal
	BDFClusterGal* parentGal() { return (BDFClusterGal*)parent();}
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

	// function to attempt a combination merge and while-loop
	// 0 is returned on failure, the new merged cluster on success.
	BDFCluster* tryLoopMerge(BDFCluster*,BDFCluster*);

	// check needed by fullSearchMerge
	int buriedCtlArcs(BDFCluster*, BDFCluster*);

	// merge two clusters, returning the result.
	BDFCluster* merge(BDFCluster* c1,BDFCluster* c2);

	// do non-integral rate changes, if any
	int nonIntegralRateChange();

	// create a while loop
	void makeWhile(BDFClustPort*, BDFRelation);

	// generate a name for a new member ClusterBag
	const char* genBagName();

	// create a new arc to pass boolean information
	BDFClustPort* connectBoolean(BDFCluster* c,BDFClustPort* cond,
				     BDFRelation& rel);

	// return true if there is an indirect path between two
	// member clusters
	int indirectPath(BDFCluster* src,BDFCluster* dst,
			 int ignoreDelayArcs = FALSE);

	// function used in finding paths by indirectPath.
	int findPath(BDFCluster* src,BDFCluster* dst,
		     int ignoreDelayArcs = FALSE);

	// mark feed-forward delays
	int markFeedForwardDelayArcs();

	// make feedback arcs for while loops
	int markWhileFeedbackArcs();

private:
	int bagNumber;		// number for generating bag names
	int urateFlag;
	SequentialList stopList;// this list is used by fullSearchMerge.
};

enum BDFLoopType {
	DO_ITER, DO_IFTRUE, DO_IFFALSE, DO_UNTILTRUE, DO_UNTILFALSE
};

// This is the top-level cluster.  Just like BDFClusterGal but it has
// a pointer to a scheduler (for dynamic execution).

class BDFTopGal : public BDFClusterGal {
public:
	BDFTopGal(Galaxy& g, ostream* log = 0)
	: BDFClusterGal(g,log), sched(0) {}
	Scheduler* scheduler() const { return sched;}
	void setSched(Scheduler* s) { sched = s;}
private:
	Scheduler* sched;
};

// This is the baseclass for BDF cluster objects.  A cluster may have
// an internal galaxy (if it is an BDFClusterBag); it always has a loop
// count, indicating how many times it is to be looped.

class BDFCluster : public BDFStar {
protected:
	int pLoop;		// loop count
	BDFClustPort* pCond;	// condition
	BDFLoopType pType;	// type of "loop"
	short visitFlag;	// visit flag
	int dataIndependent();
public:
	// constructor: looping is 1 by default
	BDFCluster() : pLoop(1), pType(DO_ITER), pCond(0), visitFlag(0) {}

	// make destructor virtual
	virtual ~BDFCluster() {}

	void setVisit(int i) { visitFlag = i; }
	int  visited() { return visitFlag; }

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
private:
	DataFlowStar& pStar;
public:
	// The constructor turns the DataFlowStar into an atomic cluster.
	BDFAtomCluster(DataFlowStar& s,Galaxy* parent = 0);
	// destructor
	~BDFAtomCluster();

	// return my insides
	DataFlowStar& real() { return pStar;}

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
};

// An BDFBagScheduler is a modified BDFScheduler that lives in
// a BDFClusterBag.

class BDFBagScheduler : public SDFScheduler {
protected:
	// we permit disconnected galaxies.
	int checkConnectivity() { return TRUE;}
public:
	// return the schedule
	virtual StringList displaySchedule(int depth);

	// default version (since SDFScheduler has a no-argument version)
	StringList displaySchedule() { return displaySchedule(0);}

	// code generation
	virtual void genCode(Target&, int depth);
};

// An BDFClusterBag is a composite object.  In some senses it is like
// a wormhole, but it's done in a simpler way to avoid all the overhead.
// Its Galaxy member contains a list of member BDFCluster objects.
class BDFClusterBag : public BDFCluster {
	friend class BDFClusterBagIter;
public:
	// constructor: makes an empty bag
	BDFClusterBag();
	// destructor deletes contents if owner is set
	~BDFClusterBag();

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
	BDFClusterBag* asBag() { return this;}

	// return my galaxy.
	BDFClusterGal& myGal() { return *gal; }

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
private:
	int owner;
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
	BDFWhileLoop(BDFRelation t, BDFClustPort* cond,
		     BDFCluster* first,BDFCluster* second=0);
	// overrides of virtual functions
	ostream& printOn(ostream&);
	void runInner();
	StringList displaySchedule(int depth);
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
	BDFClustSched(const char* log = 0, int canDoDyn = TRUE);
	~BDFClustSched();

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
private:
	// The clustered galaxy.
	BDFClusterGal* cgal;
	// The dynamic scheduler
	DynDFScheduler* dynSched;
	// The log file name
	const char* logFile;
	// True if dynamic scheduling allowed.
	int dynamicAllowed;
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
