/**************************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

 Programmer:  S. Ha

This file defines all the classes for a clustering CG galaxy
Similar to CGMacroCluster.h except that cluster classes are derived from
CG star, which is necessary for APEG generation and parallel scheduling.
It implies unfortunate code duplication! Maybe, we should have
multiple inheritance.

Given a galaxy, we create a parallel hierarchy called an CGMacroClusterGal.
In the parallel hierarchy, objects of class CGAtomCluster correspond
to the individual stars of the original galaxy; each CGAtomCluster contains
a reference to an CGStar.  The clustering algorithm transforms the
CGMacroClusterGal into a set of nested clusters.  class CGMacroClusterBag
represents a composite cluster with its own schedule; it resembles a
wormhole in some respects but is simpler (it does not use a pair of
event-horizon objects and does not have a Target; it does have a contained
galaxy and a type of SDFScheduler).

Cluster boundaries are represented by disconnected portholes;
SDFScheduler treats such portholes exactly as if they were wormhole
boundaries.

The virtual baseclass CGMacroCluster refers to either type of cluster.

The remaining class defined here is CGClustPort, a porthole for use
in CGMacroCluster objects.

**************************************************************************/
static const char file_id[] = "CGMacroCluster.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGMacroCluster.h"
#include "GalIter.h"
#include "Geodesic.h"
#include "Plasma.h"
#include "Target.h"
#include "pt_fstream.h"
#include "Error.h"

// A CGMacroClusterGal is a Galaxy, built from another galaxy.

// function to print an CGMacroClusterGal on an ostream.
ostream& operator<< (ostream& o, CGMacroClusterGal& g) {
	CGMacroClusterGalIter next(g);
	CGMacroCluster* c;
	while ((c = next++) != 0) {
		o << *c << "\n";
	}
	return o;
}

// constructor: builds a flat galaxy of CGAtomCluster objects.

CGMacroClusterGal::CGMacroClusterGal(Galaxy* gal, ostream* log)
: logstrm(log), myGal(gal), owner(0), bagNumber(1)
{
	int nports = setPortIndices(*gal);
	LOG_NEW; CGClustPort** ptable = new CGClustPort*[nports];
	for (int i = 0; i < nports; i++)
		ptable[i] = 0;
	DFGalStarIter nextStar(*gal);
	CGStar* s;
	while ((s = (CGStar*) nextStar++) != 0) {
		LOG_NEW; CGAtomCluster* c = new CGAtomCluster(*s,this);
		addBlock(*c);
		CGClustPortIter nextPort(*c);
		CGClustPort *p;
		while ((p = nextPort++) != 0) {
			ptable[p->real().index()] = p;
		}
	}
	// now connect up the cluster ports to match the real ports.
	// There may be fewer cluster ports than real ports if there are
	// self-loops, for such cases, ptable[i] will be null.
	for (i = 0; i < nports; i++) {
		CGClustPort* out = ptable[i];
		if (!out || out->isItInput()) continue;
		CGClustPort* in = ptable[out->real().far()->index()];
		int delay = out->real().numTokens();
		out->connect(*in,delay);
		out->initGeo();
	}
	LOG_DEL; delete ptable;

	// data structure initialization
        targetGalList.initialize();
        bagList.initialize();

	setNameParent(gal->name(), 0);
}

CGMacroClusterGal :: CGMacroClusterGal(CGMacroClusterBag* c, ostream* l): 
	logstrm(l), myGal(0), owner(c), bagNumber(-1) {
        targetGalList.initialize();
        bagList.initialize();
}

