#ifndef _SynDFCluster_h
#define _SynDFCluster_h 1
#ifdef __GNUG__
#pragma implementation
#endif

#include "Cluster.h"


/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  Praveen K. Murthy
 Date of creation: Feb/9/96

SynDFCLuster is analogous to the SDFStar class.
We call it SynDFCluster because there is already an SDFCluster class here.
However, the SDFCluster class suffers from being poorly written and
being fairly un-modular; hence, the alternate hierarchy starting from Cluster
has been established.  Unlike SDFCluster, the methods in SynDFCluster do
not contain any scheduler-specific functions; they merely contain very
generic functions to absorb and group clusters together.  SynDFCluster is
a fairly lightweight class, as is Cluster.

Mainly, SynDFCluster assumes that the atomic blocks inside the clusters
are SDFStars;hence, they produce and consume fixed numbers of tokens per firing.
It maintains a loopFac parameter that represents the gcd of all of the
repetitions of the SDFCLusters inside itself.



*******************************************************************/

class SynDFClusterPort : public ClusterPort {
public:
	SynDFClusterPort(GenericPort& gp) : ClusterPort(gp) {};
	/* virtual */ void initializeClusterPort();
	/* virtual */ void update();
};

class SynDFClusterPortIter : private ClusterPortIter {
public:
    inline SynDFClusterPortIter(Block& b): ClusterPortIter(b) {};
    inline SynDFClusterPort* next() {
	return (SynDFClusterPort*)ClusterPortIter::next();
    }
    inline SynDFClusterPort* operator++(POSTFIX_OP) { return next(); }
    ClusterPortIter::reset;
    ClusterPortIter::remove;
};


class SequentialList;

class SynDFCluster: public Cluster {
private:
        // maintains the gcd of all the loopFacs of constituent clusters
        int loopFac;
	// tnob = total number of blocks in cluster (at the lowest levels)
	int tnob;
public:
	SynDFCluster();

	/* virtual */ Block* makeNew() const { return new SynDFCluster; }
	inline virtual SynDFClusterPort* makeNewPort(GenericPort& p) {
	    return new SynDFClusterPort(p);
	}
	/* virtual */ Cluster* convertStar(Star&);
	/* virtual */ int absorb(Cluster& c, int removeFlag=TRUE);
	/* virtual */ int merge(Cluster& c, int removeFlag=TRUE);
	inline int totalNumberOfBlocks() {
	    if (tnob < 0) setTotalNumberOfBlocks();
	    return tnob;
	}
	inline void settnob(int i) {tnob = i;}
	void setTotalNumberOfBlocks();
	int computeTNSE(SynDFCluster* c1, SynDFCluster* c2, SynDFClusterPort* a);
	int loopFactor() { return loopFac;}
	void setLoopFac(int r) {loopFac = r;}
};

class SynDFClusterIter : private ClusterIter {
public:
	inline SynDFClusterIter(Galaxy& g):ClusterIter(g) {};
	inline SynDFCluster* next() { return (SynDFCluster*)(ClusterIter::next());}
	inline SynDFCluster* operator++(POSTFIX_OP) { return next(); }
	SynDFCluster* next(int flagValue, int index) {
	    return (SynDFCluster*)(ClusterIter::next(flagValue, index));
	}
	ClusterIter::reset;
	ClusterIter::remove;
};

class SynDFClusterOutputIter : private ClusterOutputIter {
public:
	inline SynDFClusterOutputIter(Block& b):ClusterOutputIter(b) {};
	inline SynDFClusterPort* next() {
	    return (SynDFClusterPort*)ClusterOutputIter::next();
	}
	inline SynDFClusterPort* next(int flag_loc, int flag_val) {
	    return (SynDFClusterPort*)ClusterOutputIter::next(flag_loc,flag_val);
	}
	inline SynDFClusterPort* operator++(POSTFIX_OP) {
	    return SynDFClusterOutputIter::next();
	}
	ClusterOutputIter::reset;
	ClusterOutputIter::remove;
};

class SynDFClusterInputIter : private ClusterInputIter {
public:
	inline SynDFClusterInputIter(Block& b):ClusterInputIter(b) {};
	inline SynDFClusterPort* next() {
	    return (SynDFClusterPort*)ClusterInputIter::next();
	}
	inline SynDFClusterPort* next(int flag_loc, int flag_val) {
	    return (SynDFClusterPort*)ClusterInputIter::next(flag_loc,flag_val);
	}
	inline SynDFClusterPort* operator++(POSTFIX_OP) {
	    return SynDFClusterInputIter::next();
	}
	ClusterInputIter::reset;
	ClusterInputIter::remove;
};


#endif
