/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 7/6/92

This file defines all the classes for a clustering BDF scheduler.
This one is a modified copy of SDFCluster.cc, later we will merge
them.

Given a galaxy, we create a parallel hierarchy called an BDFClusterGal.
In the parallel hierarchy, objects of class BDFAtomCluster correspond
to the individual stars of the original galaxy; each BDFAtomCluster contains
a reference to an BDFStar.  The clustering algorithm transforms the
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
static const char file_id[] = "BDFCluster.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "BDFCluster.h"
#include "GalIter.h"
#include "Geodesic.h"
#include "Plasma.h"
#include "Target.h"
#include "streamCompat.h"
#include "Error.h"

// common tests on BDFRelations
inline int TorF(BDFRelation r) { return r == BDF_TRUE || r == BDF_FALSE;}
inline int SorC(BDFRelation r) { return r == BDF_SAME || r == BDF_COMPLEMENT;}

// classes used to mark ports that need remapping.

class BDFremap {
	friend class BDFRemapList;
	BDFClustPort* orig;
	BDFClustPort* remap;
	BDFRelation rel;
	BDFremap(BDFClustPort* o,BDFClustPort* r, BDFRelation rr)
	: orig(o), remap(r), rel(rr) {}
};

class BDFRemapList : public SequentialList
{
public:
	BDFRemapList() {}
	void put(BDFremap& p) { SequentialList::put(&p);}
	void add(BDFClustPort* o,BDFClustPort* r,BDFRelation re = BDF_SAME);
	BDFClustPort* lookup(BDFClustPort* o,BDFRelation&);
	void initialize();
	~BDFRemapList() { initialize();}
};

// Having this object makes this non-reentrant.  It contains remappings
// for buried control arcs.

static BDFRemapList remaps;

// A BDFClusterGal is a Galaxy, built from another galaxy.

// function to print an BDFClusterGal on an ostream.
ostream& operator<< (ostream& o, BDFClusterGal& g) {
	BDFClusterGalIter next(g);
	BDFCluster* c;
	while ((c = next++) != 0) {
		o << *c << "\n";
	}
	return o;
}

// constructor: builds a flat galaxy of BDFAtomCluster objects.
// assumption: the initial galaxy has only SDF and BDF stars.

// We use a table, based on the indices, to map portholes back to
// their corresponding BDFClustPorts and connect it all up.

BDFClusterGal::BDFClusterGal(Galaxy& gal, ostream* log)
: bagNumber(1), logstrm(log)
{
	int nports = setPortIndices(gal);
	LOG_NEW; BDFClustPort** ptable = new BDFClustPort*[nports];
	GalStarIter nextStar(gal);
	DataFlowStar* s;
	BDFCluster* c;
	while ((s = (DataFlowStar*)nextStar++) != 0) {
		LOG_NEW; c = new BDFAtomCluster(*s,this);
		addBlock(*c);
		BDFClustPortIter nextPort(*c);
		BDFClustPort *p;
		while ((p = nextPort++) != 0) {
			ptable[p->real().index()] = p;
		}
	}
	// now connect up the cluster ports to match the real ports
	for (int i = 0; i < nports; i++) {
		BDFClustPort* curCP = ptable[i];

		// create corresponding assocPort relations, if they exist.
		DFPortHole& realP = curCP->real();
		PortHole* a = realP.assocPort();
		if (a) {
			// find the ClustPort that corresponds to a, and
			// set me to be associated with that.
			BDFClustPort* assoc = ptable[a->index()];
			int r = realP.assocRelation();
			curCP->setRelation(r, assoc);

			// flag the associated port if it controls
			// the dataflow of another port

			if (TorF(r)) assoc->setControl(TRUE);
		}
		if (curCP->isItOutput()) {
			// only one connection for each pair, hence
			// the isItOutput check.
			BDFClustPort* in = ptable[realP.far()->index()];
			int delay = realP.numTokens();
			curCP->connect(*in,delay);
			curCP->initGeo();
		}
	}
	LOG_DEL; delete ptable;
}

// Core clustering routine.  Alternate merge passes and loop passes
// until no more can be done.
int BDFClusterGal::clusterCore(int& urate) {
	int change = FALSE;
	while ((urate = uniformRate()) == FALSE) {
                int status = mergePass();
                status |= loopPass();
		if (logstrm) logstrm->flush();
                if (status) change = TRUE;
                else break;
	}
	if (numberClusts() == 1) {
		if (logstrm)
			*logstrm << "Reduced to one cluster\n";
	}
	else if (urate) {
		if (logstrm)
			*logstrm << "Uniform rate achieved: one extra merge pass now\n";
		if (mergePass()) change = TRUE;
	}
	return change;
}

// Top level of clustering algorithm.  Here's the outline.
// 1. Do a clusterCore pass.
// 2. If not reduced to uniform rate, mark feedforward delays as ignorable
//    and do another clusterCore pass.
// 3. Do a pass that merges parallel loops with the same rate.

int BDFClusterGal::cluster() {
	int urate;
	int change = clusterCore(urate);

	// if we did not get to uniform rate, try marking feedforward
	// delay arcs as ignorable and clustering some more.

	if (!urate && markFeedForwardDelayArcs()) {
		change |= clusterCore(urate);
	}
//	if (!urate)
//		Error::warn("clustering algorithm did not achieve uniform rate");
	if (change) {
		if (logstrm)
			*logstrm << "Looking for parallel loops\n";
		parallelLoopMergePass();
	}
	return change;
}

// Generate schedules in all the lower-level clusters.
int BDFClusterGal::genSubScheds() {
	BDFClusterGalIter nextClust(*this);
	BDFCluster* c;
	while ((c = nextClust++) != 0)
		if (!c->genSched()) return FALSE;
	return TRUE;
}

