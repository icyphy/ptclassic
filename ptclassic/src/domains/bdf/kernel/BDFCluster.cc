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
#include "BDFClustPort.h"
#include "GalIter.h"
#include "Geodesic.h"
#include "Target.h"
#include "pt_fstream.h"
#include "Error.h"
#include <assert.h>

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
	for (int i = 0; i < nports; i++)
		ptable[i] = 0;
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
	// now connect up the cluster ports to match the real ports.
	// There may be fewer cluster ports than real ports if there are
	// self-loops, for such cases, ptable[i] will be null.
	for (i = 0; i < nports; i++) {
		BDFClustPort* curCP = ptable[i];
		if (!curCP) continue;
		// create corresponding assocPort relations, if they exist.
		DFPortHole& realP = curCP->real();

		PortHole* a = realP.assocPort();

		if (a) {
			// find the ClustPort that corresponds to a, and
			// set me to be associated with that.
			// because of the special properties of BDF_SAME
			// and BDF_COMPLEMENT relations, we do not need
			// to do redundant settings -- the relations are
			// transitive -- hence the test.
			BDFClustPort* assoc = ptable[a->index()];
			BDFRelation r = (BDFRelation)realP.assocRelation();
			if (!SorC(r) || r != curCP->relType())
				curCP->setRelation((BDFRelation)r, assoc);

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

static int pseudNum = 0;

// fn to make names for pseudoports
static const char* pseudoName() {
	StringList name = "_dup_";
	pseudNum++;
	name += pseudNum;
	return hashstring(name);
}

// Top level of clustering algorithm.  Here's the outline.
// 1. Do a clusterCore pass.
// 2. If not reduced to uniform rate, mark feedforward delays as ignorable
//    and do another clusterCore pass.
// 3. Do a pass that merges parallel loops with the same rate.

int BDFClusterGal::cluster() {
	// this next one is not re-entrant, sorry
	pseudNum = 0;
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
			if (p->far() == 0) continue;
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
				 c->name() << " and " << c2->name();
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
		if (p->far() == 0) continue;
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
		if (p->isItInput() || p->far() == 0) continue;
		BDFCluster* peer = p->far()->parentClust();
		if (peer == dst) return TRUE;
		if (stopList.member(peer)) continue;
		if (findPath(peer,dst)) return TRUE;
	}
	return FALSE;
}

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
			if (p->isItInput() || p->far() == 0) continue;
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
	if (numberClusts() == 2 && nonIntegralRateChange())
		return TRUE;

	// "normal" case: only loop to do integral rate conversions,
	// or insertion of an "if" clause.
	BDFCluster *c;
	while ((c = nextClust++) != 0) {
		BDFRelation rel;
		int fac;
		BDFClustPort* cond = c->ifFactor(rel);
		if (cond) {
			c->ifIze(cond,rel,logstrm);
		}
		else if ((fac = c->loopFactor()) > 1) {
				c->loopBy(fac);
			if (logstrm)
				*logstrm << "looping " << c->name()
					<< " by " << fac <<"\n";
			changes = TRUE;
		}
	}
	if (logstrm) {
		if (changes)
			*logstrm << "After loop pass:\n" << *this;
		else *logstrm << "Loop pass made no changes\n";
	}
	return changes;
}

// see if we should create an "if".  We want to have no fixed rate changes
// on any arcs -- we are looking for an arc where my end is unconditional
// and the far end is conditional.  We will never do if-ing if the reverse
// is true, which means the other end should be surrounded by an if.

BDFClustPort* BDFCluster::ifFactor(BDFRelation& rel) {
	BDFClustPortIter nextPort(*this);
	BDFClustPort *p;
	BDFClustPort *cond = 0;
	while ((p = nextPort++) != 0) {
		BDFClustPort* pFar = p->far();
		if (pFar == 0 || p->selfLoop()) continue;
		if (p->numTokens() > 0 || p->numIO() != pFar->numIO() ||
		    condMatch(p,pFar))
			return 0;
		// we are looking for: my end unconditional, far end is
		// conditional.  If we see the reverse, the other end
		// should loop instead, so we return false.
		if (!cond && !TorF(p->relType()) && TorF(pFar->relType())) {
			// conditional: if a previous one was also,
			// this must be the same condition.
			cond = pFar->assoc();
			rel = pFar->relType();
		}
		else if (TorF(p->relType()) && !TorF(pFar->relType())) {
			return 0;
		}
	}
	return cond;
}

