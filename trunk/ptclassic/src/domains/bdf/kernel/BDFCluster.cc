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
#include "DynDFScheduler.h"
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
: bagNumber(1), logstrm(log), urateFlag(FALSE)
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
// generate sub-schedules and fix buffer sizes.
void BDFClusterGal::genSubScheds() {
	BDFClusterGalIter nextClust(*this);
	BDFCluster* c;
	while ((c = nextClust++) != 0)
		c->genSched();
	nextClust.reset();
	while ((c = nextClust++) != 0)
		c->fixBufferSizes(c->reps());
}

// Test to see if we have uniform rate.  Since a uniform-rate cluster
// is never turned into a nonuniform rate cluster, the information is
// cached.

int BDFClusterGal::uniformRate() {
	if (urateFlag) return TRUE;
	if (numberClusts() <= 1) return (urateFlag = TRUE);
	BDFClusterGalIter nextClust(*this);
	BDFCluster* c;
	while ((c = nextClust++) != 0) {
		if (!c->matchNeighborRates()) return FALSE;
	}
	urateFlag = TRUE;
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
	} while (c2 && !SimControl::haltRequested());

	// Try to turn clusters with self-loops into while-loops now.
	// this code is the way it is because iterators are not
	// safe on modified lists.  So we reset and search again
	// if we do a makeWhile.
	int gotThrough;
	do {
		BDFClustPort* ctl;
		BDFRelation rel;
		gotThrough = 1;
		nextClust.reset();
		while ((c1 = nextClust++) != 0) {
			if ((ctl = c1->canBeWhiled(rel)) != 0) {
				makeWhile(ctl,rel);
				gotThrough = 0;
				changes = TRUE;
				break;
			}
		}
	} while (!gotThrough);
				
	if (logstrm) {
		if (changes)
			*logstrm << "After merge pass:\n" << *this;
		else *logstrm << "Merge pass made no changes\n";
	}
	return changes;
}

void BDFClusterGal::makeWhile(BDFClustPort* ctl, BDFRelation rel) {
	BDFCluster *c_in, *c_new;
	c_in = ctl->parentClust();
	LOG_NEW; c_new = new BDFWhileLoop(rel,ctl,c_in);
	addBlock(c_new->setBlock(genBagName(),this));
	if (logstrm)
		*logstrm << "Created while loop around " << c_in->name()
			 << ": " << *c_new << "\n";
}

void showBDFg(BDFClusterGal* g) {
	cerr << g->numberClusts() << " clusters: " << *g << "\n";
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
			BDFCluster *peer = p->far()->parentClust();
			// check requirement 1: same rate and no delay
			// also, do not try to merge with myself.
			if (peer != c && !p->fbDelay() && p->sameRate()) {
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
				// if 3 is not met, it may be possible to
				// make a while-loop.
				// if we can, do the merge and return the
				// result.
				if (!indirectPath(src,dest)) {
					if (!buriedCtlArcs(src,dest) &&
					    !buriedCtlArcs(dest,src))
						return merge(src,dest);
					else {
						BDFCluster* tlm =
							tryLoopMerge(src,dest);
						if (tlm) return tlm;
					}
				}
			}
		}
	}
	return 0;
}

// this function returns true if the only neighbor of "me" is "only"
// and false otherwise.
static int onlyNeighbor(BDFCluster* me,BDFCluster* only) {
	BDFClustPortIter nextPort(*me);
	BDFClustPort* p;
	while ((p = nextPort++) != 0) {
		BDFClustPort* pFar = p->far();
		if (pFar == 0) continue;
		BDFCluster* peer = pFar->parentClust();
		if (peer != me && peer != only) return FALSE;
	}
	return TRUE;
}

