#ifndef _CGMacroCluster_h
#define _CGMacroCluster_h 1

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

 Programmer:  S. Ha

Given a galaxy, we create a parallel hierarchy called an CGMacroClusterGal.
Similar to SDFCluster.h except that cluster classes are derived from
CG star, which is necessary for APEG generation and parallel scheduling.
It implies unfortunate code duplication! Maybe, we should have
multiple inheritance.

In the parallel hierarchy, objects of class CGAtomCluster correspond
to the individual stars of the original galaxy; each CGAtomCluster contains
a reference to an CGStar.  The clustering algorithm transforms the
CGMacroClusterGal into a set of nested clusters.  class CGMacroClusterBag
represents a composite cluster with its own schedule; it resembles a
wormhole in some respects but is simpler (it does not use a pair of
event-horizon objects and does not have a Target; it does have a contained
galaxy and a type of Scheduler).

Cluster boundaries are represented by disconnected portholes;
SDFScheduler treats such portholes exactly as if they were wormhole
boundaries.

The virtual baseclass CGMacroCluster refers to either type of cluster.

The remaining class defined here is CGClustPort, a porthole for use
in CGMacroCluster objects.

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"
#include "DynamicGalaxy.h"
#include "SDFScheduler.h"
#include "Profile.h"
#include "MultiTarget.h"
#include "MacroScheduler.h"

class CGMacroCluster;
class CGMacroClusterBag;
class ostream;

// A CGMacroClusterGal is a galaxy made up of CGMacroCluster objects.  When
// constructed, it is a flat galaxy with one atomic cluster corresponding
// to each star in the original galaxy.  The cluster() function introduces
// hierarchy, using the merge/loop clustering algorithm.

class CGMacroClusterGal : public DynamicGalaxy {
friend class CGMacroClusterBag;
public:
	// constructor creates flat galaxy of atomic clusters
	CGMacroClusterGal(Galaxy*,ostream* log = 0);
	CGMacroClusterGal(CGMacroClusterBag* c, ostream* log = 0);

	// destructor zaps all the member clusters (inherited)
	~CGMacroClusterGal() {}

	// how many?
	int numberClusts() const { return numberBlocks();}

	// this is it!  The main clustering routine Returns TRUE
	// if any clustering is performed.
	int cluster();

	// return TRUE if all member clusters have the same rate
	int uniformRate();

	// clustering galaxy
	int clusterGalaxy(Galaxy&);

        // do global clustering. Perform the bottommost clustering first
        int globalCluster();

	// merge any "parallel loops".  Return TRUE if a change was made
	int parallelLoopMergePass();

        // recursive parallel scheduling from inside clusters.
        int recursiveSched(MultiTarget* t);

	// return galaxy
	Galaxy* realGal() { return myGal; }

	// set log stream member to match that of another (parent)
	// galaxy
	void dupStream(CGMacroClusterGal *pgal) { logstrm = pgal->logstrm;}

protected:
	// do a loop pass.  Return TRUE if a change was made
	int loopPass();

	// do a merge pass.  Return TRUE if a change was made
	int mergePass();

	ostream* logstrm;	// for logging errors
	Galaxy* myGal;
	CGMacroClusterBag* owner;

	// core of clustering routine; returns TRUE if any clustering
	// is performed and sets uniformRate if all clusters/actors have
	// the same rate.
	int clusterCore(int& uniformRate);

	// function to do an "expensive merge" -- it does the full
	// search for indirect paths.  If it finds a mergeable pair
	// it merges it and returns the result; otherwise it returns 0.
	CGMacroCluster* fullSearchMerge();

	// merge two clusters, returning the result.
	CGMacroCluster* merge(CGMacroCluster* c1,CGMacroCluster* c2);

	// There might be more restriction for merging two clusters.
	int canMerge(CGMacroCluster* c1, CGMacroCluster* c2); 

	// virtual method to create a cluster-bag
	virtual CGMacroClusterBag* createBag();

	// generate a name for a new member ClusterBag
	const char* genBagName();

	// return true if there is an indirect path between two
	// member clusters
	int indirectPath(CGMacroCluster* src,CGMacroCluster* dst);

	// function used in finding paths by indirectPath.
	int findPath(CGMacroCluster* src,CGMacroCluster* dst);

	// mark feed-forward delays
	int markFeedForwardDelayArcs();

	// interior portions of loop pass
	int integralLoopPass(int doAnyLoop);
	int loopTwoClusts();

	// pass to do non-integral rate changes if structure is a tree.
	int tryTreeLoop();

	// predicate to determine whether cluster has a tree topology
	int isTree();

        // create a CGMacroClusterBag if not created yet. If already created,
        // return it.
        virtual CGMacroClusterBag* createGalBag(Galaxy* g);

private:
	int bagNumber;		// number for generating bag names
	SequentialList stopList;// this list is used by fullSearchMerge.

        // data structure to identify the component galaxies
        SequentialList targetGalList;   // target for clustering
        SequentialList bagList;         // list of galaxy clusters

	// select the target galaxies for clustering
	void selectTargetGals(Galaxy& gal);