void BDFCluster::ifIze(BDFClustPort* cond, BDFRelation rel,ostream* logstrm) {
	if (pCond) {
		Error::abortRun(*this," already conditional!");
		return;
	}

// see if this condition is available already in this cluster.
	BDFClustPortIter nextPort(*this);
	BDFClustPort *p;
	BDFRelation r;
	while ((p = nextPort++) != 0 && (r = sameSignal(cond,p)) == BDF_NONE)
		;
	if (!p) {
		// condition is not available here -- create a new connection
		if (logstrm)
			*logstrm << "Creating a boolean arc to pass "
				 << cond->name() << " to " << name()
				 << "\n";
		cond = connectBoolean(cond,rel);
		r = BDF_SAME;
	}
	else if (logstrm)
		*logstrm << "Making " << name() << " conditional on "
			 << p->name() << "\n";

	pCond = cond;
	if (r == BDF_COMPLEMENT) rel = reverse(rel);
	pType = (rel == BDF_TRUE ? DO_IFTRUE : DO_IFFALSE);
	// now adjust conditions on my ports:
	// unconditionals get a condition
	// matching condition unchanged
	// complementary conditions get zeroed
	nextPort.reset();
	while ((p = nextPort++) != 0) {
		if (p == cond) continue;

		// FIXME: should deal correctly with already-conditional
		// ports.

		p->setRelation(rel,cond);
	}
	cond->setControl(TRUE);
	if (logstrm)
		*logstrm << "After ifIzing: " << *this << "\n";
}

// return true if any condition associated with the port is satisfied.
// This setup is ok for unconditional, DO_IFTRUE and DO_IFFALSE.
int BDFCluster::condSatisfied() {
	if (pType != DO_IFTRUE && pType != DO_IFFALSE) return TRUE;
	// get most recent token from geodesic.
	Geodesic* g = pCond->innermost()->real().geo();
	Particle* p = g->get();
	if (!p) {
		Error::abortRun(*pCond,"Attempt to read from empty geodesic");
		return FALSE;
	}
	int v = int(*p);
	g->pushBack(p);
	return (pType == DO_IFTRUE) == (v != 0);
}

// this creates a duplicate of a port.  For bag ports, we recursively
// go all the way down to the "real thing".
static BDFClustPort* createDupPort(BDFClustPort* cond,const char* name) {
	BDFClustPort* in = cond->inPtr();
	BDFClustPort* a;
	BDFCluster* cpar = cond->parentClust();
	if (!in) {
		LOG_NEW; a = new BDFClustPort(*cond,cpar,BCP_DUP);
		a->setPort(name,cpar,INT);
	}
	else {
		BDFClustPort* d = createDupPort(in,name);
		LOG_NEW; a = new BDFClustPort(*d,cpar,BCP_BAG);
	}
	a->setRelation(BDF_SAME,cond);
	cpar->addPort(*a);
	return a;
}

// this function forms a connection that provides "cond" to the current
// cluster.  If cond is an input, we try to find a corresponding output.
// if there is not we just use it as is.

BDFClustPort* BDFCluster::connectBoolean(BDFClustPort* cond,
					  BDFRelation& rel) {
	if (cond->isItInput()) {
		BDFClustPortRelIter iter(*cond);
		BDFClustPort* acond;
		BDFRelation r2;
		while ((acond = iter.next(r2)) != 0) {
			if (acond->isItOutput()) {
				cond = acond;
				if (r2 == BDF_COMPLEMENT)
					rel = reverse(rel);
			}
		}
	}
	// create a new port in the far cluster
	BDFClustPort *a = createDupPort(cond,pseudoName());
	// create it in the near cluster -- name matches original
	// condition and the inner port is cond->innermost(), so
	// we will not be effected when outer bag ports are zapped.
	LOG_NEW; BDFClustPort *b =
		new BDFClustPort(*(cond->innermost()),this,BCP_DUP_IN);
	b->setPort(cond->name(),this,INT);
	addPort(*b);
	b->setRelation(BDF_NONE);
	b->connect(*a,0);
	b->initGeo();
	return b;
}