// Test to see if we have uniform rate.
int BDFClusterGal::uniformRate() {
	if (numberClusts() <= 1) return TRUE;
	BDFClusterGalIter nextClust(*this);
	BDFCluster* c;
	while ((c = nextClust++) != 0) {
		if (!c->matchNeighborRates()) return FALSE;
	}
	return TRUE;
}

// Merge adjacent actors that can be treated as a single cluster.
int BDFClusterGal::mergePass() {
	if (numberClusts() <= 1) return FALSE;
	int changes = 0;
	BDFClusterGalIter nextClust(*this);
	BDFCluster *c1, *c2 = 0;
	do {
		while ((c1 = nextClust++) != 0) {
			c2 = c1->mergeCandidate();
			if (c2) break;
		}
		if (c2) {
			c1 = merge(c1, c2);
			// keep expanding the result as much as possible.
			BDFCluster* c3;
			while ((c3 = c1->mergeCandidate()) != 0)
				c1 = merge(c1, c3);
			changes = TRUE;
			nextClust.reset();
		}
		// if no more merge candidates, try the slow merge.
		// note we must always reset the iterator after a
		// merge since it might be invalid (point to a deleted
		// cluster).
		else if ((c2 = fullSearchMerge()) != 0)
			nextClust.reset();
	} while (c2);

	if (logstrm) {
		if (changes)
			*logstrm << "After merge pass:\n" << *this;
		else *logstrm << "Merge pass made no changes\n";
	}
	return changes;
}

// This function does a full search for possible clusters to merge.  If
// it finds a candidate pair, it merges the pair and returns true.  If
// not, if returns false.
BDFCluster* BDFClusterGal::fullSearchMerge() {
	if (logstrm)
		*logstrm << "Starting fullSearchMerge\n";
	BDFClusterGalIter nextClust(*this);
	BDFCluster* c;
	while ((c = nextClust++) != 0) {
		BDFClustPortIter nextPort(*c);
		BDFClustPort *p;
		while ((p = nextPort++) != 0) {
			// check requirement 1: same rate and no delay
			if (!p->fbDelay() && p->sameRate()) {
				BDFCluster *peer = p->far()->parentClust();
				BDFCluster *src, *dest;
				// sort them so src is the source.
				if (p->isItOutput()) {
					src = c;
					dest = peer;
				}
				else {
					src = peer;
					dest = c;
				}
				// requirement 2: no indirect path.
				// requirement 3: no buried control arcs.
				// if met, do the merge and return the
				// result.
				// buriedCtlArcs fills remaps in.
				remaps.initialize();
				if (!indirectPath(src,dest) &&
				    !buriedCtlArcs(src,dest))
					return merge(src,dest);
			}
		}
	}
	return 0;
}

int BDFClusterGal::parallelLoopMergePass() {
	// See if we can do a merge of two disconnected
	// clusters that have the same loop factor.
	BDFClusterGalIter nextClust(*this);
	int changes = FALSE;
	BDFCluster *c;
// step 1: apply recursively to each cluster
	while ((c = nextClust++) != 0)
		changes |= c->internalClustering();
	nextClust.reset();
// step 2: apply at top level.

	// falseHitList is a list of clusters not to try again.
	SequentialList falseHitList;

	while ((c = nextClust++) != 0) {
		if (c->loop() <= 1) continue;
		if (falseHitList.member(c)) continue;
		// copy the iterator using the copy constructor.
		BDFClusterGalIter i2(nextClust);
		BDFCluster* c2;
		int falseMatch = FALSE;
		while ((c2 = i2++) != 0) {
			if (c2->loop() != c->loop()) continue;
			if (logstrm)
				*logstrm <<
				 "Trying parallel loop merge of " <<
				 c->readName() << " and " << c2->readName();
			// merge if no indirect paths in either direction.
			if (indirectPath(c,c2) || indirectPath(c2,c)) {
				if (logstrm) *logstrm <<
					"... Can't merge, indirect paths\n";
				falseMatch = TRUE;
			}
			else {
				if (logstrm) *logstrm << "... looks good!\n";
				// Do the merge!
				merge(c,c2);
				changes = TRUE;
				// reset iterator so we will rescan
				// from the beginning.
				nextClust.reset();
				falseMatch = FALSE;
				break;
				// break out of inner while, continue
				// to do the outer while from the beginning.
			}
		}
		// if we get here and falseMatch is true, then c cannot
		// merge with anything even though its rate matches another
		// loop.  We add it to a stop list so we don't keep trying
		// to merge it each time through.
		if (falseMatch) falseHitList.put(c);
	}
	return changes;
}

// this function returns true if there exists an indirect path from the
// src cluster to the dst cluster (one that passes through another
// cluster), treating the ports as directed paths.

int BDFClusterGal::indirectPath(BDFCluster* src, BDFCluster* dst) {
	stopList.initialize();
	stopList.put(src);
	BDFClustPortIter nextP(*src);
	BDFClustPort* p;
	while ((p = nextP++) != 0) {
		BDFCluster* peer = p->far()->parentClust();
		if (p->isItOutput() && peer != dst && findPath(peer,dst))
			return TRUE;
	}
	return FALSE;
}

// This recursive function is used by indirectPath.  The member object
// stopList prevents the same paths from being checked repeatedly.
// TRUE is returned if we can reach dst from start without going through
// any cluster on the stop list.  When indirectPath calls this it puts
// src on the stop list to initialize it.

