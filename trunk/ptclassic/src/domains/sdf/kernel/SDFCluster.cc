/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

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
static const char file_id[] = "SDFCluster.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "SDFCluster.h"
#include "GalIter.h"
#include "Geodesic.h"
#include "Plasma.h"
#include "Target.h"
#include "streamCompat.h"
#include "Error.h"

// A SDFClusterGal is a Galaxy, built from another galaxy.

// function to print an SDFClusterGal on an ostream.
ostream& operator<< (ostream& o, SDFClusterGal& g) {
	SDFClusterGalIter next(g);
	SDFCluster* c;
	while ((c = next++) != 0) {
		o << *c << "\n";
	}
	return o;
}

// constructor: builds a flat galaxy of SDFAtomCluster objects.

SDFClusterGal::SDFClusterGal(Galaxy& gal, ostream* log)
: bagNumber(1), logstrm(log)
{
	int nports = setPortIndices(gal);
	LOG_NEW; SDFClustPort** ptable = new SDFClustPort*[nports];
	GalStarIter nextStar(gal);
	SDFStar* s;
	while ((s = (SDFStar*)nextStar++) != 0) {
		LOG_NEW; SDFAtomCluster* c = new SDFAtomCluster(*s,this);
		addBlock(*c);
		SDFClustPortIter nextPort(*c);
		SDFClustPort *p;
		while ((p = nextPort++) != 0) {
			ptable[p->real().index()] = p;
		}
	}
	// now connect up the cluster ports to match the real ports
	for (int i = 0; i < nports; i++) {
		SDFClustPort* out = ptable[i];
		if (out->isItInput()) continue;
		SDFClustPort* in = ptable[out->real().far()->index()];
		int delay = out->real().numTokens();
		out->connect(*in,delay);
		out->initGeo();
	}
	LOG_DEL; delete ptable;
}