        // incremental search to make a galaxy cluster around the argument
        // cluster and the argument cluster bag
        void incrementalSearch(CGMacroCluster* c, CGMacroClusterBag* mg);

        // Detect all strongly connected components and cluster them
        // to decompose the galaxy. return TRUE if find any.
        int clusterSCC();

        // Make clusters of strongly connected graph
        void makeSCCCluster(SequentialList*);

        // The nodes between the stop node and the start node are
        // strongly connected. We trace back the nodes from the start node
        // to the stop node to identify those nodes.
        SequentialList* makeList(CGMacroCluster* start, CGMacroCluster* stop);

        // find a strongly connected component
        // depth-first search.
        SequentialList* findSCComponent(CGMacroCluster* c);
};

// This is the baseclass for CG cluster objects.  A cluster may have
// an internal galaxy (if it is an CGMacroClusterBag); it always has a loop
// count, indicating how many times it is to be looped.

class CGMacroCluster : public CGStar {
protected:
	int pLoop;		// loop count
	int visitFlag;		// visit flag
	int stickyFlag;		// indicate that this cluster is not
				// parallelizable.
public:
	// constructor: looping is 1 by default, and not parallelizable.
	CGMacroCluster() : pLoop(1), stickyFlag(1) {}

	// make destructor virtual
	virtual ~CGMacroCluster() {}

	void setVisit(int i) { visitFlag = i; }
	int  visited() { return visitFlag; }

	// return loop count
	int loop() const { return pLoop;}

	// cast to a bag (return null if not a bag)
	virtual CGMacroClusterBag* asBag() { return 0;}
	// cast to a special bag (return null even in the base bag class)
	virtual CGMacroClusterBag* asSpecialBag() { return 0;}
	// if idFlag is reset, the bag is regarded as an atomic cluster.

	// fix internal buffer sizes.  Arg is #times cluster is executed.
	virtual void fixBufferSizes(int) = 0;

	// return some other cluster that can merge with me
	CGMacroCluster* mergeCandidate();

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

	// the real name of the original star
	virtual const char* realName() const = 0;

	// print me
	virtual ostream& printOn(ostream&) = 0;

	// print just name and looping
	ostream& printBrief(ostream&);

	// print ports
	ostream& printPorts(ostream&);

	// indicate whether this cluster is parallelizable or not
	virtual int isParallelizable() { return !stickyFlag; }
	void setStickyFlag(int i) { stickyFlag = i; }
};

// define << operator to use virt fn.
inline ostream& operator<<(ostream& o, CGMacroCluster& cl) {
	return cl.printOn(o);
}

// An atomic cluster surrounds an CGStar, allowing a parallel hierarchy
// to be built up.
class CGAtomCluster : public CGMacroCluster {
public:
	// The constructor turns the CGStar into an atomic cluster.
	CGAtomCluster(CGStar& s, Galaxy* parent = 0);
	// destructor
	~CGAtomCluster();

	// return my insides
	CGStar& real() { return pStar;}

	// isA function
	/* virtual */ int isA(const char* cname) const;

	// clone method
	/* virtual */ Block* makeNew() const { return pStar.makeNew(); }

	// set procId: write-through
	/* virtual */ void setProcId(int i) 
		{ procId = i; pStar.setProcId(i);}

	/* virtual */ int myExecTime();

	// parallelizable if it has no internal state
	/* virtual */ int isParallelizable() 
		{ return !pStar.hasInternalState(); }

	// set buffer sizes of the real star.
	void fixBufferSizes(int);

	// print name
	/* virtual */ const char* realName() const { return pStar.name(); }
	/* virtual */ const char* readTypeName() 
		{ return pStar.readTypeName(); }

	// print me
	ostream& printOn(ostream&);

private:
	CGStar& pStar;
};

// An CGMacroClusterBag is a composite object.  In some senses it is like
// a wormhole, but it's done in a simpler way to avoid all the overhead.
// Its Galaxy member contains a list of member CGMacroCluster objects.
class CGMacroClusterBag : public CGMacroCluster {
	friend class CGMacroClusterBagIter;
public:
	// constructor: makes an empty bag
	CGMacroClusterBag(Galaxy* g = 0);
	// destructor deletes contents if owner is set
	~CGMacroClusterBag();

	// how many clusters
	int size() const { return gal ? gal->numberBlocks() : 0;}

	// redefine
	/* virtual */ int myExecTime();

	// set info on maximum # of tokens on each arc.
	void fixBufferSizes(int);

	// absorb adds the CGMacroCluster argument to the bag
	void absorb(CGMacroCluster*,CGMacroClusterGal*);

	// merge merges the argument bag with me and then deletes the
	// shell of the argument.
	void merge(CGMacroClusterBag*,CGMacroClusterGal*);

	// asBag returns myself, since I am a bag
	// if idFlag is reset, the bag is regarded as an atomic cluster.
	CGMacroClusterBag* asSpecialBag() { return this; }
	CGMacroClusterBag* asBag() { return idFlag? this: 0;}
	void resetId() { idFlag = 0; }