int BDFClusterGal::findPath(BDFCluster* start, BDFCluster* dst) {
	stopList.put(start);
	BDFClustPortIter nextP(*start);
	BDFClustPort* p;
	while ((p = nextP++) != 0) {
		if (p->isItInput()) continue;
		BDFCluster* peer = p->far()->parentClust();
		if (peer == dst) return TRUE;
		if (stopList.member(peer)) continue;
		if (findPath(peer,dst)) return TRUE;
	}
	return FALSE;
}

static ostream& operator<<(ostream& o, BDFClustPort& p);

// This routine marks all feed-forward delays on a clusterGal to
// enhance the looping capability without hazard of dead-lock condition.
// We suggest that this method be called after the initial clustering is
// completed for efficiency (since searching for indirect paths is expensive)

int BDFClusterGal :: markFeedForwardDelayArcs() {
	if (logstrm)
		*logstrm << "starting markFeedForwardDelayArcs()\n";
	if (numberClusts() <= 1) return FALSE;
	int changes = FALSE;
	BDFClusterGalIter nextClust(*this);
	BDFCluster* c;

	while ((c = nextClust++) != 0) {
		BDFClustPortIter nextP(*c);
		BDFClustPort* p;
		while ((p = nextP++) != 0) {
			if (p->isItInput()) continue;
			if (p->numTokens() == 0) continue;
			// this port has a delay on the arc.
			BDFCluster* peer = p->far()->parentClust();
			stopList.initialize();
			if (!findPath(peer,c))  {
				p->markFeedForward();
				if (logstrm) {
					*logstrm << "Marking " << *p;
					*logstrm << "as a feedforward arc\n";
				}
				changes = TRUE;
			}
		}
	}
	return changes;
}

// This is the loop pass.  It loops any cluster for which loopFactor
// returns a value greater than 1.

// This version only does non-integral rate conversions (e.g. 2->3 or
// 3->2) when there are only two clusters.

int BDFClusterGal::loopPass() {
	if (logstrm)
		*logstrm << "Starting loop pass\n";
	if (numberClusts() <= 1) return FALSE;
	int changes = 0;
	BDFClusterGalIter nextClust(*this);

	// special handling for two clusters: do arbitrary rate
	// changes if no delays between the clusters and rate
	// mismatch exists.
	// We can ignore feedforward delays.
	if (numberClusts() == 2) {
		BDFCluster* c1 = nextClust++;
		BDFCluster* c2 = nextClust++;
		// check to see that there are no delays on arcs,
		// and that a sample rate change is needed.
		BDFClustPortIter nextPort(*c1);
		BDFClustPort* p;
		while ((p = nextPort++) != 0) {
			if (p->fbDelay() ||
			    p->numIO() == p->far()->numIO()) return FALSE;
		}
		// ok, loop both clusters so that their
		// repetitions values become 1.
		int r1 = c1->reps();
		int r2 = c2->reps();
		c1->loopBy(r1);
		c2->loopBy(r2);
		if (logstrm)
			*logstrm << "looping " << c1->readName() << " by "
				<< r1 << " and " << c2->readName() << " by "
					<< r2 << "\n";
		return TRUE;
	}

	// "normal" case: only loop to do integral rate conversions.
	BDFCluster *c;
	while ((c = nextClust++) != 0) {
		int fac = c->loopFactor();
		if (fac > 1) {
			c->loopBy(fac);
			if (logstrm)
				*logstrm << "looping " << c->readName()
					<< " by " << fac <<"\n";
			changes = TRUE;
		}
	}
	if (logstrm && !changes)
		*logstrm << "Loop pass made no changes\n";
	return changes;
}

// get the control port.  We don\'t care here about "same data" relations.
static PortHole* controlPort(DFPortHole* p,BDFRelation& relation) {
	PortHole* a = p->assocPort();
	int r = p->assocRelation();
	if (!TorF(r)) {
		relation = BDF_NONE;
		a = 0;
	}
	else relation = (BDFRelation)r;
	return a;
}

// determine whether two signals are "the same"
static int sameSignal(PortHole* a1,PortHole* a2) {
	// this needs to be smarter.
	return (a1 == a2);
}


// determine whether two ports have matching conditions
static int condMatch(DFPortHole* p1,DFPortHole* p2) {
	BDFRelation r1, r2;
	PortHole* a1 = controlPort(p1,r1);
	PortHole* a2 = controlPort(p2,r2);
	if (r1 != r2) return FALSE;
	if (a1 == 0) return TRUE;
// both have the same condition; we need to know if the signal is the
// same.
	return sameSignal(a1,a2);
}

// This function returns TRUE if a cluster matches the rate of all its
// neighbors.
int BDFCluster::matchNeighborRates() {
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	while ((p = nextPort++) != 0) {
		if (!p->sameRate()) return FALSE;
	}
	return TRUE;
}

// This function determines a constant loop factor for a cluster.
// We attempt to find a factor to repeat the loop by that will make
// it better match the sample rate of its neighbors.
int BDFCluster::loopFactor() {
	int retval = 0;
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	while ((p = nextPort++) != 0) {
		BDFClustPort* pFar = p->far();
		int myIO = p->numIO();
		int peerIO = pFar->numIO();
		// can't loop if connected to a different rate star by a delay
		// (unless it is a feedforward).
		if (p->fbDelay() && (myIO != peerIO || !condMatch(p,pFar)))
			return 0;
		// don't loop if peer should loop first
		if (myIO > peerIO) return 0;
		// try looping if things go evenly.
		if (peerIO % myIO == 0 && condMatch(p,pFar)) {
			int possFactor = peerIO / myIO;
			// choose the smallest possible valid loopfactor;
			// if retval is zero we have not found one yet.
			if (retval == 0 || retval > possFactor)
				retval = possFactor;
		}
	}
	return retval;
}