BDFCluster* BDFClusterGal::tryLoopMerge(BDFCluster* a,BDFCluster* b) {
	// assumption: a and b meet every condition for merging
	// except for buried control arcs.
	// we can still merge them under certain conditions by doing
	// a do-while loop around the merge.

	// It will be preferable under some circumstances to avoid the
	// do-while and do if-then-else instead.  We do this when all
	// external ports are attached to "leaf clusters" without a
	// rate change.   That is what the "ifInstead" flag is for.

	// A necessary condition is that all external arcs be
	// conditional on the same (or equivalent) signal.
	// They must also be conditional "in the same way".
	// Even so, we may not want to do it; creating "if" arcs
	// may be better.

	// We should verify that while-loops produced by this method
	// are "proper" -- non-deadlocked and bounded.  This could be
	// done by a mini-state-traversal.

	BDFClustPortIter nexta(*a);
	BDFClustPortIter nextb(*b);
	BDFClustPort *p;
	BDFClustPort *condSrc = 0;
	BDFRelation desrel = BDF_NONE;
	int ifInstead = TRUE;

	while ((p = nexta++) != 0 || (p = nextb++) != 0) {
		BDFClustPort *pFar = p->far();
		if (!pFar || pFar->parentClust() == a ||
		    pFar->parentClust() == b) continue;
		// OK, p is external.  Must be conditional.
		if (!TorF(p->relType())) return 0;
		// all sources of conditionals must correspond
		// (the actual conditionals may be delays of one another)
		if (condSrc) {
			BDFClustPort* nSrc = p->assoc();
			if (nSrc->isItInput()) nSrc = nSrc->far();
			BDFRelation ssig = sameSignal(condSrc,nSrc);
			// either the control signals must be the same, or
			// they must be opposite and the relTypes are also
			// opposite.  Otherwise return.
			switch (ssig) {
			case BDF_SAME:
				if (p->relType() != desrel) return 0;
				break;
			case BDF_COMPLEMENT:
				if (p->relType() == desrel) return 0;
				break;
			default:
				return 0;
			}
		}
		else {
			condSrc = p->assoc();
			if (condSrc->isItInput()) condSrc = condSrc->far();
			desrel = p->relType();
		}
		// see if we can turn off "ifInstead"
		if (ifInstead &&
		    (p->numIO() != pFar->numIO() ||
		     !onlyNeighbor(pFar->parentClust(),p->parentClust())))
			ifInstead = FALSE;
	}
	if (!condSrc) return 0;
	if (logstrm)
		*logstrm << a->name() << " and " << b->name()
			 << ": doing loop merge, result:\n";
	if (ifInstead) {
		if (logstrm)
			*logstrm << "could loop-merge, but do if instead\n";
	}
	else {
		// make a the source of the condition.
		if (condSrc->parentClust() == b) {
			BDFCluster* t = a; a = b; b = t;
		}
		if (logstrm)
			*logstrm << a->name() << " and " << b->name()
				 << ": doing loop merge, result:\n";
		LOG_NEW; BDFCluster *c = new BDFWhileLoop(desrel,condSrc,a,b);
		addBlock(c->setBlock(genBagName(),this));
		if (logstrm) *logstrm << *c << "\n";
		return c;
	}
	return 0;
}

