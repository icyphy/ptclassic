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

// Main clustering routine.  Alternate merge passes and loop passes
// until no more can be done.
int SDFClusterGal::cluster() {
	if (numberClusts() <= 1) return FALSE;

	int urate, changes = FALSE;
	while ((urate = uniformRate()) == FALSE) {
		if (!mergePass()) break;
		changes = TRUE;
		if (!loopPass()) break;
	}
	if (numberClusts() == 1) {
		if (logstrm)
			*logstrm << "Reduced to one cluster\n";
		return TRUE;
	}
	if (urate) {
		if (logstrm)
			*logstrm << "Uniform rate achieved: one extra merge pass now\n";
		mergePass();
	}
	return changes;
}

int SDFClusterGal::genSubScheds() {
	SDFClusterGalIter nextClust(*this);
	SDFCluster* c;
	while ((c = nextClust++) != 0)
		if (!c->genSched()) return FALSE;
	return TRUE;
}

int SDFClusterGal::uniformRate() {
	if (numberClusts() <= 1) return TRUE;
	SDFClusterGalIter nextClust(*this);
	SDFCluster* c = nextClust++;
	int rate = c->reps();
	while ((c = nextClust++) != 0)
		if (rate != c->reps()) return FALSE;
	return TRUE;
}

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
		// if no more merge candidates, try the slow merge
		// note we must always reset the iterator after a
		// merge since it might be invalid.
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
			if (p->numTokens() == 0 &&
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

// this function returns true if there exists an indirect path from the
// src cluster to the dst cluster (one that passes through another
// cluster), treating the ports as directed paths.

int SDFClusterGal::indirectPath(SDFCluster* src, SDFCluster* dst) {
	stopList.initialize();
	stopList.put(src);
	if (logstrm)
		*logstrm << "indirectPath(" << src->readName()
			<< "," << dst->readName() << ")\n";
	SDFClustPortIter nextP(*src);
	SDFClustPort* p;
	while ((p = nextP++) != 0) {
		SDFCluster* peer = p->far()->parentClust();
		if (p->isItOutput() && peer != dst && findPath(peer,dst)) {
			if (logstrm)
				*logstrm << "indirectPath(" << src->readName()
					<< "," << dst->readName()
						<< ") = TRUE\n";
			return TRUE;
		}
	}
	if (logstrm)
		*logstrm << "indirectPath(" << src->readName()
			<< "," << dst->readName() << ") = FALSE\n";
	return FALSE;
}

// this should be a part of the SequentialList class.
// it returns true if the pointer argument is a member of the list.
static int member(Pointer p,SequentialList& l) {
	ListIter next(l);
	Pointer q;
	while ((q = next++) != 0) {
		if (q == p) return TRUE;
	}
	return FALSE;
}

// This recursive function is used by indirectPath.  The member object
// stopList prevents the same paths from being checked repeatedly.
// TRUE is returned if we can reach dst from start without going through
// any cluster on the stop list.  When indirectPath calls this it puts
// src on the stop list to initialize it.

int SDFClusterGal::findPath(SDFCluster* start, SDFCluster* dst) {
	if (logstrm)
		*logstrm << "findPath(" << start->readName()
			<< "," << dst->readName() << ")\n";
	stopList.put(start);
	SDFClustPortIter nextP(*start);
	SDFClustPort* p;
	while ((p = nextP++) != 0) {
		if (p->isItInput()) continue;
		SDFCluster* peer = p->far()->parentClust();
		if (peer == dst) return TRUE;
		if (member(peer,stopList)) continue;
		if (findPath(peer,dst)) return TRUE;
	}
	return FALSE;
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
	if (numberClusts() == 2) {
		SDFCluster* c1 = nextClust++;
		SDFCluster* c2 = nextClust++;
		// check to see that there are no delays on arcs,
		// and that a sample rate change is needed.
		SDFClustPortIter nextPort(*c1);
		SDFClustPort* p;
		while ((p = nextPort++) != 0) {
			if (p->numTokens() > 0 ||
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
		if (p->numTokens() && myIO != peerIO) return 0;
		// don't loop if I already match a peer's rate or if peer
		// should loop first.
		if (myIO >= peerIO) return 0;
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

// fn to print ports of a cluster.
ostream& SDFCluster::printPorts(ostream& o) {
	const char* prefix = "( ";
	SDFClustPortIter next(*this);
	SDFClustPort* p;
	while ((p = next++) != 0) {
		o << prefix << p->parent()->readName() << "." << p->readName();
		SDFClustPort* pFar = p->far();
		if (p->isItOutput())
			o << "=>";
		else
			o << "<=";
		if (!pFar) o << "0";
		else o << pFar->parent()->readName() << "." << pFar->readName();
		if (p->numTokens() > 0) o << "[" << p->numTokens() << "]";
		prefix = "\n\t  ";
	}
	return o << " )";
}

// loop a cluster.
void SDFCluster::loopBy(int fac) {
	pLoop *= fac;
	repetitions.numerator /= fac;
	SDFClustPortIter nextPort(*this);
	SDFClustPort* p;
	while ((p = nextPort++) != 0)
		p->numberTokens *= fac;
}

// generate a name for a new bag
const char* SDFClusterGal::genBagName() {
	char buf[20];
	sprintf (buf, "bag%d", bagNumber++);
	return hashstring(buf);
}

// This method combines two clusters into one.  There are four cases:
// basically the distinction between atomic and non-atomic clusters.

SDFCluster* SDFClusterGal::merge(SDFCluster* c1, SDFCluster* c2) {
	if (logstrm)
		*logstrm << "merging " << *c1 << "\nand " << *c2 << "\n";

// if a bag has a loop factor we treat it as an atom for merging
// purposes (put it inside another bag instead of combining bags).
// That's why we test for the loop count.

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
		bag->absorb(c1,this);
		bag->absorb(c2,this);
		addBlock(bag->setBlock(genBagName(),this));
		c1Bag = bag;
	}
	if (logstrm)
		*logstrm << "result is " << *c1Bag << "\n\n";
	return c1Bag;
}

// This function absorbs an atomic cluster into a bag.

void SDFClusterBag::absorb(SDFCluster* c,Galaxy* par) {
	if (gal.numberBlocks() == 0)
		repetitions = c->repetitions;
// move c from its current galaxy to my galaxy.
	par->removeBlock(*c);
	gal.addBlock(*c,c->readName());
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
SDFClusterBag::merge(SDFClusterBag* b,Galaxy* par) {
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
	while ((c = (SDFCluster*)nextC++) != 0) {
		gal.addBlock(*c,c->readName());
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

// generate the bag's schedule.  Also make schedules for "the bags within".
int SDFClusterBag::genSched() {
	SDFClusterBagIter nextClust(*this);
	SDFCluster* c;
	while ((c = nextClust++) != 0)
		if (!c->genSched()) return FALSE;
	return sched.setup(gal);
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
	StringList sch;
	if (loop() > 1) {
		sch += tab(depth);
		sch += loop();
		sch += "*{\n";
		depth++;
	}
	sch += sched.displaySchedule(depth);
	if (loop() > 1) {
		depth--;
		sch += tab(depth);
		sch += "}\n";
	}
	return sch;
}

// run the cluster, taking into account the loop factor
void SDFClusterBag::go() {
	sched.setStopTime(loop()+exCount);
	sched.run(gal);
	exCount += loop();
}

// destroy the bag.
SDFClusterBag::~SDFClusterBag() {
	if (!owner) return;
	// delete my ports
	BlockPortIter nextP(*this);
	PortHole* p;
	while ((p = nextP++) != 0) {
		p->setNameParent("",0); // avoid p removing itself from list
		LOG_DEL; delete p;
	}
	// delete my blocks
	GalTopBlockIter nextB(gal);
	Block* b;
	while ((b = nextB++) != 0) {
		LOG_DEL; delete b;
	}
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

		int ok = (p->numTokens() == 0 && myIO == peerIO);
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
		*p++ = (*cname == '.') ? '_' : *cname;
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

void SDFAtomCluster::go() {
	for (int i = 0; i < loop(); i++)
		pStar.fire();
}

int SDFAtomCluster::myExecTime() {
	return pLoop * pStar.myExecTime();
}

// constructor for SDFClustPort, port for use in cluster.
// if bp is set it's a "bag port" belonging to an SDFClusterBag.
SDFClustPort::SDFClustPort(SDFPortHole& port,SDFCluster* parent, int bp)
: pPort(port), bagPortFlag(bp)
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

// generate subschedules
	if (!cgal->genSubScheds()) {
		invalid = TRUE; return FALSE;
	}

// generate top-level schedule
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
StringList SDFClustSched::compileRun() {
	StringList code;
	Target& target = getTarget();
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*)next++) != 0) {
		code += c->genCode(target,1);
	}
	return code;
}

StringList SDFAtomCluster::genCode(Target& t, int depth) {
	if (loop() > 1) {
		StringList out = t.beginIteration(loop(), depth);
		out += t.writeFiring(real(), depth+1);
		out += t.endIteration(loop(), depth);
		return out;
	}
	else return t.writeFiring(real(), depth);
}

StringList SDFClusterBag::genCode(Target& t, int depth) {
	StringList out;
	if (loop() > 1) {
		out += t.beginIteration(loop(), depth);
		depth++;
	}
	out += sched.genCode(t, depth);
	if (loop() > 1) {
		depth--;
		out += t.endIteration(loop(), depth);
	}
	return out;
}

StringList SDFBagScheduler::genCode(Target& t, int depth) {
	StringList out;
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*)next++) != 0) {
		out += c->genCode(t, depth);
	}
	return out;
}


