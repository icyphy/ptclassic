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
#ifndef _BDFClustPort_h
#define _BDFClustPort_h 1

#ifdef __GNUG__
#pragma interface
#endif

class BDFCluster;

#include "BDFConnect.h"

class BDFClustPort : public BDFPortHole {
private:
	// the real port
	DFPortHole& pPort;

	// the external link
	BDFClustPort* pOutPtr;

	// true if I am a bag port
	unsigned char bagPortFlag;

	// true if I am a feedforward-only port (after marking)
	unsigned char feedForwardFlag;

	// true if I control some other port
	unsigned char ctlFlag;
public:
	BDFClustPort(DFPortHole& p,BDFCluster* parent = 0,int bagp = 0);
	~BDFClustPort() {}

	// return what is inside me
	DFPortHole& real() { return pPort;}

	// these are passthrough functions
	int isItInput() const { return pPort.isItInput();}
	int isItOutput() const { return pPort.isItOutput();}

	// set/return the control bit
	int isControl() const { return ctlFlag;}
	void setControl(int val) { ctlFlag = val ? 1 : 0;}

	// my assocPort is guaranteed to be a BDFClustPort so this
	// cast is safe.
	BDFClustPort* assoc() { return (BDFClustPort*)BDFPortHole::assoc();}

	void initGeo();
	BDFCluster* parentClust() { return (BDFCluster*)parent();}
	BDFClustPort* far() { return (BDFClustPort*)PortHole::far();}
	int numIO() const { return numberTokens;}
	BDFClustPort* outPtr() {
		return far() ? 0 : pOutPtr;
	}

	int feedForward() const { return feedForwardFlag;}
	void markFeedForward() { feedForwardFlag = 1;}

	// return TRUE if there is no sample rate change between me
	// and who I am connected to
	int sameRate();

	// return TRUE if there is delay on the arc that may be a
	// problem for merging, and FALSE otherwise.
	int fbDelay() const { return (numTokens() > 0 && !feedForward());}

	// return the real far port aliased to bagPorts.
	// If the bagPort is a port of the outsideCluster, return zero.
	BDFClustPort* realFarPort(BDFCluster* outsideCluster);

	void makeExternLink(BDFClustPort* val);

	int isBagPort() const { return bagPortFlag;}

	BDFClustPort* inPtr() {
		return bagPortFlag ? (BDFClustPort*)&pPort : 0;
	}

	// this one just traverses "inPtr" all the way.
	BDFClustPort* innermost();

	// this overload hides the base version and forces arg to
	// be a BDFClustPort.
	void setRelation(BDFRelation r, BDFClustPort* assoc = 0) {
		BDFPortHole::setRelation(r,assoc);
	}
};

// determine whether two signals have a fixed relation or not.  It
// returns one of BDF_SAME, BDF_COMPLEMENT, or BDF_NONE.

BDFRelation sameSignal(BDFClustPort* a1, BDFClustPort* a2);

// determine whether two signals have the same controlling condition.
int condMatch(BDFClustPort* p1, BDFClustPort *p2);

// fn to print a single port of a cluster
ostream& operator<<(ostream& o, BDFClustPort& p);

// special iterator that steps through ports related by BDF_SAME or
// BDF_COMPLEMENT arcs to the original arc.  The interface is different
// because as we return each port, we return the relation of that port
// to the original port (BDF_SAME or BDF_COMPLEMENT).

class BDFClustPortRelIter {
public:
	BDFClustPortRelIter(BDFClustPort& p);
	BDFClustPort* next(BDFRelation&);
	void reset();
	int debug() const { return count < 0;}
	void setDebug() { count = -1;}
private:
	BDFClustPort* start;
	BDFClustPort* pos;
	// only a bit is needed for the following flags
	unsigned char justDidFar;
	unsigned char rev;
	// for error checking
	short count;
};

#endif