static const char* labels[] = { "<ifFalse:", "<ifTrue:", "<Same:", "<Compl:" };

// fn to print a single port of a cluster
static ostream& operator<<(ostream& o, BDFClustPort& p) {
	o << p.parent()->readName() << "." << p.readName();

	// print BDF control information
	if (p.isControl()) o << "[C]";
	BDFRelation r = p.relType();
	BDFClustPort* a = p.assoc();
	if (a && r != BDF_NONE) o << labels[r] << a->readName() << ">";

	// print connectivity
	BDFClustPort* pFar = p.far();
	if (p.isItOutput())
		o << "=>";
	else
		o << "<=";
	if (!pFar) o << "0";
	else o << pFar->parent()->readName() << "." << pFar->readName();
	if (p.numTokens() > 0) o << "[" << p.numTokens() << "]";
	return o;
}

// fn to print ports of a cluster.
ostream& BDFCluster::printPorts(ostream& o) {
	const char* prefix = "( ";
	BDFClustPortIter next(*this);
	BDFClustPort* p;
	while ((p = next++) != 0) {
		o << prefix << *p;
		prefix = "\n\t  ";
	}
	return o << " )";
}

// loop a cluster.  Adjust repetitions and token counts.
void BDFCluster::loopBy(int fac) {
	pLoop *= fac;
	repetitions.numerator /= fac;
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	while ((p = nextPort++) != 0)
		p->numberTokens *= fac;
}

// unloop a cluster.  Return old loop factor.  This undoes the effect
// of previous loopBy calls.
int BDFCluster::unloop() {
	int fac = pLoop;
	pLoop = 1;
	repetitions.numerator *= fac;
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	while ((p = nextPort++) != 0)
		p->numberTokens /= fac;
	return fac;
}

// generate a name for a new bag
const char* BDFClusterGal::genBagName() {
	char buf[20];
	sprintf (buf, "bag%d", bagNumber++);
	return hashstring(buf);
}

// This function combines two clusters into one.  There are four cases:
// basically the distinction between atomic and non-atomic clusters.

BDFCluster* BDFClusterGal::merge(BDFCluster* c1, BDFCluster* c2) {
	int fac = 1;

// if a bag has a loop factor we treat it as an atom for merging
// purposes (put it inside another bag instead of combining bags).
// That's why we test for the loop count.
// Exception: if both loop factors are the same, we "unloop" and
// "reloop".

	// unloop check
	if (c1->loop() == c2->loop() && c1->loop() > 1) {
		fac = c1->unloop();
		c2->unloop();
	}

	if (logstrm)
		*logstrm << "merging " << c1->readName() << " and " <<
			c2->readName() << "\n";

	BDFClusterBag* c1Bag = (c1->loop() == 1) ? c1->asBag() : 0;
	BDFClusterBag* c2Bag = (c2->loop() == 1) ? c2->asBag() : 0;
	if (c1Bag) {
		if (c2Bag) c1Bag->merge(c2Bag,this); // merge two bags
		else c1Bag->absorb(c2,this);   // put c2 into the c1 bag
	}
	else if (c2Bag) {		// c2 is a bag, c1 is not
		c2Bag->absorb(c1,this); // put c1 into the c2 bag
		c1Bag = c2Bag;	     // leave result in the same place
	}
	else {
		// make a new bag and put both atoms into it.
		LOG_NEW; BDFClusterBag* bag = new BDFClusterBag;
		addBlock(bag->setBlock(genBagName(),this));
		bag->absorb(c1,this);
		bag->absorb(c2,this);
		c1Bag = bag;
	}
	if (logstrm)
		*logstrm << "result is " << *c1Bag << "\n\n";

	if (fac > 1) {
		// reloop after merge
		c1Bag->loopBy(fac);
	}
	return c1Bag;
}

// constructor: make empty bag.
BDFClusterBag :: BDFClusterBag()
: owner(TRUE), exCount(0), sched(0), gal(0)
{}


void BDFClusterBag :: createScheduler() {
	LOG_DEL; delete sched;
	LOG_NEW; sched = new BDFBagScheduler;
}

void BDFClusterBag :: createInnerGal() {
	LOG_DEL; delete gal;
	LOG_NEW; gal = new BDFClusterGal;
	if (parent())
		gal->dupStream((BDFClusterGal*)parent());
}

// This function absorbs an atomic cluster into a bag.

void BDFClusterBag::absorb(BDFCluster* c,BDFClusterGal* par) {
	if (size() == 0) {
		createInnerGal();
		repetitions = c->repetitions;
	}
// move c from its current galaxy to my galaxy.
	par->removeBlock(*c);
	gal->addBlock(*c,c->readName());
// adjust the bag porthole list.  Some of c's ports will now become
// external ports of the cluster, while some external ports of the
// cluster that connnect to c will disappear.
	BDFClustPortIter next(*c);
	BDFClustPort* cp;
	while ((cp = next++) != 0) {
		BDFClustPort* pFar = cp->far();
		if (pFar->parent() == this) {
			// the far side of this guy is one of my bag pointers.
			// zap it and connect directly.
			BDFClustPort* p = pFar->inPtr();
			int del = cp->numTokens();
			LOG_DEL; delete pFar;
			cp->connect(*p, del);
			cp->initGeo();
		}
		else {
			// add a new connection to the outside for this guy
			LOG_NEW; BDFClustPort *np =
				new BDFClustPort(*cp,this,1);
			cp->makeExternLink(np);
			addPort(*np);
		}
	}
	adjustAssociations();
}