	// declare I am potentially parallel
	/* virtual */ int isParallel() const { return TRUE; }

	// get the profile
	/* virtual */ Profile* getProfile(int i);

	// return my galaxy.
	CGMacroClusterGal* clusterGal() { return gal; }

	// return the real galaxy if it is a galaxy cluster
	Galaxy* real() { return pGal; }

	// print name
	/* virtual */ const char* realName() const { 
		if (pGal) return pGal->name(); 
		else return name();
	}

	// return scheduler
	MacroParSched* parSched() { return sched; }

	// print me
	ostream& printOn(ostream&);

	// cluster the inside galaxy
	virtual int clusterMyGal();

	// adjust number of tokens
	virtual int prepareBag(MultiTarget*, ostream*);

	// parallel schedule
	virtual int parallelSchedule(MultiTarget* t);

	// compute the optimal number of assigned processors
	virtual int computeProfile(int nP, int rW, IntArray* avail);

	// schedule the inside manually
	// the argument is the repetition number of the cluster
	virtual Profile* manualSchedule(int);

	// final schedule of the inside galaxy
	virtual int finalSchedule(int numProcs);

	// print my schedule
	virtual StringList displaySchedule();

	// download code
	virtual void downLoadCode(CGStar*, int invoc, int pix);
	
	// do additional clustering on internal cluster (merge parallel
	// loops, for example)
	int internalClustering();

	// parallelism
	int howMany() { return maxParallelism; }

	// final number of assigned processors
	int finalNum() { return optNum; }

protected:
	// createInnerGal
	void createInnerGal();

	// recursively prepare bags
	int recurPrepareBag(MultiTarget*, ostream*);

	// for manual scheduling: check the procId property of inside stars
	int examineProcIds(IntArray& procId);

	int maxParallelism;

	CGMacroClusterGal* gal;
	Galaxy* pGal;
	MultiTarget* mtarget;

	int owner;
	int exCount;
	int optNum;	// optimal number of assigned processors
	int numProcs;	// total number of processors

private:
        MacroParSched* sched;        // parallel scheduler

	int idFlag;
};

class CGClustPort : public CGPortHole {
	friend class CGMacroCluster;
public:
	CGClustPort(DFPortHole& p,CGMacroCluster* parent = 0,int bagp = 0);
	~CGClustPort() {}
	DFPortHole& real() { return pPort;}
	int isItInput() const { return pPort.isItInput();}
	int isItOutput() const { return pPort.isItOutput();}
	void initGeo();
	CGMacroCluster* parentClust() { return (CGMacroCluster*)parent();}
	CGClustPort* far() { return (CGClustPort*)PortHole::far();}
	int numIO() const { return numberTokens;}
	CGClustPort* outPtr() {
		return far() ? 0 : pOutPtr;
	}
	int feedForward() const { return feedForwardFlag;}
	void markFeedForward() { feedForwardFlag = 1;}

	// return TRUE if there is delay on the arc that may be a
	// problem for merging, and FALSE otherwise.
	int fbDelay() const { return (numTokens() > 0 && !feedForward());}

	// return the real far port alised to bagPorts.
	// If the bagPort is a port of the outsideCluster, return zero.
	CGClustPort* realFarPort(CGMacroCluster* outsideCluster);

	void makeExternLink(CGClustPort* val);

	CGClustPort* inPtr() {
		return bagPortFlag ? (CGClustPort*)&pPort : 0;
	}
	// set arc count ... go inside until one with a geo is found
	void setMaxArcCount(int n);
	// get arc count .. go outside until one with a geo is found
	int maxArcCount();

	void loopBy(int fac) { numberTokens *= fac;}
	void unloopBy(int fac) { numberTokens /= fac;}
	// don't like this being public, but...
	void setNumXfer(int v) { numberTokens = v;}
private:
	DFPortHole& pPort;
	CGClustPort* pOutPtr;
	unsigned char bagPortFlag;
	unsigned char feedForwardFlag;
};

class CGClustPortIter : public BlockPortIter {
public:
	CGClustPortIter(CGMacroCluster& s) : BlockPortIter(s) {}
	CGClustPort* next() { return (CGClustPort*)BlockPortIter::next();}
	CGClustPort* operator++(POSTFIX_OP) { return next();}
};

class CGMacroClusterGalIter : public GalTopBlockIter {
public:
	CGMacroClusterGalIter(CGMacroClusterGal& g) : GalTopBlockIter(g) {}
	CGMacroCluster* next() { return (CGMacroCluster*)GalTopBlockIter::next();}
	CGMacroCluster* operator++(POSTFIX_OP) { return next();}
	GalTopBlockIter::reset;
};

class CGMacroClusterBagIter : public GalTopBlockIter {
public:
	CGMacroClusterBagIter(CGMacroClusterBag& b) : GalTopBlockIter(*(b.gal)) {}
	CGMacroCluster* next() { return (CGMacroCluster*)GalTopBlockIter::next();}
	CGMacroCluster* operator++(POSTFIX_OP) { return next();}
	GalTopBlockIter::reset;
};

#endif