// return TRUE if this and b have the same loop condition.
// TEMPORARY: only return true for DO_ITER type loops -- needs
// work for other types.
int BDFCluster::sameLoopCondition(const BDFCluster& b) const
{
	if (pType == DO_ITER)
		return b.pType == DO_ITER && pLoop == b.pLoop;
#if 0
	BDFRelation desrel;
	// array of "reversals" for the loop types.
	static BDFLoopType revType[] = {
		DO_ITER, DO_IFFALSE, DO_IFTRUE, DO_UNTILFALSE, DO_UNTILTRUE
	};

	if (b.pType == DO_ITER)
		return FALSE;
	if (pType == b.pType) desrel = BDF_SAME;
	else if (revType[pType] == b.pType) desrel = BDF_COMPLEMENT;
	else return FALSE;
	return sameSignal(pCond,b.pCond) == desrel;
#else
	else return FALSE;
#endif
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

	int urate = uniformRate();
	while ((c = nextClust++) != 0) {
		if (!c->looped()) continue;
		if (falseHitList.member(c)) continue;
		// copy the iterator using the copy constructor.
		BDFClusterGalIter i2(nextClust);
		BDFCluster* c2;
		int falseMatch = FALSE;
		while ((c2 = i2++) != 0) {
			if (!c->sameLoopCondition(*c2)) continue;
			if (logstrm)
				*logstrm <<
				 "Trying parallel loop merge of " <<
				 c->name() << " and " << c2->name();
			// merge if no indirect paths in either direction.
			if (indirectPath(c,c2,urate) ||
			    indirectPath(c2,c,urate)) {
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

// The optional ignoreDelayArcs is passed onto findPath.
int BDFClusterGal::indirectPath(BDFCluster* src, BDFCluster* dst,
			int ignoreDelayArcs) {
	stopList.initialize();
	stopList.put(src);
	BDFClustPortIter nextP(*src);
	BDFClustPort* p;
	while ((p = nextP++) != 0) {
		if (p->far() == 0) continue;
		BDFCluster* peer = p->far()->parentClust();
		if (p->isItOutput() && peer != dst && peer != src &&
		    findPath(peer,dst,ignoreDelayArcs))
			return TRUE;
	}
	return FALSE;
}

// This recursive function is used by indirectPath.  The member object
// stopList prevents the same paths from being checked repeatedly.
// TRUE is returned if we can reach dst from start without going through
// any cluster on the stop list.  When indirectPath calls this it puts
// src on the stop list to initialize it.

// If ignoreDelayArcs is set, we ignore the presence
// of arcs with delays that equal or exceed the number of tokens
// transferred on the arc.  Such paths do not interfere with clustering
// if the cluster is uniform rate.

int BDFClusterGal::findPath(BDFCluster* start, BDFCluster* dst,
		    int ignoreDelayArcs) {
	stopList.put(start);
	BDFClustPortIter nextP(*start);
	BDFClustPort* p;
	while ((p = nextP++) != 0) {
		if (p->isItInput() || p->far() == 0) continue;
		if (ignoreDelayArcs && p->numTokens() >= p->numIO())
			continue;
		BDFCluster* peer = p->far()->parentClust();
		if (peer == start) continue; // ignore self-loops
		if (peer == dst) return TRUE;
		if (stopList.member(peer)) continue;
		if (findPath(peer,dst,ignoreDelayArcs)) return TRUE;
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
			BDFClustPort* pFar = p->far();
			if (p->isItInput() || pFar == 0) continue;
			if (p->numTokens() == 0) continue;
			// this port has a delay on the arc.
			// cannot mark if either end is a control port.

			// FIXME: may want to leave the mark, but make
			// sure the delay is not forgotten where it matters.

			if (p->isControl() || pFar->isControl()) continue;
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
			// add Boolean arc if needed.  cond may be remapped
			// and rel may be reversed.
			cond = connectBoolean(c,cond,rel);
		}
		if (cond) {
			c->ifIze(cond,rel,logstrm);
			changes = TRUE;
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
	// if the conditional is on a self-loop, we may want to avoid
	// creating an if construct and create a "while" instead.
	// we will avoid the if if the if-condition matches the state
	// of the initial token on the feedback arc.
	// FIXME: for now we assume initial tokens are always FALSE.
	if (cond->selfLoop() && rel == BDF_FALSE) {
//		cerr << "Avoiding making 'if' out of " << name() << "\n";
		return 0;
	}
	return cond;
}

void die(NamedObj& o, const char* m1, const char* m2="", const char* m3="") {
	cerr << "FATAL: " << o.fullName() << ": " << m1 << m2 << m3 << "\n";
	exit (1);
}

// this should move to BDFClustPort but I wanted to use die.

void BDFClustPort::setRelation(BDFRelation r, BDFClustPort* assoc) {
	if (assoc && parent() != assoc->parent()) {
		StringList a = assoc->fullName();
		die(*this, " illegal argument to setRelation: ", a);
	}
	BDFPortHole::setRelation(r,assoc);
}

void BDFCluster::ifIze(BDFClustPort* cond, BDFRelation rel,ostream* logstrm) {
	if (pCond) {
		die(*this," already conditional!");
		return;
	}

	if (logstrm)
		*logstrm << "Making " << name() << " conditional on "
			 << cond->name() << "\n";
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	pCond = cond;
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

// this creates a duplicate of a port.  For bag ports, we recursively
// go all the way down to the "real thing".
static BDFClustPort* createDupPort(BDFClustPort* cond,const char* name) {
	BDFClustPort* in = cond->inPtr();
	BDFClustPort* a;
	BDFCluster* cpar = cond->parentClust();
	cond->markDuped();
	if (!in) {
		LOG_NEW; a = new BDFClustPort(cond->real(),cpar,BCP_DUP);
		a->setPort(name,cpar,INT);
	}
	else {
		BDFClustPort* d = createDupPort(in,name);
		LOG_NEW; a = new BDFClustPort(*d,cpar,BCP_BAG);
		// a is the external link for d.
		d->makeExternLink(a);
	}
	a->setRelation(BDF_SAME,cond);
	cpar->addPort(*a);
	return a;
}

// return TRUE if the parent cluster of arc has an output that is
// connected to c.
static int hasParallel(BDFClustPort* arc,BDFCluster* c) {
	BDFCluster* pc = arc->parentClust();
	BDFClustPortIter nextp(*pc);
	BDFClustPort* p;
	while ((p = nextp++) != 0) {
		if (p->far() == 0 || p->isItInput()) continue;
		if (p->far()->parentClust() == c) return TRUE;
	}
	return FALSE;
}

// this function forms a connection that provides "cond" to the current
// cluster.  We prefer trying to remap the signal so that it is "parallel"
// to another input arc.

BDFClustPort* BDFClusterGal::connectBoolean(
	  BDFCluster* c, BDFClustPort* cond, BDFRelation& rel) {

// see if this condition is available already in this cluster.
	BDFClustPortIter nextPort(*c);
	BDFClustPort *p;
	BDFRelation r;
	while ((p = nextPort++) != 0 && (r = sameSignal(cond,p)) == BDF_NONE)
		;
	if (p) {
		if (r == BDF_COMPLEMENT) rel = reverse(rel);
		return p;
	}

	// condition is not available here -- create a new connection
	if (logstrm)
		*logstrm << "Creating a boolean arc to pass "
			 << cond->fullName() << " to " << c->name()
			 << "\n";
// condition is not present.  We must pass it.  If we can pair it with
// a parallel arc in the same direction this passing is always safe,
// otherwise we must make sure a delay-free-loop is not introduced.
	BDFClustPort * acc = hasParallel(cond,c) ? cond : 0;
	if (!acc) {
		BDFClustPortRelIter iter(*cond);
		BDFClustPort* acond;
		BDFRelation r2;
		while ((acond = iter.next(r2)) != 0) {
			if (hasParallel(acond,c)) {
				acc = acond;
				break;
			}
		}
		if (!acc) {
			iter.reset();
			while ((acond = iter.next(r2)) != 0) {
				stopList.initialize();
				if (!findPath(c,acond->parentClust(), 1)) {
					acc = acond;
					break;
				}
			}
		}
		if (!acc) {
			if (logstrm)
				*logstrm << "whoops: " << cond->fullName()
					 << " cannot be passed to "
					 << c->name()
					 << "w/o delay free loop\n";
			return 0;
		}
		if (r2 == BDF_COMPLEMENT) rel = reverse(rel);
	}
	cond = acc;
	// create a new port in the far cluster
	BDFClustPort *a = createDupPort(cond,pseudoName());
	// mark it as a control port
	a->setControl(TRUE);
	// create it in the near cluster -- name matches original
	// condition and the inner port is cond->innermost(), so
	// we will not be effected when outer bag ports are zapped.
	LOG_NEW; BDFClustPort *b =
		new BDFClustPort(cond->innermost()->real(),c,BCP_DUP_IN);
	b->setPort(cond->name(),c,INT);
	c->addPort(*b);
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
		if (p->far() && !p->sameRate()) return FALSE;
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
		// for selfloops, print only the input arc of the loop.
		if (p->isItInput() || !p->selfLoop()) {
			o << prefix << *p;
			prefix = "\n\t  ";
		}
	}
	return o << " )";
}

// loop a cluster.  Adjust repetitions and token counts.
void BDFCluster::loopBy(int fac) {
	pLoop *= fac;
	repetitions *= Fraction(1,fac);
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	while ((p = nextPort++) != 0) {
		p->setNumXfer(p->numXfer() * fac);
	}
}

// unloop a cluster.  Return old loop condition signal.  This undoes the effect
// of previous loopBy calls.
BDFClustPort* BDFCluster::unloop(int& fac,BDFLoopType& lcond) {
	fac = pLoop;
	pLoop = 1;
	repetitions *= Fraction(fac,1);
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	while ((p = nextPort++) != 0)
		p->setNumXfer(p->numXfer() / fac);
	lcond = pType;
	pType = DO_ITER;
	return pCond;
}

void BDFCluster::reloop(int fac, BDFLoopType lcond,BDFClustPort* pcond) {
	loopBy(fac);
	pType = lcond;
	pCond = pcond;
}

// generate a name for a new bag
const char* BDFClusterGal::genBagName() {
	BDFClusterGal* pgal = parentGal();
	if (pgal) return pgal->genBagName();
	char buf[20];
	sprintf (buf, "bag%d", bagNumber++);
	return hashstring(buf);
}

// This function combines two clusters into one.  There are four cases:
// basically the distinction between atomic and non-atomic clusters.

BDFCluster* BDFClusterGal::merge(BDFCluster* c1, BDFCluster* c2) {
	int fac = 1;
	BDFLoopType lcond = DO_ITER;
	BDFClustPort* pcond = 0;

// if a bag has a loop factor we treat it as an atom for merging
// purposes (put it inside another bag instead of combining bags).
// That's why we test for the loop count.
// Exception: if both loop factors are the same, we "unloop" and
// "reloop".

	if (c1->looped() && c1->sameLoopCondition(*c2)) {
		pcond = c1->unloop(fac,lcond);
		c2->unloop(fac,lcond);
	}

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

// restore any saved loop factors.
	c1Bag->reloop(fac,lcond,pcond);
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
	if (parent()) {
		BDFClusterGal* pgal = (BDFClusterGal*)parent();
		gal->dupStream(pgal);
		gal->setNameParent(name(),pgal);
	}
}

// test to see whether we should turn a connection into a self loop after
// a merge.  It is assumed that a and b are connected.  We leave the
// self-loop under two conditions:
// 1) if it is a control arc with delay, or
// 2) it is a pair of conditional ports with mismatched conditions.
// in the latter case, we should check somehow that the control arcs
// for the signals remain visible.

static int leaveSelfLoop(BDFClustPort* a,BDFClustPort* b) {
	return ((a->numTokens()>0 && (a->isControl() || b->isControl())) ||
		(TorF(a->relType()) && TorF(b->relType()) && !condMatch(a,b)));
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
// cluster that connnect to c will disappear.  We will leave connecting
// arcs as self-loops under some conditions (see above).
	BDFClustPortIter next(*c);
	BDFClustPort* cp;
	while ((cp = next++) != 0) {
		BDFClustPort* pFar = cp->far();
		if (pFar && pFar->parent() == this && !leaveSelfLoop(cp,pFar)) {
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
	// of external pointers.  Exceptions: as for absorb

	while ((p = nextbp++) != 0) {
		BDFClustPort *pFar = p->far();
		assert(pFar);
		if (pFar->parent() == b && !leaveSelfLoop(p,pFar))
			zap.put(p);
	}

	// now zap those that become internal

	ListIter nextZap(zap);
	while ((p = (BDFClustPort*)nextZap++) != 0) {
		BDFClustPort* pFar = p->far();
		BDFClustPort* near = p->inPtr();
		BDFClustPort* far = pFar->inPtr();
		int del = p->numTokens();
		LOG_DEL; delete pFar;
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
			// "upper" should always be non-null now.  Bomb the
			// program if something has screwed up.
			if (!upper) {
				bagp->setRelation(BDF_NONE);
				die(*bagp,msg);
				return;
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

// propagate buffer sizes downward to set sizes of cluster-boundary
// buffers.
void BDFClusterBag::fixBufferSizes(int nReps) {
	// account for looping of this cluster
	// we need not worry about pCond, it does not affect the size.
	nReps *= loop();
	// pass down sizes of external buffers.
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	while ((p = nextPort++) != 0) {
		BDFClustPort* in = p->inPtr();
		int n = p->maxArcCount();
		in->setMaxArcCount(n);
	}
	BDFClusterBagIter nextClust(*this);
	BDFCluster* c;
	while ((c = nextClust++) != 0)
		c->fixBufferSizes(nReps);
}

// generate internal schedules
int BDFClusterBag::genSched() {
	if (sched) return TRUE;
	LOG_NEW; sched = new BDFBagScheduler;
	sched->setGalaxy(*gal);
	sched->setup();
	if (Scheduler::haltRequested()) return FALSE;
	// generate sub-schedules.
	BDFClusterBagIter nextClust(*this);
	BDFCluster* c;
	while ((c = nextClust++) != 0) {
		if (!c->genSched()) return FALSE;
	}
	return TRUE;
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
	int close = 0;
	if (sched == 0) genSched();
	StringList sch;
	if (loop() > 1) {
		sch << tab(depth) << loop() << "*{\n";
		close = 1;
	}
	else if (pType != DO_ITER) {
		sch << tab(depth) << do_conds[pType]
		    << pCond->name() << ") {\n";
		close = 1;
	}
	depth += close;
	sch << sched->displaySchedule(depth);
	if (close) {
		depth--;
		sch << tab(depth) << "}\n";
	}
	return sch;
}

// Control for executing all clusters: we must grab any control tokens
// needed from the level below.
// At this point, outer run() has done receiveData() to get any input
// tokens, and of course the inside of the cluster has not yet been
// run.

// This version copies values for all tokens -- could be improved by
// only copying needed tokens.

void BDFCluster::go() {
	// First, cluster might be conditional with a false condition.
	// Exit immediately if so.
	if (pType == DO_IFTRUE || pType == DO_IFFALSE) {
		Particle& boolToken = (*pCond)%0;
		int v = (int(boolToken) != 0);
		if (v != (pType == DO_IFTRUE)) return;
	}
	runInner();
}

// run inside of the BDFClusterBag loop() times.

void BDFClusterBag::runInner() {
	sched->setStopTime(loop()+exCount);
	sched->run();
	exCount += loop();
}

// invoke wrapup functions: used only in dynamic execution
void BDFClusterBag::wrapup() {
	BDFClusterBagIter nextClust(*this);
	BDFCluster* c;
	while ((c = nextClust++) != 0) c->wrapup();
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

	while ((a = iter.next(rel)) != 0) {
		if (needInput && a->isItOutput()) continue;
		BDFClustPort* afar = a->far();
		if (afar == 0) continue;
		BDFCluster *cl = afar->parentClust();
		if (cl != me && cl != you) return a;
	}
	return 0;
}

// check for any buried control arcs resulting from merging
// the two stars.
// find conditional ports that will remain external, and check their
// control arcs.  We search only arcs on the src cluster, so buriedCtlArcs
// must be called TWICE: with args in both orders.

int BDFClusterGal::buriedCtlArcs(BDFCluster* src, BDFCluster* dest)
{
	BDFClustPortIter nextp(*src);
	BDFClustPort *p;
	while ((p = nextp++) != 0) {
		BDFClustPort *pFar = p->far();
		if (pFar == 0) continue;
		BDFCluster *peer = pFar->parentClust();
		if (peer == src || peer == dest || !TorF(p->relType()))
			continue;
		// OK, p remains an external port and is conditional.
		// See if its control port gets buried.
		BDFClustPort *ctl = p->assoc();
		pFar = ctl->far();
		if (!pFar || pFar->parentClust() != dest ||
		    ctl->numTokens() > 0)
			continue;
		// OK, control arc connects src and dest w/o delay.
		// Cannot merge unless we can remap it.
		// If p is an input, remapped port must also be input,
		// hence isItInput is used to get the argument.
		BDFRelation r;
		if (!remapAfterMerge(ctl,r,p->isItInput())) {
			if (logstrm) {
				*logstrm << "Can't merge " << src->name()
					<< " and " << dest->name() <<
					": would bury a control arc:\n"
					<< *ctl << "\n"
					<< "Arc controlled: " << *p << "\n";
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

// See if cluster can be converted into a while loop.  It MUST have a
// control port that is on a self-loop.  Furthermore, all non-self-loop
// ports must be conditional with the same sign on one or the other end
// of the self-loop.
// We could do more checking to make sure the loop is really valid and
// deadlock or unbounded behavior does not happen.
// Return value is either 0 or the source port for the control signal.

BDFClustPort* BDFCluster::canBeWhiled(BDFRelation& rel) {
	// if <= 2 ports, forget it.
	rel = BDF_NONE;
	if (numberPorts() <= 2) return 0;
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	BDFClustPort* csrc = 0;
	// pass 1: find a control loop.
	while ((p = nextPort++) != 0) {
		if (p->isControl() && p->selfLoop()) {
			// must have 1 token.
			if (p->numTokens() != 1) return 0;
			csrc = p->isItOutput() ? p : p->far();
		}
	}
	if (csrc == 0) return 0;

	// for now, all initial tokens are FALSE so we must have
	// this.  Eventually this is determined by the value of the
	// token on the control arc.
	rel = BDF_FALSE;

	// pass 2: check conditions on ports.  All external ports must
	// be conditional in the same way.
	nextPort.reset();
	while ((p = nextPort++) != 0) {
		if (p->selfLoop()) continue;
		BDFRelation r = p->relType();
		if (!TorF(r) ||
		    (p->assoc() != csrc && p->assoc() != csrc->far()))
			return 0;
		if (rel == BDF_NONE) rel = r;
		else if (rel != r) return 0;
	}
	return csrc;
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
		sch << loop() << "*";
	}
	else if (pType != DO_ITER) {
		sch << do_conds[pType] << pCond->name() << ") ";
	}
	sch << real().fullName() << "\n";
	return sch;
}

// run inside of the atomCluster loop() times.

void BDFAtomCluster::runInner() {
	for (int i = 0; i < loop(); i++) {
		if (!pStar.run()) return;
	}
}

// wrapup: pass through to inner star
void BDFAtomCluster::wrapup() {
	pStar.wrapup();
}

// set the buffer sizes of the actual buffers -- we can pass down all
// but the self-loops.
// A self-loop buffer size is just numInitDelays * numXfer.
// For a wormhole boundary, the size is nReps * numXfer (the
// total # moved throughout the execution of the schedule).
void BDFAtomCluster::fixBufferSizes(int nReps) {
	// # times real star is executed
	nReps *= loop();
	BDFClustPortIter nextPort(*this);
	BDFClustPort *cp;
	while ((cp = nextPort++) != 0) {
		DFPortHole& rp = cp->real();
		int sz = cp->maxArcCount();
		rp.setMaxArcCount(sz);
	}
	// set repetitions: only includes constant term
	pStar.repetitions = Fraction(nReps,1);
	// may need to set self-loops separately
	if (numberPorts() == pStar.numberPorts()) return;
	DFStarPortIter nextReal(pStar);
	DFPortHole *dp;
	while ((dp = nextReal++) != 0) {
		if (dp->atBoundary()) {
			int siz = dp->numXfer() * nReps;
			dp->setMaxArcCount(siz);
		}
		if (dp->far()->parent() == &pStar) {
			// self-loop! bufsize = #init + #write
			int siz = dp->numInitDelays() + dp->numXfer();
			dp->setMaxArcCount(siz);
		}
	}
}


int BDFAtomCluster::myExecTime() {
	return pLoop * pStar.myExecTime();
}

// methods for BDFClustSched, the clustering scheduler.

BDFClustSched::BDFClustSched(const char* log , int canDoDyn)
: cgal(0), logFile(log), dynamicAllowed(canDoDyn), dynSched(0) {}

BDFClustSched::~BDFClustSched() {
	LOG_DEL; delete cgal;
	LOG_DEL; delete dynSched;
}

// timing control
void BDFClustSched::setStopTime (double limit) {
	BDFScheduler::setStopTime(limit);
	if (dynSched)
		dynSched->setStopTime(limit);
}

void BDFClustSched::resetStopTime (double limit) {
	BDFScheduler::resetStopTime(limit);
	if (dynSched)
		dynSched->resetStopTime(limit);
}

// for now, we assume we are SDF at top level.
void BDFClustSched::runOnce() { SDFScheduler::runOnce();}

// verify that at top-level we are SDF.

static int SDFcheck(BDFClusterGal& gal) {
	if (gal.numberBlocks() == 1) return TRUE;
	BDFClusterGalIter nextClust(gal);
	BDFCluster* c;
	while ((c = nextClust++) != 0) {
		c->initialize();
		if (!c->isSDFinContext()) return FALSE;
	}
	return TRUE;
}

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
	if (logstrm) {
		*logstrm << "Clustering complete.  ";
		if (cgal->numberClusts() > 1)
			*logstrm << "Final clustering:\n" << *cgal;
		else *logstrm << "\n";
	}
	cgal->initialize();
	setGalaxy(*cgal);
// verify that top level is SDF.  If not, handle as dynamic case.
// NOTE: it is possible to schedule some BDF universes statically even
// if the top level is not SDF -- not yet implemented.
	int status;
	if (!SDFcheck(*cgal)) {
		status = handleDynamic(gal);
	}
	else {
		// top level is SDF.
		SDFScheduler::repetitions();
		status = SDFScheduler::computeSchedule(*cgal);
		if (status) {
			if (logstrm)
				*logstrm << "Generate subschedules, propagate buffer sizes\n";
			cgal->genSubScheds();
			if (logstrm)
				*logstrm << "Schedule:\n" << displaySchedule();
		}
	}
	if (logstrm) { LOG_DEL; delete logstrm; }
	return status;
}

// display the top-level schedule.
StringList BDFClustSched::displaySchedule() {
	StringList sch;
	BDFCluster* c;
	if (dynSched) {
		sch << "Dynamic execution of " << cgal->numberClusts()
		    << " clusters:\n";
		int i = 1;
		BDFClusterGalIter next(*cgal);
		while ((c = next++) != 0) {
			sch << "Cluster " << i++ << ":\n";
			sch << c->displaySchedule(1);
		}
	}
	else {
		SDFSchedIter next(*this);
		while ((c = (BDFCluster*)next++) != 0) {
			sch << c->displaySchedule(0);
		}
	}
	return sch;
}

int BDFClustSched::handleDynamic(Galaxy& gal) {
	if (!dynamicAllowed) {
		Error::abortRun(gal, "Top level of clustering is not SDF",
				"\nDynamic execution required");
		return FALSE;
	}
	// create dynamic scheduler
	LOG_DEL; delete dynSched;
	LOG_NEW; dynSched = new DynDFScheduler;
	dynSched->setGalaxy(*cgal);

	// set stop time for dynamic scheduler (copy my own)
	dynSched->setStopTime(getStopTime());

	// create schedules for each cluster
	BDFClusterGalIter nextClust(*cgal);
	BDFCluster* c;
	while ((c = nextClust++) != 0) c->genSched();

	// finish the setup.
	dynSched->setup();
	return !haltRequested();
}

int BDFClustSched::run() {
	if (dynSched) return dynSched->run();
	return BDFScheduler::run();
}

StringList BDFBagScheduler::displaySchedule(int depth) {
	StringList sch;
	SDFSchedIter next(*this);
	BDFCluster* c;
	while ((c = (BDFCluster*)next++) != 0) {
		sch << c->displaySchedule(depth);
	}
	return sch;
}

// The following functions are used in code generation.  Work is
// distributed among the two scheduler and two cluster classes.
void BDFClustSched::compileRun() {
	Target& targ = target();
	SDFSchedIter next(SDFScheduler::mySchedule);
	BDFCluster* c;
	while ((c = (BDFCluster*)next++) != 0) {
		c->genCode(targ,1);
	}
}

void BDFAtomCluster::genCode(Target& t, int depth) {
	if (pCond) {
		DFPortHole& condPort = pCond->innermost()->real();
		t.beginIf(condPort,pType==DO_IFTRUE,depth++,0);
	}
	if (loop() > 1) {
		t.genLoopInit(real(),loop());
		t.beginIteration(loop(), depth);
		t.writeFiring(real(), depth+1);
		t.endIteration(loop(), depth);
		t.genLoopEnd(real());
	}
	else t.writeFiring(real(), depth);
	if (pCond)
		t.endIf(--depth);
}

void BDFAtomCluster::genLoopInit(Target& t, int reps) {
	t.genLoopInit(real(), reps);
}

void BDFAtomCluster::genLoopEnd(Target& t) {
	t.genLoopEnd(real());
}

void BDFClusterBag::genCode(Target& t, int depth) {
	BDFClusterBagIter nextClust(*this);
	BDFCluster* c;
	if (pCond) {
		DFPortHole& condPort = pCond->innermost()->real();
		t.beginIf(condPort,pType==DO_IFTRUE,depth++,0);
	}
	if (loop() > 1) {
		while ((c = nextClust++) != 0)
			c->genLoopInit(t,loop());
		t.beginIteration(loop(), depth++);
	}
	sched->genCode(t, depth);
	if (loop() > 1) {
		t.endIteration(loop(), --depth);
		nextClust.reset();
		while ((c = nextClust++) != 0)
			c->genLoopEnd(t);
	}
	if (pCond)
		t.endIf(--depth);
}

void BDFBagScheduler::genCode(Target& t, int depth) {
	SDFSchedIter next(mySchedule);
	BDFCluster* c;
	while ((c = (BDFCluster*)next++) != 0) {
		c->genCode(t, depth);
	}
}

// return true if star can be scheduled data-independently, because
// all conditional ports have conditions and rates that match neighbors.
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

// actions for BDFWhileLoop.

ostream& BDFWhileLoop::printOn(ostream& o) {
	printBrief(o);
	o << "= { ";
	a->printBrief(o);
	if (b) {
		o << ", ";
		b->printBrief(o);
	}
	if (numberPorts() == 0) return o << " }\n";
	o << " }\nports:\t";
	return printPorts(o);
}

void BDFWhileLoop::runInner() {
	int tokval;
	// do-while stops when a token has the same value as endMark.
	int endMark = (pType == DO_UNTILTRUE);
	Geodesic* condGeo = pCond->geo();
	for (int i = 0; i < loop(); i++) {
		do {
			if (!a->run()) return;
			// get last token produced by control port.
			tokval = *(condGeo->tail());
			tokval = (tokval != 0);	// 0 or 1.
			if (b && !b->run()) return;
		} while (tokval != endMark);
	}
}

void BDFWhileLoop::fixBufferSizes(int nReps) {
	nReps *= loop();
	// pass down sizes of external buffers.
	BDFClustPortIter nextPort(*this);
	BDFClustPort* p;
	while ((p = nextPort++) != 0) {
		BDFClustPort* in = p->inPtr();
		int n = p->maxArcCount();
		in->setMaxArcCount(n);
	}
	// now fix inner clusters.
	a->fixBufferSizes(nReps);
	if (b) b->fixBufferSizes(nReps);
}

// Output schedule as a do-while.

StringList BDFWhileLoop::displaySchedule(int depth) {
	int close = 0;
	StringList sch;
	if (loop() > 1) {
		sch << tab(depth) << loop() << "*{\n";
		close = 1;
	}
	depth += close;
	sch << tab(depth) << "do {\n";
	depth += 1;
	sch << a->displaySchedule(depth);
	sch << tab(depth) << name() << "_token := <value from "
	    << pCond->name() << ">\n";
	if (b)
		sch << b->displaySchedule(depth);
	depth -= 1;
	sch << tab(depth) << "} while (";
	if (pType == DO_UNTILTRUE) sch << "!";
	sch << name() << "_token)\n";
	if (close) {
		depth--;
		sch << tab(depth) << "}\n";
	}
	return sch;
}

void BDFWhileLoop::genCode(Target& t, int depth) {
	t.beginDoWhile(depth);
	a->genCode(t,depth+1);
	if (b) b->genCode(t,depth+1);
	DFPortHole& condPort = pCond->innermost()->real();
	t.endDoWhile(condPort,pType == DO_UNTILFALSE,depth);
}

// Build a BDFWhileLoop.  It has one or two member clusters, which
// are pulled out of the parent galaxy.

BDFWhileLoop::BDFWhileLoop(BDFRelation t, BDFClustPort* cond,
			   BDFCluster* first,BDFCluster* second)
: a(first), b(second) {
	pType = (t == BDF_TRUE ? DO_UNTILTRUE : DO_UNTILFALSE);
	pCond = cond;
	a->parent()->asGalaxy().removeBlock(*a);
	if (b) {
		b->parent()->asGalaxy().removeBlock(*b);
		fixArcs(a,b);
		fixArcs(b,a);
	}
	else fixArcs(a,0);
}

// setup: initialize sub-clusters.
void BDFWhileLoop::setup() {
	a->initialize();
	if (b) b->initialize();
}

void BDFWhileLoop::fixArcs(BDFCluster* x,BDFCluster* y) {
	BDFClustPortIter next(*x);
	BDFClustPort* cp;
	while ((cp = next++) != 0) {
		BDFClustPort* pFar = cp->far();
		if (pFar == 0) continue;
		BDFCluster* peer = pFar->parentClust();
		if (peer != x && peer != y) {
			// create an external port
			LOG_NEW; BDFClustPort *np =
				new BDFClustPort(*cp,this,BCP_BAG);
			cp->makeExternLink(np);
			addPort(*np);
			// make the external port unconditional
			np->setRelation(BDF_NONE);
		}
	}
}
