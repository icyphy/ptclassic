/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992, 1993 The Regents of the University of California.
                       All Rights Reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY


 Programmer:  J. Buck
 Date of creation: 9/10/92

This file defines a special type of BDF PortHole for use with the
BDFCluster class.

**************************************************************************/
#ifndef _BDFClustPort_h
#define _BDFClustPort_h 1

#ifdef __GNUG__
#pragma interface
#endif

class BDFCluster;

#include "BDFPortHole.h"

const int BCP_ATOM = 0;
const int BCP_BAG = 1;
const int BCP_DUP = 2;
const int BCP_DUP_IN = 3;

class BDFClustPort : public BDFPortHole {
public:
	BDFClustPort(DFPortHole& p,BDFCluster* parent = 0,int bagp = BCP_ATOM);
	~BDFClustPort();

	// return what is inside me
	DFPortHole& real() { return pPort;}

	int isItInput() const { return inFlag;}
	int isItOutput() const { return !inFlag;}

	// set/return the control bit
	int isControl() const { return ctlFlag;}
	void setControl(int val) { ctlFlag = val ? 1 : 0;}

	// am I a self-loop?
	int selfLoop() const { 
		PortHole* p = PortHole::far();
		return p && parent() == p->parent();
	}

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

	int isBagPort() const { return bagPortFlag == BCP_BAG;}
	int isDupPort() const { return bagPortFlag == BCP_DUP;}

	BDFClustPort* inPtr() {
		return (bagPortFlag == BCP_BAG) ? (BDFClustPort*)&pPort : 0;
	}

	// this one just traverses "inPtr" all the way.
	BDFClustPort* innermost();

	// this overload hides the base version and forces arg to
	// be a BDFClustPort.  It also does checking: assoc port must
	// have same parent as me (unless null).
	void setRelation(BDFRelation r, BDFClustPort* assoc = 0);

	// data movement routines (for dynamic execution of clusters)
	void receiveData();
	void sendData();
	void initialize();

	// indicate that there is a dup port.  This requires tokens
	// be recorded for dynamic execution.
	void markDuped() { dupedFlag = 1;}
	int duped() const { return dupedFlag;}

	// return last Boolean value
	int lastBool() const { return lastBoolValue;}

	// functions to set and get maximum arc counts for buffer allocation
	int maxArcCount();
	void setMaxArcCount(int howMany);
	// don't like this being public, but...
	void setNumXfer(int v) { numberTokens = v;}
private:
	// test for need to move tokens up through the cluster hierarchy
	int seeIfMoveupNeeded();

	// the real port
	DFPortHole& pPort;

	// the external link
	BDFClustPort* pOutPtr;

	// true if I am an input
	unsigned char inFlag;
	// true if I am a bag port
	unsigned char bagPortFlag;

	// true if I am a feedforward-only port (after marking)
	unsigned char feedForwardFlag;

	// true if I control some other port
	unsigned char ctlFlag;

	// value of last boolean
	unsigned char lastBoolValue;

	// true if I have a corresponding DUP port
	unsigned char dupedFlag;

	// true if output needs to be moved up
	unsigned char moveupNeeded;
};

// determine whether two signals have a fixed relation or not.  It
// returns one of BDF_SAME, BDF_COMPLEMENT, or BDF_NONE.

BDFRelation sameSignal(BDFClustPort* a1, BDFClustPort* a2);

// determine whether two signals have the same controlling condition.
int condMatch(BDFClustPort* p1, BDFClustPort *p2);

// fn to print a single port of a cluster
ostream& operator<<(ostream& o, BDFClustPort& p);

#endif