// this function merges two bags.
void
BDFClusterBag::merge(BDFClusterBag* b,BDFClusterGal* par) {
	if (b->size() == 0) return;
	if (!gal) createInnerGal();
	// get a list of all "bagports" that connect the two clusters.
	// we accumulate them on one pass and delete on the next to avoid
	// problems with iterators.
	BDFClustPortIter nextbp(*this);
	BDFClustPort* p;
	SequentialList zap;
	while ((p = nextbp++) != 0)
		if (p->far()->parent() == b) zap.put(p);
	// zap is the list of connections between the two clusters.  These
	// become internal connections so we zap them from both bags' lists
	// of external pointers.
	ListIter nextZap(zap);
	while ((p = (BDFClustPort*)nextZap++) != 0) {
		BDFClustPort* near = p->inPtr();
		BDFClustPort* far = p->far()->inPtr();
		int del = p->numTokens();
		LOG_DEL; delete p->far();
		LOG_DEL; delete p;
		near->connect(*far, del);
		near->initGeo();
	}
	// now we simply combine the remaining bagports and clusters into this.
	BDFClusterBagIter nextC(*b);
	BDFCluster* c;
	while ((c = nextC++) != 0) {
		gal->addBlock(*c,c->readName());
	}
	BDFClustPortIter nextP(*b);
	while ((p = nextP++) != 0) {
		p->setNameParent(p->readName(),this);
		addPort(*p);
	}
// get rid of b.
	par->removeBlock(*b); // remove from parent galaxy
	b->owner = FALSE;     // prevent from zapping contents, I took them
	LOG_DEL; delete b;    // zap the shell
	adjustAssociations();
}

// this function finds an external port in the cluster that, if the inPtr
// chains are followed, leads to the same AtomCluster port as does the
// argument.  If no such port is found, zero is returned.

static BDFClustPort* findPort(BDFCluster* cl, BDFClustPort* p) {
	p = p->innermost();
	BDFClustPortIter nextPort(*cl);
	BDFClustPort* q;
	while ((q = nextPort++) != 0) {
		if (p == q->innermost()) return q;
	}
	return 0;
}

// this function recalculates BDF_{SAME,COMPLEMENT,TRUE,FALSE} relations
// after merging of clusters.  For each bag port, the underlying real
// port may be marked the SAME or COMPLEMENT of other real ports, but
// these relationships are only visible at the bag port level if the
// relationship holds with another bag port.

// For BDF_TRUE and BDF_FALSE, the clustering algorithm makes sure that
// the control port remains an external port, or can be "remapped" to
// one that is.  If not -- which indicates a bug -- an error is reported.

static BDFClustPort* remapAfterMerge(BDFClustPort*,BDFRelation&);

static const char msg[] = "Control port is buried, algorithm error?";

void BDFClusterBag::adjustAssociations() {
	BDFClustPortIter nextPort(*this);
	BDFClustPort* bagp;
	while ((bagp = nextPort++) != 0) {
		BDFRelation r = bagp->inPtr()->relType();
		if (r == BDF_NONE) continue;
		BDFClustPort* lower = bagp->inPtr()->assoc();
		BDFClustPort* upper = lower->outPtr();
		if (TorF(r)) {
			if (upper == 0) {
				// lower should be remappable.
				BDFRelation r_remap;
				lower = remaps.lookup(lower,r_remap);
				if (lower) upper = findPort(this,lower);
				// "upper" should always be non-null now
				if (!upper) {
					Error::abortRun(*bagp,msg);
					return;
				}
				if (r_remap == BDF_COMPLEMENT) {
					// reverse the sense of the relation
					r = (r == BDF_TRUE) ?
						BDF_FALSE : BDF_TRUE;
				}
			}
			bagp->setRelation(r, upper);
			upper->setControl(TRUE);
			continue;
		}
		// handle BDF_SAME and BDF_COMPLEMENT now.
		// reversed expresses the relationship between
		// bagp->inPtr() at a given time.

		int reversed = (r == BDF_COMPLEMENT);

		while (!upper && SorC(r = lower->relType())) {

			// first one is not a bag port, but there is more
			// to the chain.  If connected to another star
			// with SAME and COMPLEMENT links we also include
			// those.
			if (lower->numTokens() == 0 &&
			    SorC(lower->far()->relType())) {
				lower = lower->far();
				r = lower->relType();
				// the fall-through here is intentional: if i
				// am internal, so is my peer.
			}
			lower = lower->assoc();
			upper = lower->outPtr();
			if (r == BDF_COMPLEMENT) reversed = !reversed;
		}
		// we have either found a bag port that we are related to
		// (same or complement) or we have gone around to ourselves.
		if (upper == 0 || upper == bagp)
			bagp->setRelation(BDF_NONE);
		else bagp->setRelation(reversed ? BDF_COMPLEMENT : BDF_SAME,
				       upper);
	}
}

// internal clustering for a bag
int BDFClusterBag::internalClustering() {
	return size() >= 2 ? gal->parallelLoopMergePass() : FALSE;
}

// print a bag on a stream.
ostream& BDFClusterBag::printOn(ostream& o) {
	printBrief(o);
	o << "= {";
	BDFClusterBagIter next(*this);
	BDFCluster* c;
	while (( c = next++) != 0) {
		o << " ";
		c->printBrief(o);
	}
	if (numberPorts() == 0) return o << " }\n";
	o << " }\nports:\t";
	return printPorts(o);
}

// generate the bag's schedule.  Also make schedules for "the bags within".
// Does nothing for an empty bag.
int BDFClusterBag::genSched() {
	if (size() == 0) return TRUE;

	// create scheduler
	createScheduler();

	BDFClusterBagIter nextClust(*this);
	BDFCluster* c;
	while ((c = nextClust++) != 0)
		if (!c->genSched()) return FALSE;
	return (sched->setup(*gal));
}

