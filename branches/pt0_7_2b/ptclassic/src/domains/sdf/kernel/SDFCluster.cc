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
#include "streamCompat.h"

// A SDFClusterGal is a Galaxy, built from another galaxy.

ostream& operator<< (ostream& o, SDFClusterGal& g) {
	SDFClusterGalIter next(g);
	SDFCluster* c;
	while ((c = next++) != 0) {
		o << *c << "\n";
	}
}

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
	if (logstrm) {
		*logstrm << "Initial galaxy:\n" << *this;
	}
}

int SDFClusterGal::cluster() {
	int urate, changes = FALSE;
	while ((urate = uniformRate()) == FALSE) {
		if (!mergePass()) break;
		changes = TRUE;
		if (!loopPass()) break;
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
	SDFCluster *c1, *c2;
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
	} while (c2);
	if (logstrm) {
		if (changes)
			*logstrm << "After merge pass:\n" << *this;
		else *logstrm << "Merge pass made no changes\n";
	}
	return changes;
}

int SDFClusterGal::loopPass() {
	if (numberClusts() <= 1) return FALSE;
	int changes = 0;
	SDFClusterGalIter nextClust(*this);
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
	if (logstrm) {
		if (changes) *logstrm << "After loop pass:\n" << *this;
		else *logstrm << "Loop pass made no changes\n";
	}
	return changes;
}

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

void SDFCluster::loopBy(int fac) {
	pLoop *= fac;
	repetitions.numerator /= fac;
}

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
		if (pFar == 0) {
			// external port: this is now a "grandparent pointer".
			LOG_NEW; SDFClustPort *gp = new SDFClustPort(*cp,this,1);
			SDFClustPort* alias = cp->outPtr();
			gp->makeExternLink(alias);
			cp->makeExternLink(gp);
			addPort(*gp);
		}
		else if (pFar->parent() == this) {
			// the far side of this guy is one of my bag pointers.
			// zap it and connect directly.
			SDFClustPort* p = pFar->inPtr();
			int del = cp->numTokens();
			LOG_DEL; delete pFar;
			cp->connect(*p, del);
			cp->initGeo();
		}
		else {
			LOG_NEW; SDFClustPort *np = new SDFClustPort(*cp,this,1);
			cp->makeExternLink(np);
			addPort(*np);
		}
	}
}

void
SDFClusterBag::merge(SDFClusterBag* b,Galaxy* par) {
	// get a list of all "bagports" that connect the two clusters.
	SDFClustPortIter nextbp(*this);
	SDFClustPort* p;
	SequentialList zap;
	while ((p = nextbp++) != 0)
		if (p->far()->parent() == b) zap.put(p);
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
	// now we simply combine the bagports and clusters into this.
	SDFClusterBagIter nextC(*b);
	SDFCluster* c;
	while ((c = (SDFCluster*)nextC++) != 0) {
		gal.addBlock(*c,c->readName());
	}
	SDFClustPortIter nextP(*b);
	while ((p = nextP++) != 0) {
		addPort(p->setPort(p->readName(),this));
	}
// get rid of b.
	par->removeBlock(*b); // remove from parent galaxy
	b->owner = FALSE;     // prevent from zapping contents, I took them
	LOG_DEL; delete b;    // zap the shell
}

ostream& SDFClusterBag::printOn(ostream& o) {
	printBrief(o);
	o << "= {";
	SDFClusterBagIter next(*this);
	SDFCluster* c;
	while (( c = next++) != 0) {
		o << " ";
		c->printBrief(o);
	}
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

StringList SDFClusterBag::displaySchedule() {
	StringList sch;
	if (loop() > 1) {
		sch += loop();
		sch += "*{ ";
	}
	sch += sched.displaySchedule();
	if (loop() > 1) sch += " }";
	return sch;
}

// run the cluster, taking into account the loop factor
void SDFClusterBag::go() {
	sched.setStopTime(loop()+exCount);
	sched.run(gal);
	exCount += loop();
}

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

SDFCluster* SDFCluster::mergeCandidate() {
// find an attractive and compatible neighbor.
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


SDFAtomCluster::SDFAtomCluster(SDFStar& star,Galaxy* parent) : pStar(star)
{
	SDFStarPortIter nextPort(star);
	SDFPortHole* p;
	while ((p = nextPort++) != 0) {
		// do not make a port in the cluster if it is a "loopback" port
		if (p->far()->parent() == &star) continue;
		LOG_NEW; SDFClustPort *cp = new SDFClustPort(*p,this);
		addPort(*cp);
	}
	setNameParent(mungeName(star),parent);
	repetitions = pStar.repetitions;
}

SDFAtomCluster::~SDFAtomCluster() {
	BlockPortIter nextPort(*this);
	PortHole* p;
	while ((p = nextPort++) != 0) {
		// avoid removal from parent list.
		p->setNameParent("",0);
		LOG_DEL; delete p;
	}
}

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

StringList SDFAtomCluster::displaySchedule() {
	StringList sch;
	if (loop() > 1) {
		sch += loop();
		sch += "*";
	}
	sch += real().readFullName();
	return sch;
}

void SDFAtomCluster::go() {
	for (int i = 0; i < loop(); i++)
		pStar.fire();
}

int SDFAtomCluster::myExecTime() {
	return pLoop * pStar.myExecTime();
}

SDFClustPort::SDFClustPort(SDFPortHole& port,SDFCluster* parent, int bp)
: pPort(port), bagPortFlag(bp)
{
	const char* name = bp ? port.readName() : mungeName(port);
	setPort(name,parent,INT);
	myPlasma = Plasma::getPlasma(INT);
}

void SDFClustPort::initGeo() { myGeodesic->initialize();}

// This method is called on a cluster porthole to create a connection
// with a "bag port" in the parent ClusterBag.  

void SDFClustPort::makeExternLink(SDFClustPort* bagPort) {
	pOutPtr = bagPort;
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


// return the number of tokens read or written by the port, taking into
// account any looping.
int SDFClustPort::numIO() {
	SDFClustPort* in = inPtr();
	int n = in ? in->numIO() : pPort.numberTokens;
	return n * parentClust()->loop();
}

SDFClustSched::~SDFClustSched() { LOG_DEL; delete cgal;}

int SDFClustSched::computeSchedule (Galaxy& gal) {
	LOG_DEL; delete cgal;
	const char* file = logFile;
	ostream* logstrm = 0;
	if (file && *file) {
		int fd = creat(expandPathName(file), 0666);
		if (fd < 0)
			Error::warn(gal, "Can't open log file ",file);
		else logstrm = new ofstream(fd);
	}
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

StringList SDFClustSched::displaySchedule() {
	StringList sch;
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	while ((c = (SDFCluster*)next++) != 0) {
		sch += c->displaySchedule();
		sch += "\n";
	}
	return sch;
}

StringList SDFBagScheduler::displaySchedule() {
	StringList sch;
	SDFSchedIter next(mySchedule);
	SDFCluster* c;
	const char* sep = "";
	while ((c = (SDFCluster*)next++) != 0) {
		sch += sep;
		sch += c->displaySchedule();
		sep = " ";
	}
	return sch;
}
