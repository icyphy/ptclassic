/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 9/10/92

This file defines a special type of BDF PortHole for use with the
BDFCluster class.

There is also a special iterator that finds all BDFClustPorts that
have a SAME or COMPLEMENT relationship to the original.

**************************************************************************/
static const char file_id[] = "BDFClustPort.cc";

#include "BDFClustPort.h"
#include "BDFCluster.h"
#include "Plasma.h"
#include "Geodesic.h"
#include <iostream.h>

const int UNKNOWN = 2;		// a value other than TRUE/FALSE.

// constructor for BDFClustPort, port for use in cluster.
// if bp is set it's a "bag port" belonging to an BDFClusterBag.
BDFClustPort::BDFClustPort(DFPortHole& port,BDFCluster* parent, int bp)
: pPort(port), bagPortFlag(bp), pOutPtr(0), feedForwardFlag(0),
  ctlFlag(0), lastBoolValue(2), dupedFlag(0), moveupNeeded(UNKNOWN)
{
	const char* name = bp ? port.name() : BDFCluster::mungeName(port);
	setPort(name,parent,INT);
	myPlasma = Plasma::getPlasma(INT);
	numberTokens = port.numXfer();
	// a DUP_IN port is always an input.  a DUP is always an output.
	if (bp == BCP_DUP)
		inFlag = FALSE;
	else if (bp == BCP_DUP_IN) {
		inFlag = TRUE;
		bagPortFlag = BCP_DUP;
	}
	else inFlag = port.isItInput();
}

// destructor
BDFClustPort::~BDFClustPort() {
	if (isBagPort())
		inPtr()->pOutPtr = 0;
	if (pOutPtr) {
		pOutPtr = 0;
	}
}

void BDFClustPort::initGeo() { myGeodesic->initialize();}

void BDFClustPort::initialize() {
	bufferSize = numberTokens;
	maxBackValue = numberTokens - 1;
	BDFPortHole::initialize();
}