// indent by depth tabs.
static const char* tab(int depth) {
	// this fails for depth > 20, so:
	if (depth > 20) depth = 20;
	static const char *tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	return (tabs + 20 - depth);
}

// return the bag's schedule.
StringList BDFClusterBag::displaySchedule(int depth) {
	if (sched == 0) {
		return "schedule has not been computed";
	}
	StringList sch;
	if (loop() > 1) {
		sch += tab(depth);
		sch += loop();
		sch += "*{\n";
		depth++;
	}
	sch += sched->displaySchedule(depth);
	if (loop() > 1) {
		depth--;
		sch += tab(depth);
		sch += "}\n";
	}
	return sch;
}

// run the cluster, taking into account the loop factor
int BDFClusterBag::fire() {
	if (!sched) return FALSE;
	sched->setStopTime(loop()+exCount);
	sched->run(*gal);
	exCount += loop();
	return !Scheduler::haltRequested();
}

// destroy the bag.
BDFClusterBag::~BDFClusterBag() {
	if (!owner) return;
	LOG_DEL; delete gal;
	LOG_DEL; delete sched;
}

// This function searches for a port related to ctlP with SAME or
// COMPLEMENT arcs that is connected to a port that is not a child either
// of "me" or of "you".  That is, if "me" and "you" are merged, this port
// will be an external port rather than an internal connection in the cluster.
// if one is found, "rel" gives the relationship: same or complement,
// and the return value is the port.  Otherwise 0 is returned.

static BDFClustPort *seekExternal(BDFClustPort* ctlP,BDFCluster *me,
				  BDFCluster* you, BDFRelation& rel) {
	BDFRelation r = ctlP->relType();
	if (!SorC(r)) return 0;
	int reversed = (r == BDF_COMPLEMENT);
	BDFClustPort *a = ctlP;
	BDFCluster *cl;
	while (a && ((cl = a->far()->parentClust()) == me || cl == you)) {
		r = a->relType();
		a = a->assoc();
		if (a == ctlP) return 0;
		if (r == BDF_COMPLEMENT) reversed = !reversed;
		else if (r != BDF_SAME) a = 0;
	}
	if (a) {
		rel = r;
		return a;
	}
	else return 0;
}

// This function, given a BDFClustPort, tries to find another port that
// is the SAME or COMPLEMENT of it that will remain an external
// port if the two clusters it connects are merged.
// it returns null if there is no such port; otherwise it returns
// the port as well as the relationship \(through rel\) which is either
// BDF_SAME or BDF_COMPLEMENT.

static BDFClustPort* remapAfterMerge(BDFClustPort* ctlP,BDFRelation& rel)
{
	BDFClustPort *pFar = ctlP->far();
	BDFCluster *me = ctlP->parentClust();
	BDFCluster *you = pFar->parentClust();

// step 1: we can remap if ctlP is connected by SAME or COMPLEMENT
// arcs to a port that is connected to some other cluster than me or you.

	BDFClustPort *a = seekExternal(ctlP,me,you,rel);
	if (a) return a;

// we can also remap if there is no delay between ctlP and pFar, and
// pFar has an external match.
	if (ctlP->numTokens() == 0 &&
	    (a = seekExternal(pFar,me,you,rel)))
		return a;
	else {
		return 0;
	}
}

// This function is called to check whether a merge operation that
// makes the argument, which is a controlling port, an internal port
// will result in a cluster that is not a legal BDF actor.  It returns
// 0 if the control arc will be lost and the star I/O will depend
// on a hidden signal.

// If only internal arcs are controlled by the control signal, the
// control signal itself is returned \(no remapping is required\).
// If the control signal is available externally \(because of SAME
// or COMPLEMENT relations\) the alias is returned.

static BDFClustPort* remapControl (BDFClustPort *ctlP, BDFRelation& r) {
	BDFClustPort *pFar = ctlP->far();
	BDFClustPort *a;
	BDFCluster *me = ctlP->parentClust();
	BDFCluster *you = pFar->parentClust();
	BDFCluster *cl;

	// we can return FALSE if none of the ports of "me" that are
	// controlled by ctlP will end up external.

	BDFClustPortIter nextPort(*me);
	BDFClustPort *p;
	int sorry = FALSE;
	while ((p = nextPort++) != 0) {
		if ((a = p->assoc()) != ctlP || !TorF(p->relType()))
			continue;
		// OK, ctlP controls p.  But this is OK if p connects
		// to a port belonging to "me" or "you".
		cl = p->far()->parentClust();
		if (cl != me && cl != you) {
			sorry = TRUE;
			break;
		}
	}
	// if sorry is false, all controlled ports are internal so we
	// can merge without remapping.  We return the original argument.
	if (!sorry) {
		r = BDF_SAME;	// I am the same as myself!
		return ctlP;
	}

	// OK, see if the control signal is exported -- for example,
	// it may come from a fork.  If so, we return the remapped
	// value, if not, return 0.

	return remapAfterMerge(ctlP,r);
}

// check for any buried control arcs resulting from merging
// the two stars.

int BDFClusterGal::buriedCtlArcs(BDFCluster* src, BDFCluster* dest)
{
	BDFClustPortIter nextp(*src);
	BDFClustPort *p;
	while ((p = nextp++) != 0) {
		BDFClustPort *pFar = p->far();
		BDFClustPort *pRemap;
		BDFClustPort *pFail = 0;
		BDFRelation r;
		if (pFar->parentClust() != dest) continue;
		if (p->isControl()) {
			pRemap = remapControl(p,r);
			if (pRemap)
				remaps.add(p,pRemap,r);
			else pFail = p;
		}
		if (pFar->isControl() && !pFail) {
			pRemap = remapControl(pFar,r);
			if (pRemap)
				remaps.add(pFar,pRemap,r);
			else pFail = p;
		}
		if (pFail) {
			if (logstrm) {
				*logstrm << "Can't merge " << src->readName()
					<< " and " << dest->readName() <<
					": would bury a control arc:\n"
					<< *pFail << "\n";
			}
			return TRUE;
		}
	}
	return FALSE;
}

