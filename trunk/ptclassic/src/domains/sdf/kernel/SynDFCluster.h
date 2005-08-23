#ifndef _SynDFCluster_h
#define _SynDFCluster_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Cluster.h"


/******************************************************************
Version identification:
@(#)SynDFCluster.h	1.8	11/13/97

Copyright (c) 1996-1997 The Regents of the University of California.
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

*******************************************************************/

/****

SynDFClusterPort is the SDF version of ClusterPort

@Description
This class maintains the number of tokens transferred on the ports.

@Author Praveen K. Murthy

****/
class SynDFClusterPort : public ClusterPort {
public:

	SynDFClusterPort(PortHole& p) : ClusterPort(p) {}

	// Set the loop factor.
	/* virtual */ void initializeClusterPort();

	// called after Absorb/merge operations.  Takes care of aliases
	// , loopFac, and tnob if possible.
	/* virtual */ void update();
	void updateNumxfer();
};

/**** 
Iterator class for SynDFClusterPorts; steps through each port in a Block.

@Author Praveen K. Murthy

****/
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

/****
<code>SynDFCLuster</code> is the SDF version of <code>Cluster</code>; analogous to what <code>SDFStar</code> is to <code>Star</code>.

@Description
We call it SynDFCluster because there is already an SDFCluster class here.
However, the SDFCluster class suffers from being poorly written and
being fairly un-modular; hence, the alternate hierarchy starting from Cluster
has been established.  Unlike SDFCluster, the methods in SynDFCluster do
not contain any scheduler-specific functions; they merely contain very
generic functions to absorb and group clusters together.  SynDFCluster is
a fairly lightweight class, as is Cluster.
<p>
Mainly, SynDFCluster assumes that the atomic blocks inside the clusters
are SDFStars;hence, they produce and consume fixed numbers of tokens per
firing.
<p>
It maintains a <code>loopFac</code> parameter that represents the gcd
of all of the repetitions of the SynDFCLusters inside itself.

@Author Praveen K. Murthy

****/
class SynDFCluster: public Cluster {
private:

        // maintains the gcd of all the loopFacs of constituent clusters
        int loopFac;

	// tnob = total number of blocks in cluster (at the lowest levels)
	int tnob;

public:
	// Constructor
	SynDFCluster();

	// make a new SynDFCluster
	/* virtual */ Block* makeNew() const { return new SynDFCluster; }

	// make a new SynDFClusterPort
	inline /* virtual */ ClusterPort* makeNewPort(PortHole& p) {
	    return new SynDFClusterPort(p);
	}

	// Convert a Star to a SynDFCluster.
	/* virtual */ Cluster* convertStar(Star&);

	// Absorb c into *this.  c is kept intact; hence, the number 
	// of blocks in *this goes up by 1.
	/* virtual */ int absorb(Cluster& c, int removeFlag=TRUE);

	// Merge the contents of c into *this.  Hence, the number
	// of blocks in *this goes up by the number of blocks in c.
	/* virtual */ int merge(Cluster& c, int removeFlag=TRUE);

	// return tnob; compute it and return it if not set yet.
	inline int totalNumberOfBlocks() {
	    if (tnob < 0) setTotalNumberOfBlocks();
	    return tnob;
	}

	inline void settnob(int i) {tnob = i;}

	// compute and set tnob.
	void setTotalNumberOfBlocks();

	// Compute the Total Number of Samples Exchanged on c1 ---> c2
	int computeTNSE(SynDFCluster* c1, SynDFCluster* c2, SynDFClusterPort* a);

	// return the loop factor.
	int loopFactor() { return loopFac;}

	// Set the loop factor.
	void setLoopFac(int r) {loopFac = r;}

	// return the number of input ports this cluster has.
	int numInputs();

	// return the number of output ports this cluster has.
	int numOutputs();
};

/****
Iterator class for SynDFClusters; steps through each SynDFCluster in a Galaxy.

@Author Praveen K. Murthy

****/
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

/****
Iterator class that returns the SynDFClusterPorts that are outputs of a SynDFCluster.

@Author Praveen K. Murthy

****/
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

/****
Iterator class that returns the SynDFClusterPorts that are inputs of a particular SynDFCluster.

@Author Praveen K. Murthy

****/
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