// Core clustering routine.  Alternate merge passes and loop passes
// until no more can be done.
int CGMacroClusterGal::clusterCore(int& urate) {
	int change = FALSE;
	while ((urate = uniformRate()) == FALSE) {
                int status = mergePass();
                status |= loopPass();
                if (status) change = TRUE;
                else break;
	}
	if (!urate) {
		// try doing the tree-structure nonintegral loop pass
		if (tryTreeLoop()) urate = change = TRUE;
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

int CGMacroClusterGal :: canMerge(CGMacroCluster* c1, CGMacroCluster* c2) {
	return c1->isParallelizable() == c2->isParallelizable();
}

// Top level of clustering algorithm.  Here's the outline.
// 1. Do a clusterCore pass.
// 2. If not reduced to uniform rate, mark feedforward delays as ignorable
//    and do another clusterCore pass.
// 3. Do a pass that merges parallel loops with the same rate.

int CGMacroClusterGal::cluster() {
	int urate;
	int change = clusterCore(urate);

	// if we did not get to uniform rate, try marking feedforward
	// delay arcs as ignorable and clustering some more.

	if (!urate && markFeedForwardDelayArcs()) {
		change |= clusterCore(urate);
	}
	if (!urate && logstrm)
		*logstrm <<
			"clustering algorithm did not achieve uniform rate";
	if (change) {
		if (logstrm)
			*logstrm << "Looking for parallel loops\n";
		parallelLoopMergePass();
	}
	return change;
}

// Test to see if we have uniform rate.
int CGMacroClusterGal::uniformRate() {
	if (numberClusts() <= 1) return TRUE;
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c = nextClust++;
	int rate = c->reps();
	while ((c = nextClust++) != 0)
		if (rate != c->reps()) return FALSE;
	return TRUE;
}

// Merge adjacent actors that can be treated as a single cluster.
int CGMacroClusterGal::mergePass() {
	if (numberClusts() <= 1) return FALSE;
	int changes = 0;
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster *c1, *c2 = 0;
	do {
		while ((c1 = nextClust++) != 0) {
			c2 = c1->mergeCandidate();
			if (c2 && canMerge(c1,c2)) break;
		}
		if (c2) {
			c1 = merge(c1, c2);
			// keep expanding the result as much as possible.
			CGMacroCluster* c3;
			while ((c3 = c1->mergeCandidate()) && canMerge(c1,c3))
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
CGMacroCluster* CGMacroClusterGal::fullSearchMerge() {
	if (logstrm)
		*logstrm << "Starting fullSearchMerge\n";
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c;
	while ((c = nextClust++) != 0) {
		CGClustPortIter nextPort(*c);
		CGClustPort *p;
		while ((p = nextPort++) != 0) {
			if (p->far() == 0) continue;
			// check requirement 1: same rate and no delay
			if (!p->fbDelay() &&
			    p->numIO() == p->far()->numIO()) {
				CGMacroCluster *peer = p->far()->parentClust();
				if (!canMerge(peer,c)) continue;
				CGMacroCluster *src, *dest;
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
				// if met, do the merge and return the
				// result.
				if (!indirectPath(src,dest))
					return merge(src,dest);
			}
		}
	}
	return 0;
}

int CGMacroClusterGal::parallelLoopMergePass() {
	// See if we can do a merge of two disconnected
	// clusters that have the same loop factor.
	CGMacroClusterGalIter nextClust(*this);
	int changes = FALSE;
	CGMacroCluster *c;
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
		CGMacroClusterGalIter i2(nextClust);
		CGMacroCluster* c2;
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

int CGMacroClusterGal::indirectPath(CGMacroCluster* src, CGMacroCluster* dst) {
	stopList.initialize();
	stopList.put(src);
	CGClustPortIter nextP(*src);
	CGClustPort* p;
	while ((p = nextP++) != 0) {
		if (p->far() == 0) continue;
		CGMacroCluster* peer = p->far()->parentClust();
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

int CGMacroClusterGal::findPath(CGMacroCluster* start, CGMacroCluster* dst) {
	stopList.put(start);
	CGClustPortIter nextP(*start);
	CGClustPort* p;
	while ((p = nextP++) != 0) {
		if (p->isItInput() || p->far() == 0) continue;
		CGMacroCluster* peer = p->far()->parentClust();
		if (peer == dst) return TRUE;
		if (stopList.member(peer)) continue;
		if (findPath(peer,dst)) return TRUE;
	}
	return FALSE;
}

static ostream& operator<<(ostream& o, CGClustPort& p);

// This routine marks all feed-forward delays on a clusterGal to
// enhance the looping capability without hazard of dead-lock condition.
// We suggest that this method be called after the initial clustering is 
// completed for efficiency (since searching for indirect paths is expensive)

int CGMacroClusterGal :: markFeedForwardDelayArcs() {
	if (logstrm)
		*logstrm << "starting markFeedForwardDelayArcs()\n";
	if (numberClusts() <= 1) return FALSE;
	int changes = FALSE;
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c;

	while ((c = nextClust++) != 0) {
		CGClustPortIter nextP(*c);
		CGClustPort* p;
		while ((p = nextP++) != 0) {
			if (p->isItInput() || p->far() == 0) continue;
			if (p->numTokens() == 0) continue;
			// this port has a delay on the arc.
			CGMacroCluster* peer = p->far()->parentClust();
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

int CGMacroClusterGal::loopPass() {
	if (logstrm)
		*logstrm << "Starting loop pass\n";
	if (numberClusts() <= 1) return FALSE;

	if (numberClusts() == 2) 
		return loopTwoClusts();
	int changes = integralLoopPass(0);
	if (!changes) changes = integralLoopPass(1);
	if (logstrm && !changes)
		*logstrm << "Loop pass made no changes\n";
	return changes;
}

int CGMacroClusterGal::loopTwoClusts() {
	// this assumes there are only two clusters.
	// We can ignore feedforward delays.
	// We can ignore feedback delays if large enough.
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c1 = nextClust++;
	CGMacroCluster* c2 = nextClust++;
	// check to see that there are no delays on arcs,
	// and that a sample rate change is needed.
	// we can ignore delays that are >= the total # of tokens
	// moved in the entire execution of the schedule.
	CGClustPortIter nextPort(*c1);
	CGClustPort* p;
	int r1 = c1->reps();
	while ((p = nextPort++) != 0) {
		if (p->far() == 0) continue;
		if (p->numIO() == p->far()->numIO()) return FALSE;
		if (p->fbDelay() && p->numTokens() < p->numIO()*r1)
			return FALSE;
	}
	// ok, loop both clusters so that their
	// repetitions values become 1.
	c1->loopBy(r1);
	int r2 = c2->reps();
	c2->loopBy(r2);
	if (logstrm)
		*logstrm << "looping " << c1->name() << " by "
			 << r1 << " and " << c2->name() << " by "
			 << r2 << "\n";
	return TRUE;
}

int CGMacroClusterGal::tryTreeLoop() {
	if (!isTree()) {
		if (logstrm)
			*logstrm <<
			"structure is not a tree, cannot do TreeLoop\n";
		return FALSE;
	}
	// we loop all the clusters so that the resulting repetitions is 1.
	// 
	if (logstrm)
		*logstrm << "Doing TreeLoop\n";
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c;
	while ((c = nextClust++) != 0) {
		int r = c->reps();
		if (logstrm)
			*logstrm << "looping " << c->name() << " by "
				 << r << "\n";
		c->loopBy(r);
	}
	if (logstrm)
		*logstrm << "TreeLoop successful\n";
	return TRUE;
}
		

int CGMacroClusterGal::integralLoopPass(int doAnyLoop) {
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster *c;
	int changes = 0;
	while ((c = nextClust++) != 0) {
		int fac = c->loopFactor(doAnyLoop);
		if (fac > 1) {
			c->loopBy(fac);
			if (logstrm)
				*logstrm << "looping " << c->name()
					<< " by " << fac <<"\n";
			changes = TRUE;
		}
	}
	return changes;
}

// This function determines the loop factor for a cluster.
// We attempt to find a factor to repeat the loop by that will make
// it better match the sample rate of its neighbors.
int CGMacroCluster::loopFactor(int doAnyLoop) {
	int retval = 0;
	CGClustPortIter nextPort(*this);
	CGClustPort* p;
	while ((p = nextPort++) != 0) {
		CGClustPort* pFar = p->far();
		int myIO = p->numIO();
		int peerIO = pFar->numIO();
		// can't loop if connected to a different rate star by a delay
		// (unless it is a feedforward or value is large enough).
		if (p->fbDelay() && myIO != peerIO
		    && p->numTokens() < myIO * reps()) return 0;
		// don't loop if peer should loop first
		if (myIO > peerIO) return 0;
		// try looping if things go evenly.
		if (doAnyLoop && peerIO == myIO) continue;
		if (peerIO % myIO == 0) {
			int possFactor = peerIO / myIO;
			// choose the smallest possible valid loopfactor;
			// if retval is zero we have not found one yet.
			if (retval == 0 || retval > possFactor)
				retval = possFactor;
		}
	}
	return retval;
}

// fn to print a single port of a cluster
static ostream& operator<<(ostream& o, CGClustPort& p) {
	o << p.parent()->name() << "." << p.name();
	CGClustPort* pFar = p.far();
	if (p.isItOutput())
		o << "=>";
	else
		o << "<=";
	if (pFar==0) {
		o << "0";
	} else {
		o << pFar->parent()->name() << "." << pFar->name();
		// we prob. wont have a geodisic unless we have a far side
		if (p.numTokens() > 0) o << "[" << p.numTokens() << "]";
	}
	return o;
}

// fn to print ports of a cluster.
ostream& CGMacroCluster::printPorts(ostream& o) {
	const char* prefix = "( ";
	CGClustPortIter next(*this);
	CGClustPort* p;
	while ((p = next++) != 0) {
		o << prefix << *p;
		prefix = "\n\t  ";
	}
	return o << " )";
}

// loop a cluster.  Adjust repetitions and token counts.
void CGMacroCluster::loopBy(int fac) {
	pLoop *= fac;
	repetitions *= Fraction(1,fac);
	CGClustPortIter nextPort(*this);
	CGClustPort* p;
	while ((p = nextPort++) != 0)
		p->loopBy(fac);
}

// unloop a cluster.  Return old loop factor.  This undoes the effect
// of previous loopBy calls.
int CGMacroCluster::unloop() {
	int fac = pLoop;
	pLoop = 1;
	repetitions *= Fraction(fac);
	CGClustPortIter nextPort(*this);
	CGClustPort* p;
	while ((p = nextPort++) != 0)
		p->unloopBy(fac);
	return fac;
}

// generate a name for a new bag
const char* CGMacroClusterGal::genBagName() {
	char buf[20];
	sprintf (buf, "bag%d", bagNumber++);
	return hashstring(buf);
}

// This function combines two clusters into one.  There are four cases:
// basically the distinction between atomic and non-atomic clusters.

CGMacroCluster* CGMacroClusterGal::merge(CGMacroCluster* c1, CGMacroCluster* c2) {
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
		*logstrm << "merging " << c1->name() << " and " <<
			c2->name() << "\n";

	CGMacroClusterBag* c1Bag = (c1->loop() == 1) ? c1->asBag() : 0;
	CGMacroClusterBag* c2Bag = (c2->loop() == 1) ? c2->asBag() : 0;
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
		CGMacroClusterBag* bag = createBag();
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

CGMacroClusterBag* CGMacroClusterGal :: createBag() {
	LOG_NEW; return new CGMacroClusterBag;
}

///////////////////////////////////////////////////////////////////
//   methods unique to CGMacroCluster
///////////////////////////////////////////////////////////////////

// cluster galaxies.
// cluster galaxy if "asFunc" parameter is set "YES"
int CGMacroClusterGal :: clusterGalaxy(Galaxy& outGal) {
	if (logstrm)
		*logstrm << "Enter clusterGalaxy()" << "\n";

	// identify galaxies for clustering
	selectTargetGals(outGal);

	// perform clustering
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c;

	// current active galaxy and active clusterbag
	// increase efficiency by caching effect.
	Galaxy* curGal = 0;
	CGMacroClusterBag* curBag = 0;

	while ((c = nextClust++) != 0) {
		c->setVisit(0);
	}

	// list of galaxy already clustered
	SequentialList galList;
	galList.initialize();

	// clustering begins.
	nextClust.reset();
	while ((c = nextClust++) != 0) {
		CGMacroClusterBag* bag = (CGMacroClusterBag*) c->asSpecialBag();
		Galaxy* g;
		if (bag) {
			g = (Galaxy*) bag->real()->parent();
		} else {
			CGAtomCluster* as = (CGAtomCluster*) c;
			CGStar& s = as->real();
			g = (Galaxy*) s.parent();
		}

		// check whether the galaxy or its
		// ancestor is to be clustered or not.
		while (g) {
			if (targetGalList.member(g)) break;
			g = (Galaxy*) g->parent();
		}
		if (!g) {
			continue;
		} else if (g != curGal) {
			ListIter nextBag(bagList);
			CGMacroClusterBag* m;
			curBag = 0;
			while ((m = (CGMacroClusterBag*) nextBag++) != 0) {
				if (g == m->real()) {
					curBag = m;
					break;
				}
			}
			if (!curBag) {
				curBag = createGalBag(g);
				bagList.prepend(curBag);
			}
			curGal = g;
		}
		c->setVisit(1);
		// if a bag contains a star with internal state,
		if (!c->isParallelizable()) {
			curBag->setStickyFlag(TRUE);
		}
		incrementalSearch(c, curBag);
		curBag->absorb(c, this);
		nextClust.reset();
	}

	if (logstrm)
		*logstrm << "Ending clustering galaxies. \n";

	return TRUE;
}

// create a CGMacroClusterBag associated with the argument galaxy if not
// created yet. Return the CGMacroClusterBag.
CGMacroClusterBag* CGMacroClusterGal::createGalBag(Galaxy* g) {
	
	LOG_NEW; CGMacroClusterBag* curBag = new CGMacroClusterBag(g);
	addBlock(curBag->setBlock(genBagName(), this));
	curBag->resetId();	// make it look like an atomic cluster
	if (logstrm)
		*logstrm << curBag->name() << "(" <<
		g->className() << ")\n";
	return curBag;
}
		
// select the target galaxies for clustering
void CGMacroClusterGal :: selectTargetGals(Galaxy& outGal) {
	// complete the list by traversing the original galaxy.
	GalTopBlockIter topIter(outGal);
	Block* b;
	topIter.reset();
	while ((b = topIter++) != 0) {
		if (!b->isItAtomic()) { // this block is a galaxy

			Galaxy& g = b->asGalaxy();
	
			IntState* ist = 
				(IntState*) g.stateWithName("asFunc");
			if (ist && int(*ist)) { // compulsory clustering
				targetGalList.append(&g);
			}
			selectTargetGals(g);  // recursive call
		}
	}
}

void CGMacroClusterGal :: incrementalSearch(CGMacroCluster* c, CGMacroClusterBag* bag) {
	CGClustPortIter nextP(*c);
	CGClustPort* p;
	while ((p = nextP++) != 0) {
		CGMacroCluster* s = p->far()->parentClust();
		if (s->visited()) continue;
		CGMacroClusterBag* temp = (CGMacroClusterBag*) s->asSpecialBag();
		Galaxy* g;
		if (temp) {
			g = (Galaxy*) temp->real()->parent();
		} else {
			CGAtomCluster* as = (CGAtomCluster*) s;
			CGStar& cs = as->real();
			g = (Galaxy*) cs.parent();
		}
		if (g == bag->real()) { // binggo
			s->setVisit(1);
			if (!s->isParallelizable()) bag->setStickyFlag(TRUE);
			incrementalSearch(s, bag);
			bag->absorb(s, this);
		}
	}
}

	
// do global clustering. Perform clustering inside the galaxy cluster, first.
int CGMacroClusterGal :: globalCluster() {

	// clustering inside each galaxy cluster
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c;

	while ((c = (CGMacroCluster*) nextClust++) != 0) {
		CGMacroClusterBag* bag = (CGMacroClusterBag*) c->asSpecialBag();
		if (bag) {
			if (!bag->clusterMyGal()) return FALSE;
		} 
	}

	// do outside cluster
	if (logstrm) {
		if (owner) 
		   *logstrm << "Galaxy-Cluster(" << owner->name() << ")";
		else
		   *logstrm << "Global-Cluster";
		*logstrm << "\n------------------------------------\n";
		*logstrm << "(a) clusterSCC begins\n";
	}
	// cluster of strongly connected component.
	if (clusterSCC() && owner) 
		owner->setStickyFlag(TRUE);	// if a galaxy cluster contains
						// SCC, declare it sticky.
	if (logstrm)
		*logstrm << "(b) loop clustering begins\n";
	return cluster();
}

// do parallel scheduling of inside clusters
int CGMacroClusterGal :: recursiveSched(MultiTarget* t) {
	if (logstrm && (!owner)) {
		*logstrm << "\n recursive parallel scheduling begins:\n";
	}
	
	// scheduling from the inside.
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c;

	while ((c = (CGMacroCluster*) nextClust++) != 0) {
		CGMacroClusterBag* bag = (CGMacroClusterBag*) c->asSpecialBag();
		if (bag) {
			if (logstrm) {
				*logstrm << "parallel schedule -- cluster " 
					 << bag->name() << "\n";
			}
			if (!bag->clusterGal()->recursiveSched(t)) 
				return FALSE;
		} 
	}

	// now, scheduling myself.
	if (owner) {
		return owner->parallelSchedule(t);
	}
	return TRUE;
}

// Detect all strongly connected components and cluster them
// to decompose the galaxy.
// return TRUE if find any.
int CGMacroClusterGal :: clusterSCC() {
	if (numberClusts() <= 1) return TRUE;

	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c;

	int change = TRUE;

	while (change) {
		change = FALSE;

		// reset visit flags.
		nextClust.reset();
		while ((c = nextClust++) != 0) {
			c->setVisit(0);
		}
		nextClust.reset();
		
		// find a strongly connected components.
		while ((c = nextClust++) != 0) {
			SequentialList* clist = 0;
			if (!(c->visited()) && (clist = findSCComponent(c))) {
				makeSCCCluster(clist);
				LOG_DEL; delete clist;
				change = TRUE;
				break;
			}
		}
	}

	// perform clustering inside the SCC clusters
	// make SCC cluster look like a atomic in the next clustering phase
	nextClust.reset();
	CGMacroClusterBag* mbag;
	if (logstrm) {
		*logstrm << "-- SCCs detected are: \n";
	}
	while ((c = nextClust++) != 0) {
		if ((mbag = (CGMacroClusterBag*) c->asBag())) {
			if (logstrm)  *logstrm << "--" << *mbag << "\n";
			mbag->resetId();	// make it atomic
			mbag->setStickyFlag(TRUE);
			mbag->clusterGal()->cluster();   // internal clustering
		}
	}
	return TRUE;
}

// find a strongly connected component
// depth-first search.
SequentialList* CGMacroClusterGal :: findSCComponent(CGMacroCluster* c) {
	c->setVisit(1);
		
	CGClustPortIter nextP(*c);
	CGClustPort* p;

	while ((p = nextP++) != 0) {
		// downward direction.
		if (p->isItInput()) continue;

		CGMacroCluster* peer = p->far()->parentClust();
		if (peer == c) continue;
		if (peer->visited() > 0) {
			return makeList(c, peer);
		} else if (peer->visited() == 0) {
			SequentialList* tempList = findSCComponent(peer);
			if (tempList) return tempList;
		}
	}
	c->setVisit(-1);
	return 0;
}

// The nodes between the stop node and the start node are
// strongly connected. We trace back the nodes from the start node
// to the stop node to identify those nodes.
SequentialList* CGMacroClusterGal :: makeList(CGMacroCluster* start, CGMacroCluster* stop) {

	LOG_NEW; SequentialList* newList = new SequentialList;

	CGMacroCluster* path = start;
	
	while (path != stop) {
		newList->prepend(path);

		CGClustPortIter nextP(*path);
		CGClustPort* p;

		while ((p = nextP++) != 0) {
			//  upward direction.
			if (p->isItOutput()) continue;
			CGMacroCluster* peer = p->far()->parentClust();
			if (peer == path) continue;
			if (peer->visited()) {
				path = peer;
				break;
			}
		}
	}
	newList->prepend(stop);
	return newList;
}
	
// Make clusters of strongly connected graph.
void CGMacroClusterGal :: makeSCCCluster(SequentialList* nlist) {

	CGMacroClusterBag* bag = 0;
	
	// check whether there is a cluster Bag in the list.
	// If yes, choose one.
	ListIter nodes(*nlist);
	CGMacroCluster* c;
	while ((c = (CGMacroCluster*) nodes++) != 0) {
		// note that a galaxy cluster looks like an atomic cluster!
		if ((bag = (CGMacroClusterBag*) c->asBag())) break;
	}
	if (!bag) {
		bag = createBag();
		addBlock(bag->setBlock(genBagName(), this));
	}

	nodes.reset();
	while ((c = (CGMacroCluster*) nodes++) != 0) {
		if (c->asBag()) {
			if (c != bag) bag->merge((CGMacroClusterBag*)c, this);
		} else {
			bag->absorb(c,this);
		}
	}
	bag->resetId();		// now make this bag look like an atomic
				// cluster in the following clustering stage.
}

/////////////////// End of unique methods /////////////////////////////

// constructor: make empty bag.
CGMacroClusterBag :: CGMacroClusterBag(Galaxy * g)
: maxParallelism(1), gal(0), pGal(g), mtarget(0), 
  owner(TRUE), exCount(0), sched(0), idFlag(1) 
	{ setVisit(0); }

// destroy the bag.
CGMacroClusterBag::~CGMacroClusterBag() {
	if (!owner) return;
	LOG_DEL; delete gal;
	LOG_DEL; delete sched;
	LOG_DEL; delete [] profile;
}

void CGMacroClusterBag :: createInnerGal() {
	LOG_DEL; delete gal;
	LOG_NEW; gal = new CGMacroClusterGal(this);
	if (pGal) {
		gal->myGal = pGal;
		gal->setNameParent(pGal->name(), pGal->parent());
	}
	if (parent())
		gal->dupStream((CGMacroClusterGal*)parent());
}

// This function absorbs an atomic cluster into a bag.

void CGMacroClusterBag::absorb(CGMacroCluster* c,CGMacroClusterGal* par) {
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
	CGClustPortIter next(*c);
	CGClustPort* cp;
	while ((cp = next++) != 0) {
		CGClustPort* pFar = cp->far();
		if (pFar && pFar->parent() == this) {
			// the far side of this guy is one of my bag pointers.
			// zap it and connect directly.
			CGClustPort* p = pFar->inPtr();
			int del = cp->numTokens();
			LOG_DEL; delete pFar;
			cp->connect(*p, del);
			cp->initGeo();
		}
		else {
			// add a new connection to the outside for this guy
			LOG_NEW; CGClustPort *np =
				new CGClustPort(*cp,this,1);
			cp->makeExternLink(np);
			addPort(*np);
		}
	}
}

// This method tests whether a CGMacroClusterGal is a tree structure or not.
// It uses the visit flags.

int CGMacroClusterGal::isTree() {
	CGMacroClusterGalIter nextClust(*this);
	CGMacroCluster* c;
	int num = numberClusts();

	// clear visit flags.
	while ((c = nextClust++) != 0) {
		c->setVisit(0);
	}

	if (logstrm)
		*logstrm << "isTree: " << num << " clusters\n";

	// for each pass, we "lop off"
	// clusters with their visit flags set.  So, for example, a
	// cluster that is fed only by source stars is now considered
	// a source star.  We keep doing this until we either eliminate
	// everything or there is a loop and we shrink no more.

	int passNum = 0;
	while (1) {
		int prevNum = num;
		num = 0;
		nextClust.reset();
		while ((c = nextClust++) != 0) {
			if (c->visited()) continue;
			int nI = 0, nO = 0;
			CGClustPortIter nextPort(*c);
			CGClustPort* p;
			while ((p = nextPort++) != 0) {
				if (p->far() == 0 ||
				    p->far()->parentClust()->visited())
					continue;
				if (p->isItInput()) nI++;
				else nO++;
			}
			// nI is the number of inputs that talk to
			// non-visited nodes, nO is the number of
			// outputs that talk to non-visited nodes.
			// if either is zero, this node is the root
			// of a tree and can be chopped.
			if (nI == 0 || nO == 0)
				c->setVisit(1);
			else num++;
		}
		passNum++;
		if (logstrm)
			*logstrm << "After pass " << passNum << ": "
				 << num << " non-source/non-sink clusters\n";
		if (num <= 1) return TRUE;
		// pass changed nothing: there is a loop.
		else if (num == prevNum) return FALSE;
	}
}

// this function merges two bags.
void
CGMacroClusterBag::merge(CGMacroClusterBag* b,CGMacroClusterGal* par) {
	if (b->size() == 0) return;
	if (!gal) createInnerGal();
	// get a list of all "bagports" that connect the two clusters.
	// we accumulate them on one pass and delete on the next to avoid
	// problems with iterators.
	CGClustPortIter nextbp(*this);
	CGClustPort* p;
	SequentialList zap;
	while ((p = nextbp++) != 0)
		if (p->far() && p->far()->parent() == b) zap.put(p);
	// zap is the list of connections between the two clusters.  These
	// become internal connections so we zap them from both bags' lists
	// of external pointers.
	ListIter nextZap(zap);
	while ((p = (CGClustPort*)nextZap++) != 0) {
		CGClustPort* near = p->inPtr();
		CGClustPort* far = p->far()->inPtr();
		int del = p->numTokens();
		LOG_DEL; delete p->far();
		LOG_DEL; delete p;
		near->connect(*far, del);
		near->initGeo();
	}
	// now we simply combine the remaining bagports and clusters into this.
	CGMacroClusterBagIter nextC(*b);
	CGMacroCluster* c;
	while ((c = nextC++) != 0) {
		gal->addBlock(*c,c->name());
	}
	CGClustPortIter nextP(*b);
	while ((p = nextP++) != 0) {
		p->setNameParent(p->name(),this);
		addPort(*p);
	}
// get rid of b.
	par->removeBlock(*b); // remove from parent galaxy
	b->owner = FALSE;     // prevent from zapping contents, I took them
	LOG_DEL; delete b;    // zap the shell
}

// internal clustering for a bag
int CGMacroClusterBag::internalClustering() {
	return size() >= 2 ? gal->parallelLoopMergePass() : FALSE;
}

// print a bag on a stream.
ostream& CGMacroClusterBag::printOn(ostream& o) {
	printBrief(o);
	o << "= {";
	CGMacroClusterBagIter next(*this);
	CGMacroCluster* c;
	while (( c = next++) != 0) {
		o << " ";
		c->printBrief(o);
	}
	if (numberPorts() == 0) return o << " }\n";
	o << " }\nports:\t";
	return printPorts(o);
}

// The following procedures are used in the propagation of arc
// counts from outer to inner arcs.
int CGClustPort::maxArcCount() {
	CGClustPort* p = this;
	while (p && !p->geo()) p = p->outPtr();
	if (p) return p->geo()->maxNumParticles();
	Error::abortRun(*this, "can't find outer geodesic");
	return 0;
}

// if I have a geodesic, set its count, else look inside.
void CGClustPort::setMaxArcCount(int n) {
	CGClustPort* p = this;
	while (p && !p->geo()) {
		CGClustPort* q = p->inPtr();
		if (q == 0) {
			p->real().geo()->setMaxArcCount(n);
			return;
		}
		else p = q;
	}
	if (p) p->geo()->setMaxArcCount(n);
	else Error::abortRun(*this, "can't find inner geodesic");
}

// propagate buffer sizes downward to set sizes of cluster-boundary
// buffers.
void CGMacroClusterBag::fixBufferSizes(int nReps) {
	// account for looping of this cluster
	nReps *= loop();
	// pass down sizes of external buffers.
	CGClustPortIter nextPort(*this);
	CGClustPort* p;
	while ((p = nextPort++) != 0) {
		CGClustPort* in = p->inPtr();
		int n = p->maxArcCount();
		in->setMaxArcCount(n);
	}
	CGMacroClusterBagIter nextClust(*this);
	CGMacroCluster* c;
	while ((c = nextClust++) != 0)
		c->fixBufferSizes(nReps);
}

// clustering inside galaxy
int CGMacroClusterBag :: clusterMyGal() { return gal->globalCluster(); }

// prepare itself
int CGMacroClusterBag :: prepareBag(MultiTarget* t, ostream* l) {
	// mtarget member setup
	mtarget = t;

	// create scheduler and profile
	if (!sched) {
		LOG_NEW; sched = new MacroParSched(t,l);
	}
	int numProcs = t->nProcs();
	LOG_NEW; profile = new Profile[numProcs];
	for (int i = 0; i < numProcs; i++) {
		profile[i].create(i+1);
		profile[i].createAssignArray(reps());
	}

	sched->setGalaxy(*gal);
	if(!sched->repetitions()) return FALSE;

	optNum = 1;	// initialize optNum

	// adjust sample rate of myself
	CGClustPortIter nextPort(*this);
	CGClustPort* p;
	while ((p = nextPort++) != 0) {
		int i = p->numIO() * p->inPtr()->parentClust()->reps();
		p->setNumXfer(i);
	}

	return recurPrepareBag(t,l);
}

int CGMacroClusterBag :: recurPrepareBag(MultiTarget* t, ostream* l) {
	// scheduling from the inside.
	CGMacroClusterGalIter nextClust(*gal);
	CGMacroCluster* c;

	while ((c = (CGMacroCluster*) nextClust++) != 0) {
		CGMacroClusterBag* bag = (CGMacroClusterBag*) c->asSpecialBag();
		if (bag) {
			if (!bag->prepareBag(t, l))
				return FALSE;
		} 
	}
	return TRUE;
}

int CGMacroClusterBag::parallelSchedule(MultiTarget* t) {
	sched->setGalaxy(*gal);
	if (gal) { 
		IntState* fix = (IntState*)t->galParam(gal, "fixedNum");
		if (fix) {
	 		int fixNp = int(*fix);
			sched->setUpProcs(fixNp);
			t->resetResources();
			sched->generateSchedule(gal);
			if (Scheduler::haltRequested()) return FALSE;
			sched->setProfile(&profile[fixNp-1]);
			return TRUE;
		}
	}

	sched->setUpProcs(1);
	t->resetResources();
	int prev = sched->generateSchedule(gal);
	if (Scheduler::haltRequested()) return FALSE;
	sched->setProfile(&profile[0]);
	int limit = 1;

	for (int i = 2; i <= t->nProcs(); i++) {
		t->resetResources();
		sched->setUpProcs(i);
		int leng = sched->mainSchedule();
		if (Scheduler::haltRequested()) return FALSE;
		if (leng >= prev)  break;
		limit = i;
		prev = leng;
		sched->setProfile(&profile[i-1]);
	}
	for (; i <= t->nProcs(); i++)
		sched->setProfile(&profile[i-1]);
	maxParallelism = limit;
	return TRUE;
}

	
Profile* CGMacroClusterBag :: getProfile(int i) { return profile + i; }

int CGMacroClusterBag :: examineProcIds(IntArray& procId) {

	// first check whether all stars on the boundary have the same procId.
	// If not, signal error
	// The processor assigned to the boundary should be procId[0]
	CGClustPortIter nextP(*this);
	CGClustPort* p;
	int temp = -1;
	while ((p = nextP++) != 0) {
		CGClustPort* inp = p->inPtr();
		while (inp->parentClust()->asSpecialBag()) inp = inp->inPtr();
		CGAtomCluster* s = (CGAtomCluster*) inp->parentClust();
		if (s->real().isItFork()) continue;
		if (temp < 0) {
			temp = s->getProcId();
			if(procId.size() > 0) {
				procId[0] = temp;
			}
		} else if (s->getProcId() != temp) {
			Error::abortRun("Stars on the cluster boundary ",
		    "should be assigned to the same processor! Check again");
		    return -1;
		}
	}

	// now collect all procIds
	CGMacroClusterGalIter next(*gal);
	CGMacroCluster* c;
	while ((c = next++) != 0) {
		if (c->asSpecialBag()) {
			int x = c->asSpecialBag()->examineProcIds(procId);
			if (x < 0) return FALSE;
		} else {
			CGAtomCluster* as = (CGAtomCluster*) c;
			if (as->real().isItFork()) continue;
			int y = as->getProcId();
			if ((y > mtarget->nProcs()) || y < 0) {
			    Error::abortRun(as->real(), " invalid procId");
			    return -1;
			}
			int ix = 0;
			while (procId[ix] != y) {
				if (procId[ix] < 0) {
					procId[ix] = y;
				} else {
					ix++;
				}
			}
		}
	}
	for (int sumP = 0; sumP < mtarget->nProcs(); sumP++) {
		if (procId[sumP] < 0) break;
	}
	return sumP;
}

// TO_BE_FILLED LATER
int CGMacroClusterBag :: computeProfile(int nP, int /*rW*/,
					IntArray* /*avail*/) {
	numProcs = nP;
	return TRUE;
}

Profile* CGMacroClusterBag :: manualSchedule(int) {
	return 0;
}

void CGMacroClusterBag :: downLoadCode(CGStar*, int /*invoc*/, int /*pix*/) {
}

// TO_BE_FILLED LATER END

int CGMacroClusterBag :: finalSchedule(int numProcs) { 
	Profile* pf = profile + numProcs - 1;
	sched->setUpProcs(optNum);
	if (!mtarget->reorderChildren(pf->assignArray(1))) return FALSE;
	if (!sched->mainSchedule()) return FALSE;
	sched->mapTargets();
	if (!sched->createSubGals(*gal)) return FALSE;
	if (!sched->finalSchedule()) return FALSE;
	mtarget->restoreChildOrder();
	return TRUE; 
}

// return the bag's schedule.
StringList CGMacroClusterBag::displaySchedule() {
	StringList sch;
	sch += sched->displaySchedule();
	return sch;
}

// find an attractive and compatible neighbor.
// This is the "quick version" -- it assumes that if the source has
// multiple outputs or the destination has multiple inputs, an alternate
// path might occur.  fullSearchMerge is the "slow version".
CGMacroCluster* CGMacroCluster::mergeCandidate() {
	CGMacroCluster *src = 0, *dst = 0;
	int multiSrc = 0, multiDst = 0;
	CGMacroCluster* srcOK = 0, *dstOK = 0;
	CGClustPortIter nextPort(*this);
	CGClustPort* p;
	while ((p = nextPort++) != 0) {
		CGClustPort* pFar = p->far();
		if (pFar == 0) continue;
		CGMacroCluster* peer = pFar->parentClust();
		int myIO = p->numIO();
		int peerIO = pFar->numIO();
		// a connection is ok to use for merging if there are
		// no delays and no sample rate changes.

		int ok = (!p->fbDelay() && myIO == peerIO);
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
static int uniqueIndex = 0;
static const char* mungeName(NamedObj& o) {
	StringList name = o.name();
	name << uniqueIndex++;
	return hashstring(name);
}

// create an atomic cluster to surround a single CGStar.
CGAtomCluster::CGAtomCluster(CGStar& star,Galaxy* parent) : pStar(star)
{
	DFStarPortIter nextPort(star);
	DFPortHole* p;
	while ((p = nextPort++) != 0) {
		// do not make a port in the cluster if it is a "loopback" port
		// also don't make a port for a wormhole connection.
		if (p->atBoundary() || p->far()->parent() == &star) continue;
		LOG_NEW; CGClustPort *cp = new CGClustPort(*p,this);
		addPort(*cp);
		cp->numIO();	// test
	}
	setNameParent(mungeName(star),parent);
	repetitions = pStar.repetitions;
	procId = star.getProcId();
	if(star.isItFork()) isaFork();
}

// destroy an atomic cluster.  Main job: delete the CGClustPorts.
CGAtomCluster::~CGAtomCluster() {
	BlockPortIter nextPort(*this);
	PortHole* p;
	while ((p = nextPort++) != 0) {
		// avoid removal from parent list.
		p->setNameParent("",0);
		LOG_DEL; delete p;
	}
}

// isA function
int CGAtomCluster :: isA(const char* cname) const 
	{ return pStar.isA(cname); }

// print functions.
ostream& CGMacroCluster::printBrief(ostream& o) {
	if (pLoop > 1) {
		o << pLoop << "*";
	}
	return o << name();
}

ostream& CGAtomCluster::printOn(ostream& o) {
	printBrief(o);
	o << "\nports:\t";
	return printPorts(o);
}

// set the buffer sizes of the actual buffers -- we can pass down all
// but the self-loops.
// A self-loop buffer size is just numInitDelays * numXfer.
// For a wormhole boundary, the size is nReps * numXfer (the
// total # moved throughout the execution of the schedule).
void CGAtomCluster::fixBufferSizes(int nReps) {
	// # times real star is executed
	nReps *= loop();
	CGClustPortIter nextPort(*this);
	CGClustPort *cp;
	while ((cp = nextPort++) != 0) {
		DFPortHole& rp = cp->real();
		int sz = cp->maxArcCount();
		rp.setMaxArcCount(sz);
	}
	// set repetitions
	pStar.repetitions = Fraction(nReps,1);
	// may need to set self-loops separately
	if (numberPorts() == pStar.numberPorts()) return;
	// there are additional ports (selfloops, wormholes); set their sizes
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

int CGAtomCluster::myExecTime() {
	return pStar.myExecTime();
}

int CGMacroClusterBag::myExecTime() {
	return sched->getTotalWork();
}

// constructor for CGClustPort, port for use in cluster.
// if bp is set it's a "bag port" belonging to an CGMacroClusterBag.
CGClustPort::CGClustPort(DFPortHole& port,CGMacroCluster* parent, int bp)
: pPort(port), pOutPtr(0), bagPortFlag(bp), feedForwardFlag(0)
{
	/* const char* name = bp? port.name() : mungeName(port); */
	const char* name = bp? mungeName(port) :  port.name();
	setPort(name,parent,INT);
	myPlasma = Plasma::getPlasma(INT);
	numberTokens = port.numXfer();
}

void CGClustPort::initGeo() { myGeodesic->initialize();}

// This method is called on a cluster porthole to create a connection
// with a "bag port" in the parent ClusterBag.  

void CGClustPort::makeExternLink(CGClustPort* bagPort) {
	pOutPtr = bagPort;
	bagPort->numberTokens = numberTokens;
	// if I am connected, disconnect me and connect my peer
	// to my external link (the bagPort)
	CGClustPort* pFar = far();
	if (pFar) {
		int del = numTokens();
		disconnect();
		bagPort->connect(*pFar,del);
		bagPort->initGeo();
	}
}

// return the real far port aliased to bag ports.
CGClustPort* CGClustPort :: realFarPort(CGMacroCluster* outsideCluster) {
	if (parentClust() == outsideCluster) return 0;
	if (far()) return far();
	return pOutPtr->realFarPort(outsideCluster);
}

static const char nonUniformMessage[] =
"The loop scheduler could not completely cluster the topology.\n"
"The generated schedule is valid, but may be substantially longer than\n"
"expected (with negative consequences if used in code generation).  Sorry.";