// find an attractive and compatible neighbor.
// This is the "quick version" -- it assumes that if the source has
// multiple outputs or the destination has multiple inputs, an alternate
// path might occur.  fullSearchMerge is the "slow version".
BDFCluster* BDFCluster::mergeCandidate() {
	BDFCluster *src = 0, *dst = 0;
	int multiSrc = 0, multiDst = 0;
	BDFCluster* srcOK = 0, *dstOK = 0;
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	while ((p = nextPort++) != 0) {
		BDFClustPort* pFar = p->far();
		BDFCluster* peer = pFar->parentClust();
		int myIO = p->numIO();
		int peerIO = pFar->numIO();
		// a connection is ok to use for merging if there are
		// no delays and no sample rate changes, and if neither
		// end of the connection is conditional

		int ok = (!p->fbDelay() && myIO==peerIO && condMatch(p,pFar));

		// we cannot merge a control arc unless it can be "remapped"
		// or nothing it controls is external.  Since this is the
		// fast merge pass, we do not try to merge control arcs here.
		// fullSearchMerge will attempt it, though.

		if (p->isControl() || pFar->isControl()) {
			ok = FALSE;

			// there may be multiple ports connecting two
			// clusters -- if ANY disappearing control port
			// is a problem we must veto the merge.

			if (peer == srcOK) srcOK = 0;
			if (peer == dstOK) dstOK = 0;
		}
		if (p->isItInput()) {
			if (src == 0) src = peer;
			if (src != peer) multiSrc++;
			if (ok) srcOK = peer;
		}
		else {
			if (dst == 0) dst = peer;
			if (dst != peer) multiDst++;
			if (ok) dstOK = peer;
		}
	}
	// we can use srcOK if it is the only source.
	if (srcOK && multiSrc == 0) return srcOK;
	// we can use dstOK if it is the only destination.
	else if (dstOK && multiDst == 0) return dstOK;
	else return 0;
}


// function to create names for atomic clusters and portholes.
static const char* mungeName(NamedObj& o) {
	StringList name = o.readFullName();
	const char* cname = strchr (name, '.');
	if (cname == 0) return "top";
	cname++;
// change dots to _ .
	char buf[80];
	char* p = buf;
	int i = 0;
	while (i < 79 && *cname) {
		if (*cname == '.') *p++ = '_';
		else *p++ = *cname;
		cname++;
	}
	*p = 0;
	return hashstring(buf);
}

// create an atomic cluster to surround a single DataFlowStar.
BDFAtomCluster::BDFAtomCluster(DataFlowStar& star,Galaxy* parent) : pStar(star)
{
	DFStarPortIter nextPort(star);
	DFPortHole* p;
	while ((p = nextPort++) != 0) {
		// do not make a port in the cluster if it is a "loopback" port
		if (p->far()->parent() == &star) continue;
		LOG_NEW; BDFClustPort *cp = new BDFClustPort(*p,this);
		addPort(*cp);
		cp->numIO();	// test
	}
	setNameParent(mungeName(star),parent);
	repetitions = pStar.repetitions;
}

// destroy an atomic cluster.  Main job: delete the BDFClustPorts.
BDFAtomCluster::~BDFAtomCluster() {
	BlockPortIter nextPort(*this);
	PortHole* p;
	while ((p = nextPort++) != 0) {
		// avoid removal from parent list.
		p->setNameParent("",0);
		LOG_DEL; delete p;
	}
}

// print functions.
ostream& BDFCluster::printBrief(ostream& o) {
	if (pLoop > 1) {
		o << pLoop << "*";
	}
	return o << readName();
}

ostream& BDFAtomCluster::printOn(ostream& o) {
	printBrief(o);
	o << "\nports:\t";
	return printPorts(o);
}

StringList BDFAtomCluster::displaySchedule(int depth) {
	StringList sch = tab(depth);
	if (loop() > 1) {
		sch += loop();
		sch += "*";
	}
	sch += real().readFullName();
	sch += "\n";
	return sch;
}

int BDFAtomCluster::fire() {
	for (int i = 0; i < loop(); i++) {
		if (!pStar.fire()) return FALSE;
	}
	return TRUE;
}

int BDFAtomCluster::myExecTime() {
	return pLoop * pStar.myExecTime();
}

// constructor for BDFClustPort, port for use in cluster.
// if bp is set it's a "bag port" belonging to an BDFClusterBag.
BDFClustPort::BDFClustPort(DFPortHole& port,BDFCluster* parent, int bp)
: pPort(port), bagPortFlag(bp), pOutPtr(0), feedForwardFlag(0),
  ctlFlag(0)
{
	const char* name = bp ? port.readName() : mungeName(port);
	setPort(name,parent,INT);
	myPlasma = Plasma::getPlasma(INT);
	numberTokens = port.numXfer();
}

void BDFClustPort::initGeo() { myGeodesic->initialize();}

// return true if port has same rate as its neighbor.
int BDFClustPort::sameRate() {
	return (numIO() == far()->numIO() && condMatch(this,far()));
}

// This method is called on a cluster porthole to create a connection
// with a "bag port" in the parent ClusterBag.