// This version only does non-integral rate conversions (e.g. 2->3 or
// 3->2) when there are only two clusters.

int BDFClusterGal::nonIntegralRateChange() {
	BDFClusterGalIter nextClust(*this);

	// special handling for two clusters: do arbitrary rate
	// changes if no delays between the clusters and rate
	// mismatch exists.
	// We can ignore feedforward delays.
	BDFCluster* c1 = nextClust++;
	BDFCluster* c2 = nextClust++;
	// check to see that there are no delays on arcs,
	// and that a sample rate change is needed.
	BDFClustPortIter nextPort(*c1);
	BDFClustPort* p;
	int n1, n2;
	while ((p = nextPort++) != 0) {
		n1 = p->numIO();
		n2 = p->far()->numIO();
		if (p->fbDelay() || n1 == n2 || !condMatch(p,p->far()))
			return FALSE;
	}
	// ok, loop both clusters so that they agree.
	int g = gcd(n1,n2);
	c1->loopBy(n2/g);
	c2->loopBy(n1/g);
	if (logstrm)
		*logstrm << "looping " << c1->name() << " by "
			 << n2/g << " and " << c2->name() << " by "
			 << n1/g << "\n";
	return TRUE;
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
		if (pFar == 0 || p->selfLoop()) continue;
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
	repetitions *= Fraction(1,fac);
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
	repetitions *= Fraction(fac,1);
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
//	int fac = 1;

// if a bag has a loop factor we treat it as an atom for merging
// purposes (put it inside another bag instead of combining bags).
// That's why we test for the loop count.
// Exception: if both loop factors are the same, we "unloop" and
// "reloop".

	
// unloop check: put this back right, must handle conditions as well.
//	if (c1->loop() == c2->loop() && c1->loop() > 1) {
//		fac = c1->unloop();
//		c2->unloop();
//	}

	if (logstrm)
		*logstrm << "merging " << c1->name() << " and " <<
			c2->name() << "\n";

	BDFClusterBag* c1Bag = c1->looped() ? 0 : c1->asBag();
	BDFClusterBag* c2Bag = c2->looped() ? 0 : c2->asBag();

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

//	if (fac > 1) {
//		// reloop after merge
//		c1Bag->loopBy(fac);
//	}
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

// test for a pair of ports that are connected
inline int ctlDelay(BDFClustPort* p,BDFClustPort* pfar) {
	return (p->numTokens() > 0 &&
		(p->isControl() || pfar->isControl()));
}

// This function absorbs an atomic cluster into a bag.

void BDFClusterBag::absorb(BDFCluster* c,BDFClusterGal* par) {
	if (size() == 0) {
		createInnerGal();
		repetitions = c->repetitions;
	}
// move c from its current galaxy to my galaxy.
	par->removeBlock(*c);
	gal->addBlock(*c,c->name());
// adjust the bag porthole list.  Some of c's ports will now become
// external ports of the cluster, while some external ports of the
// cluster that connnect to c will disappear.
// exception: we do not zap external ports if they are control ports
// with delay; we make them self-loops.
	BDFClustPortIter next(*c);
	BDFClustPort* cp;
	while ((cp = next++) != 0) {
		BDFClustPort* pFar = cp->far();
		assert(cp);
		if (pFar->parent() == this && !ctlDelay(cp,pFar)) {
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
				new BDFClustPort(*cp,this,BCP_BAG);
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

	// zap is the list of connections between the two clusters.  These
	// become internal connections so we zap them from both bags' lists
	// of external pointers.  Exception: do not zap control ports with
	// delay

	while ((p = nextbp++) != 0) {
		BDFClustPort *pFar = p->far();
		assert(pFar);
		if (pFar->parent() == b && !ctlDelay(p,pFar)) zap.put(p);
	}

	// now zap those that become internal

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
		gal->addBlock(*c,c->name());
	}
	BDFClustPortIter nextP(*b);
	while ((p = nextP++) != 0) {
		p->setNameParent(p->name(),this);
		addPort(*p);
	}
// get rid of b.
	par->removeBlock(*b); // remove from parent galaxy
	b->owner = FALSE;     // prevent from zapping contents, I took them
	LOG_DEL; delete b;    // zap the shell
	adjustAssociations();
}

// this function recalculates BDF_{SAME,COMPLEMENT,TRUE,FALSE} relations
// after merging of clusters.  For each bag port, the underlying real
// port may be marked the SAME or COMPLEMENT of other real ports, but
// these relationships are only visible at the bag port level if the
// relationship holds with another bag port.

// For BDF_TRUE and BDF_FALSE, the clustering algorithm makes sure that
// the control port remains an external port, or can be "remapped" to
// one that is.  If not -- which indicates a bug -- an error is reported.

static const char msg[] = "Control port is buried, algorithm error?";

void BDFClusterBag::adjustAssociations() {
	BDFClustPortIter nextPort(*this);
	BDFClustPort* bagp;
	while ((bagp = nextPort++) != 0) {
		// control bits are turned back on in a subsequent pass
		bagp->setControl(FALSE);
		BDFRelation r = bagp->inPtr()->relType();
		if (r == BDF_NONE) continue;
		BDFClustPort* lower = bagp->inPtr()->assoc();
		BDFClustPort* upper = lower->outPtr();
		if (TorF(r)) {
			if (upper == 0) {
				BDFRelation r_remap = BDF_NONE;
				BDFClustPortRelIter iter(*lower);
				while (!upper && (lower = iter.next(r_remap)) != 0) {
					upper = lower->outPtr();
				}
				if (r_remap == BDF_COMPLEMENT) {
					// reverse the sense of the relation
					r = reverse(r);
				}
			}
			// "upper" should always be non-null now
			if (!upper) {
				Error::abortRun(*bagp,msg);
				bagp->setRelation(BDF_NONE);
			}
			else
				bagp->setRelation(r, upper);
			continue;
		}
		// handle BDF_SAME and BDF_COMPLEMENT now.
		// reversed expresses the relationship between
		// bagp->inPtr() at a given time.

		// if upper is 0, then we must skip past the internal port
		// to the next one that is related to bagp and is, at the
		// same time, external.  The iterator steps through all
		// ports with an identity relationship.

		if (upper == 0) {
			BDFClustPortRelIter iter(*bagp->inPtr());
			while (!upper && (lower = iter.next(r)) != 0) {
				upper = lower->outPtr();
			}
		}
		if (upper) {
			// relation may already be set because of
			// other relations that are transitive.
			if (bagp->relType() != r)
				bagp->setRelation(r, upper);
		}
		else bagp->setRelation(BDF_NONE);
	}
	// mark arcs that are now control arcs.
	nextPort.reset();
	while ((bagp = nextPort++) != 0) {
		if (TorF(bagp->relType()))
			bagp->assoc()->setControl(TRUE);
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

// FORCE is a special input that causes the cluster to simulate execution
// even if it has already been executed "enough times"
static const int FORCE = 2;

// simulate the execution of the bag cluster.  We do not generate its
// internal schedule until it becomes runnable, so that the "real stars"
// will be executed in the right order (since simRunStar is called to
// generate the schedule).

// ? what about conditionals ?

int BDFClusterBag::simRunStar(int deferFiring) {
	// handle FORCE input
	if (deferFiring == FORCE)
		noTimes = 0;
	int status = DataFlowStar::simRunStar(deferFiring);
	if (status == 0) {
		int nRun = loop();
		if (sched == 0) {
			genSched();
			nRun -= 1;
		}
		if (nRun == 0) return status;
		SDFSchedIter nextStar(*sched);
		for (int i = 0; i < nRun; i++) {
			nextStar.reset();
			DataFlowStar* s;
			while ((s = nextStar++) != 0)
				s->simRunStar(FORCE);
		}
	}
	return status;
}

int BDFClusterBag::genSched() {
	if (sched) return TRUE;
	LOG_NEW; sched = new BDFBagScheduler;
	sched->setGalaxy(*gal);
	sched->setup();
	return !Scheduler::haltRequested();
}

// indent by depth tabs.
static const char* tab(int depth) {
	// this fails for depth > 20, so:
	if (depth > 20) depth = 20;
	static const char *tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	return (tabs + 20 - depth);
}

static const char* do_conds[] = { 0, "if(", "if(!", "until(", "until(!" };

// return the bag's schedule.
StringList BDFClusterBag::displaySchedule(int depth) {
	if (sched == 0) genSched();
	StringList sch;
	if (loop() > 1) {
		sch += tab(depth);
		sch += loop();
		sch += "*{\n";
		depth++;
	}
	else if (pType != DO_ITER) {
		sch << do_conds[pType] << pCond->name() << ") {";
		depth++;
	}
	sch += sched->displaySchedule(depth);
	if (loop() > 1 || pType != DO_ITER) {
		depth--;
		sch += tab(depth);
		sch += "}\n";
	}
	return sch;
}

// run the cluster, taking into account the loop factor
int BDFClusterBag::run() {
	if (!sched) return FALSE;
	if (!condSatisfied()) return TRUE;
	sched->setStopTime(loop()+exCount);
	sched->run();
	exCount += loop();
	return !Scheduler::haltRequested();
}

// destroy the bag.
BDFClusterBag::~BDFClusterBag() {
	if (!owner) {
		// we do not own the contained blocks and ports
		// so we remove them before deleting the galaxy.
		GalTopBlockIter nextb(*gal);
		Block* b;
		while ((b = nextb++) != 0) {
			gal->removeBlock(*b);
			nextb.reset();
		}
		BlockPortIter nextp(*gal);
		PortHole* p;
		while ((p = nextp++) != 0) {
			gal->removePort(*p);
			nextp.reset();
		}
	}
	LOG_DEL; delete gal;
	LOG_DEL; delete sched;
}

// This function, given a BDFClustPort, tries to find another port that
// is the SAME or COMPLEMENT of it that will remain an external
// port if the two clusters it connects are merged.
// it returns null if there is no such port; otherwise it returns
// the port as well as the relationship (through rel) which is either
// BDF_SAME or BDF_COMPLEMENT.

// if the needInput flag is true, only input portholes are considered.

static BDFClustPort *remapAfterMerge(BDFClustPort* ctlP, BDFRelation& rel,
				  int needInput) {
	BDFCluster *me = ctlP->parentClust();
	BDFCluster *you = ctlP->far()->parentClust();
	BDFClustPortRelIter iter(*ctlP);
	BDFClustPort* a;
	BDFCluster *cl;
	while ((a = iter.next(rel)) != 0
	       && ((cl = a->far()->parentClust()) == me || cl == you)
	       && (needInput && a->isItOutput()))
		;
	return a;
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

// Furthermore, input arcs must be controlled by input arcs.  This
// is checked for as well.

static BDFClustPort* remapControl (BDFClustPort *ctlP, BDFRelation& r) {
	BDFClustPort *pFar = ctlP->far();
	assert(pFar);
	BDFClustPort *a;
	BDFCluster *me = ctlP->parentClust();
	BDFCluster *you = pFar->parentClust();
	BDFCluster *cl;

	// we can return ourself (no remapping needed) if no arc
	// controlled by ctlP will end up external.

	BDFClustPortIter nextPort(*me);
	BDFClustPort *p;
	int sorry = FALSE;
	int needInput = TRUE;
	while ((p = nextPort++) != 0) {
		if ((a = p->assoc()) != ctlP || !TorF(p->relType()))
			continue;
		// OK, ctlP controls p.  But this is OK if p connects
		// to a port belonging to "me" or "you".
		cl = p->far()->parentClust();
		if (cl != me && cl != you) {
			sorry = TRUE;
			if (p->isItInput()) needInput = TRUE;
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

	return remapAfterMerge(ctlP,r,needInput);
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
		if (!pFar || pFar->parentClust() != dest || p->numTokens() > 0)
			continue;
		if (p->isControl()) {
			pRemap = remapControl(p,r);
			if (!pRemap) pFail = p;
		}
		if (pFar->isControl() && !pFail) {
			pRemap = remapControl(pFar,r);
			if (!pRemap) pFail = p;
		}
		if (pFail) {
			if (logstrm) {
				*logstrm << "Can't merge " << src->name()
					<< " and " << dest->name() <<
					": would bury a control arc:\n"
					<< *pFail << "\n";
				if (p->numTokens() > 0)
					*logstrm << "But there is delay!  Eventually self-loop!\n";
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
		if (!pFar) continue;
		BDFCluster* peer = pFar->parentClust();
		// selfloop test
		if (peer == this) continue;

		int myIO = p->numIO();
		int peerIO = pFar->numIO();
		// a connection is ok to use for merging if there are
		// no delays and no sample rate changes, and if any
		// conditions on the two ends match

		int ok = (!p->fbDelay() && myIO==peerIO && condMatch(p,pFar));

		// we cannot merge a control arc unless it can be "remapped"
		// or nothing it controls is external or it has a delay.
		// Since this is the fast merge pass, we do not try to 
		// remap merge control arcs here.
		// fullSearchMerge will attempt it, though.

		if (p->numTokens() == 0 &&
		    (p->isControl() || pFar->isControl())) {
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
// This is a static class function.
const char* BDFCluster::mungeName(NamedObj& o) {
	StringList name = o.fullName();
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
	if (pType == DO_ITER) {
		if (pLoop > 1)
			o << pLoop << "*";
	}
	else {
		o << do_conds[pType] << pCond->name() << ")";
	}
	return o << name();
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
	else if (pType != DO_ITER) {
		sch << do_conds[pType] << pCond->name() << ") ";
	}
	sch += real().fullName();
	sch += "\n";
	return sch;
}

int BDFAtomCluster::run() {
	if (!condSatisfied()) return TRUE;
	for (int i = 0; i < loop(); i++) {
		if (!pStar.run()) return FALSE;
	}
	return TRUE;
}

int BDFAtomCluster::myExecTime() {
	return pLoop * pStar.myExecTime();
}

// methods for BDFClustSched, the clustering scheduler.

BDFClustSched::~BDFClustSched() { LOG_DEL; delete cgal;}

// for now, we assume we are SDF at top level.
void BDFClustSched::runOnce() { SDFScheduler::runOnce();}

// compute the schedule!
int BDFClustSched::computeSchedule (Galaxy& gal) {
	LOG_DEL; delete cgal;

	// log file stuff.
	const char* file = logFile;
	ostream* logstrm = 0;
	if (file && *file)
		logstrm = new pt_ofstream(file);

	// do the clustering.
	cgal = new BDFClusterGal(gal,logstrm);
	cgal->cluster();
	cgal->initialize();
	setGalaxy(*cgal);
// recompute top-level repetitions.
	SDFScheduler::repetitions();
// generate schedule -- this assumes top level is SDF.  FIXME
	if (SDFScheduler::computeSchedule(*cgal)) {
		if (logstrm) {
			*logstrm << "Schedule:\n" << displaySchedule();
			LOG_DEL; delete logstrm;
		}
		return TRUE;
	}
	else return FALSE;
}

// display the top-level schedule.
StringList BDFClustSched::displaySchedule() {
	StringList sch;
	SDFSchedIter next(*this);
	BDFCluster* c;
	while ((c = (BDFCluster*)next++) != 0) {
		sch += c->displaySchedule(0);
	}
	return sch;
}

StringList BDFBagScheduler::displaySchedule(int depth) {
	StringList sch;
	SDFSchedIter next(*this);
	BDFCluster* c;
	while ((c = (BDFCluster*)next++) != 0) {
		sch += c->displaySchedule(depth);
	}
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

// return true if star can be scheduled data-independently, because
// all condtional ports have conditions and rates that match neighbors.
int BDFCluster::dataIndependent() {
	BDFClustPortIter nextp(*this);
	BDFClustPort *p;
	while ((p = nextp++) != 0) {
		if (TorF(p->relType())) {
			BDFClustPort* pFar = p->far();
			if (pFar && (pFar->numIO() != p->numIO() ||
				     !condMatch(p,pFar)))
				return FALSE;
		}
	}
	return TRUE;
}