// Core clustering routine.  Alternate merge passes and loop passes
// until no more can be done.
int SDFClusterGal::clusterCore(int& urate) {
	int change = FALSE;
	while ((urate = uniformRate()) == FALSE) {
                int status = mergePass();
                status |= loopPass();
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

int SDFClusterGal::cluster() {
	int urate;
	int change = clusterCore(urate);

	// if we did not get to uniform rate, try marking feedforward
	// delay arcs as ignorable and clustering some more.

	if (!urate && markFeedForwardDelayArcs()) {
		change |= clusterCore(urate);
	}
	if (!urate)
		Error::warn("clustering algorithm did not achieve uniform rate");
	if (change) {
		if (logstrm)
			*logstrm << "Looking for parallel loops\n";
		parallelLoopMergePass();
	}
	return change;
}

// Test to see if we have uniform rate.
int SDFClusterGal::uniformRate() {
	if (numberClusts() <= 1) return TRUE;
	SDFClusterGalIter nextClust(*this);
	SDFCluster* c = nextClust++;
	int rate = c->reps();
	while ((c = nextClust++) != 0)
		if (rate != c->reps()) return FALSE;
	return TRUE;
}

// Merge adjacent actors that can be treated as a single cluster.
int SDFClusterGal::mergePass() {
	if (numberClusts() <= 1) return FALSE;
	int changes = 0;
	SDFClusterGalIter nextClust(*this);
	SDFCluster *c1, *c2 = 0;
	do {
		while ((c1 = nextClust++) != 0) {
			c2 = c1->mergeCandidate();
			if (c2) break;
		}
		if (c2) {
			c1 = merge(c1, c2);
			// keep expanding the result as much as possible.
			SDFCluster* c3;
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
SDFCluster* SDFClusterGal::fullSearchMerge() {
	if (logstrm)
		*logstrm << "Starting fullSearchMerge\n";
	SDFClusterGalIter nextClust(*this);
	SDFCluster* c;
	while ((c = nextClust++) != 0) {
		SDFClustPortIter nextPort(*c);
		SDFClustPort *p;
		while ((p = nextPort++) != 0) {
			// check requirement 1: same rate and no delay
			if (!p->fbDelay() &&
			    p->numIO() == p->far()->numIO()) {
				SDFCluster *peer = p->far()->parentClust();
				SDFCluster *src, *dest;
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

int SDFClusterGal::parallelLoopMergePass() {
	// See if we can do a merge of two disconnected
	// clusters that have the same loop factor.
	SDFClusterGalIter nextClust(*this);
	int changes = FALSE;
	SDFCluster *c;
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
		SDFClusterGalIter i2(nextClust);
		SDFCluster* c2;
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

int SDFClusterGal::indirectPath(SDFCluster* src, SDFCluster* dst) {
	stopList.initialize();
	stopList.put(src);
	SDFClustPortIter nextP(*src);
	SDFClustPort* p;
	while ((p = nextP++) != 0) {
		SDFCluster* peer = p->far()->parentClust();
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

int SDFClusterGal::findPath(SDFCluster* start, SDFCluster* dst) {
	stopList.put(start);
	SDFClustPortIter nextP(*start);
	SDFClustPort* p;
	while ((p = nextP++) != 0) {
		if (p->isItInput()) continue;
		SDFCluster* peer = p->far()->parentClust();
		if (peer == dst) return TRUE;
		if (stopList.member(peer)) continue;
		if (findPath(peer,dst)) return TRUE;
	}
	return FALSE;
}

static ostream& operator<<(ostream& o, SDFClustPort& p);

// This routine marks all feed-forward delays on a clusterGal to
// enhance the looping capability without hazard of dead-lock condition.
// We suggest that this method be called after the initial clustering is 
// completed for efficiency (since searching for indirect paths is expensive)

int SDFClusterGal :: markFeedForwardDelayArcs() {
	if (logstrm)
		*logstrm << "starting markFeedForwardDelayArcs()\n";
	if (numberClusts() <= 1) return FALSE;
	int changes = FALSE;
	SDFClusterGalIter nextClust(*this);
	SDFCluster* c;

	while ((c = nextClust++) != 0) {
		SDFClustPortIter nextP(*c);
		SDFClustPort* p;
		while ((p = nextP++) != 0) {
			if (p->isItInput()) continue;
			if (p->numTokens() == 0) continue;
			// this port has a delay on the arc.
			SDFCluster* peer = p->far()->parentClust();
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

int SDFClusterGal::loopPass() {
	if (logstrm)
		*logstrm << "Starting loop pass\n";
	if (numberClusts() <= 1) return FALSE;
	int changes = 0;
	SDFClusterGalIter nextClust(*this);

	// special handling for two clusters: do arbitrary rate
	// changes if no delays between the clusters and rate
	// mismatch exists.
	// We can ignore feedforward delays.
	if (numberClusts() == 2) {
		SDFCluster* c1 = nextClust++;
		SDFCluster* c2 = nextClust++;
		// check to see that there are no delays on arcs,
		// and that a sample rate change is needed.
		SDFClustPortIter nextPort(*c1);
		SDFClustPort* p;
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
	SDFCluster *c;
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

// This function determines the loop factor for a cluster.
// We attempt to find a factor to repeat the loop by that will make
// it better match the sample rate of its neighbors.
int SDFCluster::loopFactor() {
	int retval = 0;
	SDFClustPortIter nextPort(*this);
	SDFClustPort* p;
	while ((p = nextPort++) != 0) {
		SDFClustPort* pFar = p->far();
		int myIO = p->numIO();
		int peerIO = pFar->numIO();
		// can't loop if connected to a different rate star by a delay
		// (unless it is a feedforward).
		if (p->fbDelay() && myIO != peerIO) return 0;
		// don't loop if peer should loop first
		if (myIO > peerIO) return 0;
		// try looping if things go evenly.
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
static ostream& operator<<(ostream& o, SDFClustPort& p) {
	o << p.parent()->readName() << "." << p.readName();
	SDFClustPort* pFar = p.far();
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
ostream& SDFCluster::printPorts(ostream& o) {
	const char* prefix = "( ";
	SDFClustPortIter next(*this);
	SDFClustPort* p;
	while ((p = next++) != 0) {
		o << prefix << *p;
		prefix = "\n\t  ";
	}
	return o << " )";
}

// loop a cluster.  Adjust repetitions and token counts.
void SDFCluster::loopBy(int fac) {
	pLoop *= fac;
	repetitions.numerator /= fac;
	SDFClustPortIter nextPort(*this);
	SDFClustPort* p;
	while ((p = nextPort++) != 0)
		p->numberTokens *= fac;
}

// unloop a cluster.  Return old loop factor.  This undoes the effect
// of previous loopBy calls.
int SDFCluster::unloop() {
	int fac = pLoop;
	pLoop = 1;
	repetitions.numerator *= fac;
	SDFClustPortIter nextPort(*this);
	SDFClustPort* p;
	while ((p = nextPort++) != 0)
		p->numberTokens /= fac;
	return fac;
}

// generate a name for a new bag
const char* SDFClusterGal::genBagName() {
	char buf[20];
	sprintf (buf, "bag%d", bagNumber++);
	return hashstring(buf);
}

// This function combines two clusters into one.  There are four cases:
// basically the distinction between atomic and non-atomic clusters.

SDFCluster* SDFClusterGal::merge(SDFCluster* c1, SDFCluster* c2) {
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

	SDFClusterBag* c1Bag = (c1->loop() == 1) ? c1->asBag() : 0;
	SDFClusterBag* c2Bag = (c2->loop() == 1) ? c2->asBag() : 0;
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
		LOG_NEW; SDFClusterBag* bag = new SDFClusterBag;
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
SDFClusterBag :: SDFClusterBag()
: owner(TRUE), exCount(0), sched(0), gal(0)
{}

void SDFClusterBag :: createInnerGal() {
	LOG_DEL; delete gal;
	LOG_NEW; gal = new SDFClusterGal;
	if (parent())
		gal->dupStream((SDFClusterGal*)parent());
}

// This function absorbs an atomic cluster into a bag.

void SDFClusterBag::absorb(SDFCluster* c,SDFClusterGal* par) {
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
	SDFClustPortIter next(*c);
	SDFClustPort* cp;
	while ((cp = next++) != 0) {
		SDFClustPort* pFar = cp->far();
		if (pFar->parent() == this) {
			// the far side of this guy is one of my bag pointers.
			// zap it and connect directly.
			SDFClustPort* p = pFar->inPtr();
			int del = cp->numTokens();
			LOG_DEL; delete pFar;
			cp->connect(*p, del);
			cp->initGeo();
		}
		else {
			// add a new connection to the outside for this guy
			LOG_NEW; SDFClustPort *np =
				new SDFClustPort(*cp,this,1);
			cp->makeExternLink(np);
			addPort(*np);
		}
	}
}

// this function merges two bags.
void
SDFClusterBag::merge(SDFClusterBag* b,SDFClusterGal* par) {
	if (b->size() == 0) return;
	if (!gal) createInnerGal();
	// get a list of all "bagports" that connect the two clusters.
	// we accumulate them on one pass and delete on the next to avoid
	// problems with iterators.
	SDFClustPortIter nextbp(*this);
	SDFClustPort* p;
	SequentialList zap;
	while ((p = nextbp++) != 0)
		if (p->far()->parent() == b) zap.put(p);
	// zap is the list of connections between the two clusters.  These
	// become internal connections so we zap them from both bags' lists
	// of external pointers.
	ListIter nextZap(zap);
	while ((p = (SDFClustPort*)nextZap++) != 0) {
		SDFClustPort* near = p->inPtr();
		SDFClustPort* far = p->far()->inPtr();
		int del = p->numTokens();
		LOG_DEL; delete p->far();
		LOG_DEL; delete p;
		near->connect(*far, del);
		near->initGeo();
	}
	// now we simply combine the remaining bagports and clusters into this.
	SDFClusterBagIter nextC(*b);
	SDFCluster* c;
	while ((c = nextC++) != 0) {
		gal->addBlock(*c,c->readName());
	}
	SDFClustPortIter nextP(*b);
	while ((p = nextP++) != 0) {
		p->setNameParent(p->readName(),this);
		addPort(*p);
	}
// get rid of b.
	par->removeBlock(*b); // remove from parent galaxy
	b->owner = FALSE;     // prevent from zapping contents, I took them
	LOG_DEL; delete b;    // zap the shell
}

// internal clustering for a bag
int SDFClusterBag::internalClustering() {
	return size() >= 2 ? gal->parallelLoopMergePass() : FALSE;
}

// print a bag on a stream.
ostream& SDFClusterBag::printOn(ostream& o) {
	printBrief(o);
	o << "= {";
	SDFClusterBagIter next(*this);
	SDFCluster* c;
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

int SDFClusterBag::simRunStar(int deferFiring) {
	// handle FORCE input
	if (deferFiring == FORCE)
		noTimes = 0;
	int status = SDFStar::simRunStar(deferFiring);
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
			SDFStar* s;
			while ((s = nextStar++) != 0)
				s->simRunStar(FORCE);
		}
	}
	return status;
}

int SDFClusterBag::genSched() {
	if (sched) return TRUE;
	LOG_NEW; sched = new SDFBagScheduler;
	return sched->setup(*gal);
}
	
// indent by depth tabs.
static const char* tab(int depth) {
	// this fails for depth > 20, so:
	if (depth > 20) depth = 20;
	static const char *tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	return (tabs + 20 - depth);
}

// return the bag's schedule.
StringList SDFClusterBag::displaySchedule(int depth) {
	if (sched == 0) genSched();
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
int SDFClusterBag::fire() {
	if (!sched) {
		if (!genSched()) return FALSE;
	}
	sched->setStopTime(loop()+exCount);
	sched->run(*gal);
	exCount += loop();
	return !Scheduler::haltRequested();
}

// destroy the bag.
SDFClusterBag::~SDFClusterBag() {
	if (!owner) return;
	LOG_DEL; delete gal;
	LOG_DEL; delete sched;
}

// find an attractive and compatible neighbor.
// This is the "quick version" -- it assumes that if the source has
// multiple outputs or the destination has multiple inputs, an alternate
// path might occur.  fullSearchMerge is the "slow version".
SDFCluster* SDFCluster::mergeCandidate() {
	SDFCluster *src = 0, *dst = 0;
	int multiSrc = 0, multiDst = 0;
	SDFCluster* srcOK = 0, *dstOK = 0;
	SDFClustPortIter nextPort(*this);
	SDFClustPort* p;
	while ((p = nextPort++) != 0) {
		SDFClustPort* pFar = p->far();
		SDFCluster* peer = pFar->parentClust();
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

// create an atomic cluster to surround a single SDFStar.
SDFAtomCluster::SDFAtomCluster(SDFStar& star,Galaxy* parent) : pStar(star)
{
	SDFStarPortIter nextPort(star);
	SDFPortHole* p;
	while ((p = nextPort++) != 0) {
		// do not make a port in the cluster if it is a "loopback" port
		if (p->far()->parent() == &star) continue;
		LOG_NEW; SDFClustPort *cp = new SDFClustPort(*p,this);
		addPort(*cp);
		cp->numIO();	// test
	}
	setNameParent(mungeName(star),parent);
	repetitions = pStar.repetitions;
}

// destroy an atomic cluster.  Main job: delete the SDFClustPorts.
SDFAtomCluster::~SDFAtomCluster() {
	BlockPortIter nextPort(*this);
	PortHole* p;
	while ((p = nextPort++) != 0) {
		// avoid removal from parent list.
		p->setNameParent("",0);
		LOG_DEL; delete p;
	}
}

// print functions.
ostream& SDFCluster::printBrief(ostream& o) {
	if (pLoop > 1) {
		o << pLoop << "*";
	}
	return o << readName();
}

ostream& SDFAtomCluster::printOn(ostream& o) {
	printBrief(o);
	o << "\nports:\t";
	return printPorts(o);
}

StringList SDFAtomCluster::displaySchedule(int depth) {
	StringList sch = tab(depth);
	if (loop() > 1) {
		sch += loop();
		sch += "*";
	}
	sch += real().readFullName();
	sch += "\n";
	return sch;
}

int SDFAtomCluster::fire() {
	for (int i = 0; i < loop(); i++) {
		if (!pStar.fire()) return FALSE;
	}
	return TRUE;
}

// simulate the execution of the atomic cluster, for schedule generation.
// We pass through the call of simRunStar to the real star.
int SDFAtomCluster::simRunStar(int deferFiring) {
	// handle FORCE input
	if (deferFiring == FORCE) {
		noTimes = 0;
		deferFiring = FALSE;
	}
	int status = SDFStar::simRunStar(deferFiring);
	if (status == 0)
		for (int i = 0; i < loop(); i++)
			pStar.simRunStar(FALSE);
	return status;
}

int SDFAtomCluster::myExecTime() {
	return pLoop * pStar.myExecTime();
}

// constructor for SDFClustPort, port for use in cluster.
// if bp is set it's a "bag port" belonging to an SDFClusterBag.
SDFClustPort::SDFClustPort(SDFPortHole& port,SDFCluster* parent, int bp)
: pPort(port), bagPortFlag(bp), pOutPtr(0), feedForwardFlag(0)
{
	const char* name = bp ? port.readName() : mungeName(port);
	setPort(name,parent,INT);
	myPlasma = Plasma::getPlasma(INT);
	numberTokens = port.numberTokens;
}

void SDFClustPort::initGeo() { myGeodesic->initialize();}

// This method is called on a cluster porthole to create a connection
// with a "bag port" in the parent ClusterBag.  

void SDFClustPort::makeExternLink(SDFClustPort* bagPort) {
	pOutPtr = bagPort;
	bagPort->numberTokens = numberTokens;
	// if I am connected, disconnect me and connect my peer
	// to my external link (the bagPort)
	SDFClustPort* pFar = far();
	if (pFar) {
		int del = numTokens();
		disconnect();
		bagPort->connect(*pFar,del);
		bagPort->initGeo();
	}
}

// return the real far port aliased to bag ports.
SDFClustPort* SDFClustPort :: realFarPort(SDFCluster* outsideCluster) {
	if (parentClust() == outsideCluster) return 0;
	if (far()) return far();
	return pOutPtr->realFarPort(outsideCluster);
}

// methods for SDFClustSched, the clustering scheduler.

SDFClustSched::~SDFClustSched() { LOG_DEL; delete cgal;}

// compute the schedule!
int SDFClustSched::computeSchedule (Galaxy& gal) {
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
	cgal = new SDFClusterGal(gal,logstrm);
	cgal->cluster();

// generate schedule
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
StringList SDFClustSched::displaySchedule() {
	StringList sch;
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*)next++) != 0) {
		sch += c->displaySchedule(0);
	}
	return sch;
}

StringList SDFBagScheduler::displaySchedule(int depth) {
	StringList sch;
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*)next++) != 0) {
		sch += c->displaySchedule(depth);
	}
	return sch;
}

// The following functions are used in code generation.  Work is
// distributed among the two scheduler and two cluster classes.
void SDFClustSched::compileRun() {
	Target& target = getTarget();
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*)next++) != 0) {
		c->genCode(target,1);
	}
}

void SDFAtomCluster::genCode(Target& t, int depth) {
	if (loop() > 1) {
		t.beginIteration(loop(), depth);
		t.writeFiring(real(), depth+1);
		t.endIteration(loop(), depth);
	}
	else t.writeFiring(real(), depth);
}

void SDFClusterBag::genCode(Target& t, int depth) {
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

void SDFBagScheduler::genCode(Target& t, int depth) {
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*)next++) != 0) {
		c->genCode(t, depth);
	}
}