void BDFClustPort::makeExternLink(BDFClustPort* bagPort) {
	pOutPtr = bagPort;
	bagPort->numberTokens = numberTokens;
	// if I am connected, disconnect me and connect my peer
	// to my external link (the bagPort)
	BDFClustPort* pFar = far();
	if (pFar) {
		int del = numTokens();
		disconnect();
		bagPort->connect(*pFar,del);
		bagPort->initGeo();
	}
}

// return the atomic cluster port I am connected to.
BDFClustPort* BDFClustPort :: innermost() {
	BDFClustPort* p = this;
	while (p->isBagPort()) p = p->inPtr();
	return p;
}

// return the real far port aliased to bag ports.
BDFClustPort* BDFClustPort :: realFarPort(BDFCluster* outsideCluster) {
	if (parentClust() == outsideCluster) return 0;
	if (far()) return far();
	return pOutPtr->realFarPort(outsideCluster);
}

// set relationship
void BDFClustPort :: setRelation(int r, BDFClustPort* assoc) {
	// do not reference myGeodesic before it is set.
	int nT = myGeodesic ? numTokens() : 0;
	setBDFParams(numIO(),assoc,(BDFRelation)r,nT);
}

// methods for BDFClustSched, the clustering scheduler.

BDFClustSched::~BDFClustSched() { LOG_DEL; delete cgal;}

// compute the schedule!
int BDFClustSched::computeSchedule (Galaxy& gal) {
	LOG_DEL; delete cgal;

	// log file stuff.
	const char* file = logFile;
	ostream* logstrm = 0;
	if (file && *file) {
		if (strcmp(file,"cerr") == 0 ||
		    strcmp(file,"stderr") == 0)
			logstrm = &cerr;
		else {
			int fd = creat(expandPathName(file), 0666);
			if (fd < 0)
				Error::warn(gal, "Can't open log file ",file);
			else logstrm = new ofstream(fd);
		}
	}

	// do the clustering.
	cgal = new BDFClusterGal(gal,logstrm);
	cgal->cluster();

	if (logstrm) {
		*logstrm << "Rest not done yet\n";
		logstrm->flush();
		LOG_DEL; delete logstrm;
	}
	Error::abortRun("Rest of algorithm not yet implemented");
	invalid = TRUE;
	return FALSE;
#if 0
// generate subschedules
	if (!cgal->genSubScheds()) {
		invalid = TRUE; return FALSE;
	}

// generate top-level schedule
	if (BDFScheduler::computeSchedule(*cgal)) {
		if (logstrm) {
			*logstrm << "Schedule:\n" << displaySchedule();
			LOG_DEL; delete logstrm;
		}
		return TRUE;
	}
	else return FALSE;
#endif
}

// display the top-level schedule.
StringList BDFClustSched::displaySchedule() {
	StringList sch;
//	BDFSchedIter next(mySchedule);
//	BDFCluster* c;
//	while ((c = (BDFCluster*)next++) != 0) {
//		sch += c->displaySchedule(0);
//	}
	sch << "BDFClustSched::displaySchedule not yet implemented\n";
	return sch;
}

StringList BDFBagScheduler::displaySchedule(int depth) {
	StringList sch;
//	BDFSchedIter next(mySchedule);
//	BDFCluster* c;
//	while ((c = (BDFCluster*)next++) != 0) {
//		sch += c->displaySchedule(depth);
//	}
	sch << "BDFBagScheduler::displaySchedule not yet implemented\n";
	return sch;
}

// The following functions are used in code generation.  Work is
// distributed among the two scheduler and two cluster classes.
void BDFClustSched::compileRun() {
//	StringList code;
//	Target& target = getTarget();
//	BDFSchedIter next(mySchedule);
//	BDFCluster* c;
//	while ((c = (BDFCluster*)next++) != 0) {
//		c->genCode(target,1);
//	}
}

void BDFAtomCluster::genCode(Target& t, int depth) {
	if (loop() > 1) {
		t.beginIteration(loop(), depth);
		t.writeFiring(real(), depth+1);
		t.endIteration(loop(), depth);
	}
	else t.writeFiring(real(), depth);
}

void BDFClusterBag::genCode(Target& t, int depth) {
	if (loop() > 1) {
		t.beginIteration(loop(), depth);
		depth++;
	}
	sched->genCode(t, depth);
	if (loop() > 1) {
		depth--;
		t.endIteration(loop(), depth);
	}
}

void BDFBagScheduler::genCode(Target& t, int depth) {
//	BDFSchedIter next(mySchedule);
//	BDFCluster* c;
//	while ((c = (BDFCluster*)next++) != 0) {
//		c->genCode(t, depth);
//	}
}

// Remap methods

// Add a remapping to the list.  We use the "translated" versions of the
// original and remapped value.  We avoid putting bag ports into the
// list because they are dynamically created or deleted; the innermost
// ports stay fixed throughout the clustering.

void BDFRemapList :: add(BDFClustPort* o,BDFClustPort* r,BDFRelation rr) {
	if (o != r) {
		o = o->innermost();
		r = r->innermost();
		LOG_NEW; put(*new BDFremap(o,r,rr));
	}
}

BDFClustPort* BDFRemapList :: lookup(BDFClustPort* o, BDFRelation& rel) {
	ListIter next(*this);
	o = o->innermost();
	BDFremap *r;
	while ((r = (BDFremap*)next++) != 0) {
		if (r->orig == o) {
			rel = r->rel;
			return r->remap;
		}
	}
	return 0;
}

void BDFRemapList :: initialize() {
	ListIter next(*this);
	BDFremap *r;
	while ((r = (BDFremap*)next++) != 0) {
		LOG_DEL; delete r;
	}
	SequentialList::initialize();
}