// return true if port has same rate as its neighbor.
int BDFClustPort::sameRate() {
	BDFClustPort* pFar = far();
	return (pFar && numIO() == pFar->numIO() && condMatch(this,pFar));
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

// methods for BDFClustPortRelIter.  This iterator steps through
// all ports that are "the same" or "the complement" of the original
// port.  The far() port is included in this iteration if there is
// no delay and neither side is a conditional port.  Q: should we
// generalize this to admit conditional ports controlled by the same
// conditions?

BDFClustPortRelIter::BDFClustPortRelIter(BDFClustPort& p)
: start(&p), pos(&p), justDidFar(0), rev(0), count(0) {}

void BDFClustPortRelIter::reset() {
	pos = start;
	justDidFar = rev = 0;
	count = 0;
}

const int DEBUG_LIMIT = 30;

BDFClustPort* BDFClustPortRelIter :: next(BDFRelation& rel) {
	if (pos == 0) return 0;
	// error check
	if (!debug()) count++;
	if (count > DEBUG_LIMIT) {
		BDFRelation r;
		reset();
		setDebug();
		cerr << "BDFClustPortRelIter error: starting at "
		     << start->fullName() << ":\n";
		for (int i = 0; i < DEBUG_LIMIT; i++)
			next(r);
		return 0;
	}
	BDFRelation posrel = pos->relType();
	BDFClustPort* pfar = pos->far();
	// we see if we should jump over to the farSidePort.
	// it must be connected, have the same rate and no delay.
	// the "noFar" flag avoids crossing the same arc twice in
	// a row.
	if (!justDidFar && pfar && pos->numTokens() == 0 &&
	    pos->numXfer() == pfar->numXfer() &&
	    !TorF(posrel) && !TorF(pfar->relType())) {
		pos = pos->far();
		justDidFar = 1;
		rel = BDF_SAME;
		if (pos == start) return 0;
		if (debug())
			cerr << pos->fullName() << ":S\n";
		return pos;
	}
	// if we hopped over but there are no links on this side,
	// hop back and proceed from there.
	if (justDidFar && !SorC(posrel)) {
		pos = pfar;
		posrel = pfar->relType();
	}
	if (!SorC(posrel)) pos = 0;
	else {
		if (posrel == BDF_COMPLEMENT) rev = !rev;
		pos = pos->assoc();
		rel = (rev ? BDF_COMPLEMENT : BDF_SAME);
		justDidFar = 0;
	}
	if (pos == start) return 0;
	else {
		if (debug())
			cerr << pos->fullName() <<
				(rel == BDF_SAME ? ":S\n" : "C\n");
		return pos;
	}
}

// get the control port.  We don\'t care here about "same data" relations.
BDFClustPort* controlPort(BDFClustPort* p,BDFRelation& relation) {
	BDFClustPort* a = p->assoc();
	relation = p->relType();
	if (!TorF(relation)) {
		relation = BDF_NONE;
		a = 0;
	}
	return a;
}

// determine whether two signals have a fixed relation or not.  It
// returns one of BDF_SAME, BDF_COMPLEMENT, or BDF_NONE.

BDFRelation sameSignal(BDFClustPort* a1, BDFClustPort* a2)
{
	if (a1 == a2 || &a1->real() == &a2->real()) return BDF_SAME;
	BDFRelation rel;
	BDFClustPortRelIter iter(*a1);
	BDFClustPort* p;
	while ((p = iter.next(rel)) != 0) {
		if (p == a2) return rel;
	}
	return BDF_NONE;
}

// determine whether two ports have matching conditions
int condMatch(BDFClustPort* p1, BDFClustPort *p2) {
	BDFRelation r1, r2;

	BDFClustPort* a1 = controlPort(p1,r1);
	BDFClustPort* a2 = controlPort(p2,r2);
	if (r1 == r2) {
		return (r1 == BDF_NONE) || sameSignal(a1,a2) == BDF_SAME;
	}
	else if (r1 == BDF_NONE || r2 == BDF_NONE) return FALSE;
	else {
		// opposite conditions -- but two complements cancel out
		return sameSignal(a1,a2) == BDF_COMPLEMENT;
	}
}

static const char* labels[] = { "<ifFalse:", "<ifTrue:", "<Same:", "<Compl:" };

// fn to print a single port of a cluster
ostream& operator<<(ostream& o, BDFClustPort& p) {
	if (p.selfLoop()) o << "SelfLoop:";

	o << p.parent()->name() << "." << p.name();

	// print BDF control information
	if (p.isControl()) o << "[C]";
	BDFRelation r = p.relType();
	BDFClustPort* a = p.assoc();
	if (a && r != BDF_NONE) o << labels[r] << a->name() << ">";

	// print connectivity
	BDFClustPort* pFar = p.far();
	if (p.isItOutput())
		o << "=>";
	else
		o << "<=";
	if (!pFar) o << "0";
	else o << pFar->parent()->name() << "." << pFar->name();
	if (pFar && p.numTokens() > 0) o << "[" << p.numTokens() << "]";
	return o;
}

// copy data between ports.
static void copy(PortHole* src,PortHole* dst) {
	int n = src->numXfer();
	for (int i = n-1; i>=0; i--) {
		(*dst)%i = (*src)%i;
	}
}

// routine to copy data to duplicate ports.  The dup ports form a ring
// with the real ports and any other real ports that are constrained to
// be the same.
static void broadcastDupData(BDFClustPort* src) {
	for (BDFClustPort* dst = src->assoc();
	     dst && dst != src;
	     dst = dst->assoc()
	    ) {
		if (dst->isDupPort()) {
			copy(src,dst);
		}
	}
}

// routines for execution of the ports.
void BDFClustPort::receiveData() {
	if (!inFlag) return;	// do nothing for output ports.
	if (far() == 0) {
		// we are an external port.  Obtain data by "copying down"
		// if we are duped or are a control port.
		if (duped() || isControl()) {
			copy(pOutPtr,this);
			// rely on dup ports being processed after real ones.
			if (duped()) broadcastDupData(this);
		}
	}
	else getParticle();
}

// see if signal is needed as ctl signal.
static int seeIfMoveupNeeded(BDFClustPort* me) {
	while (1) {
		BDFClustPort* o = me->outPtr();
		if (!o) break;
		me = o;
	}
	BDFClustPort* mef = me->far();
	return (mef->duped() || mef->isControl());
}

void BDFClustPort::sendData() {
	// for inputs, sendData clears old particles.
	if (inFlag) {
		clearParticle();
		return;
	}
	if (moveupNeeded == UNKNOWN)
		moveupNeeded = seeIfMoveupNeeded(this);
	// for non-bags, need to get data previously sent by geodesics.
	if (moveupNeeded && bagPortFlag == BCP_ATOM) {
		Geodesic* g = real().geo();
		if (numberTokens == 1) {
			// convert token to 0 or 1 (booleanize)
			int tokval = ( int(*(g->tail())) != 0 );
			(*this)%0 << tokval;
		}
		else {
			cerr << real().fullName()
			     << ": Cannot moveup > 1 yet\n";
			exit(1);
		}
	}
	// if I am duped, need to send particles to duplicate outputs.
	if (duped())
		broadcastDupData(this);
	// Note after putParticle, data have been cleared.
	if (far()) putParticle();
	else if (moveupNeeded) {
		copy(this,pOutPtr);
	}
}

