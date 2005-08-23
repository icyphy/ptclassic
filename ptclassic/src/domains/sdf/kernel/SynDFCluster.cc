static const char file_id[] = "SynDFCluster.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  Praveen K. Murthy
 Date of creation: Feb/9/96

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SynDFCluster.h"
#include "Error.h"
#include "DataFlowStar.h"
#include "DFPortHole.h"

void SynDFClusterPort :: initializeClusterPort()
{
    ClusterPort :: initializeClusterPort();
    DFPortHole& insidePort = (DFPortHole&)realPort();
    numberTokens = insidePort.numXfer();
    myGeodesic = insidePort.geo();
    // FIXME: need to take care of maxBackValue also later:
    // int maxBackValue = insidePort.maxDelay();
    // but problem is that we are derived from GalPort and PortHole,
    // and not from DFPortHole.
}

void SynDFClusterPort :: update()
{
    int farClustLoopFac;
    ClusterPort :: update();
    if (farSidePort && farSidePort->parent()) {
	farClustLoopFac = ((SynDFCluster*)farSidePort->parent())->loopFactor();
	numberTokens = farSidePort->numXfer() * farClustLoopFac
	/ ((SynDFCluster*)parent())->loopFactor();
    }
}

void SynDFCluster::setTotalNumberOfBlocks()
{
    // numberBlocks() returns the number of clusters in *this.  Sometimes
    // we would like to know the TOTAL number of atomic actors that are
    // there in the heirarchy, not just the top-level number of clusters.
    // Hence, this function goes through and computes this quantity.  Ideally,
    // (FIXME) we should make Galaxy::addBlock a virtual function and redefine
    // that to keep track of this number rather than computing it this way.
    // For now we do it this way.
    // WARNING: THIS FUNCTION SHOULD ONLY BE CALLED AFTER ALL CLUSTERING
    // OPERATIONS HAVE BEEN DONE SINCE OTHERWISE THE NUMBER WILL HAVE TO BE
    // RECOMPUTED.

    // Also, totalNumberOfBlocks calls this function if tnob < 0.  Hence,
    // if totalNumberOfBlocks is called and the cluster is modified later,
    // this function should be called explicitly to set tnob again.

    // In general, the Cluster methods absorb, merge, and group will change
    // this number and hence it should be recomputed after any of those
    // operations.
    SynDFClusterIter nextClust(*this);
    SynDFCluster *c;
    tnob = 0;

    // we assume that if there is only one cluster inside us, then that
    // cluster is a star.
    if (numberBlocks() == 1) {
	tnob = 1;
    } else {
	while ((c=nextClust++) != NULL)	tnob += c->totalNumberOfBlocks();
    }
}

Cluster* SynDFCluster::convertStar(Star& s)
{
    SynDFCluster* a = (SynDFCluster*)Cluster::convertStar(s);
    a->setLoopFac(((DataFlowStar&)s).reps());
    return a;
}

int SynDFCluster::absorb(Cluster& c, int removeFlag)
{
    setLoopFac(gcd(loopFactor(), ((SynDFCluster&)c).loopFactor()));
    // attempt to keep tnob consistent if it is defined for this
    // cluster already.  If not, do nothing; let it be computed
    // when something else requires it.
    if (tnob != -1) tnob += ((SynDFCluster&)c).totalNumberOfBlocks();
    return Cluster::absorb(c, removeFlag);
}

int SynDFCluster::merge(Cluster& c, int removeFlag)
{
    setLoopFac(gcd(loopFactor(), ((SynDFCluster&)c).loopFactor()));
    // attempt to keep tnob consistent if it is defined for this
    // cluster already.  If not, do nothing; let it be computed
    // when something else requires it.
    if (tnob != -1) tnob += ((SynDFCluster&)c).totalNumberOfBlocks();
    return Cluster::merge(c, removeFlag);
}

SynDFCluster :: SynDFCluster() : loopFac(0), tnob(-1) {} // constructor

int SynDFCluster::computeTNSE(SynDFCluster* c1, SynDFCluster* c2, SynDFClusterPort* a)
{
    // Compute the TNSE+numInitDelays for the arc between c1 and c2

    int numP, numC, reps;
    numP = a->numXfer();
    numC = a->far()->numXfer();
    reps = c1->loopFactor();
    if (reps*numP != numC*c2->loopFactor()) {
	Error::abortRun("Balance equations seem to be violated in SynDFCluster");
	return -1;
    }
    return reps*numP + a->numInitDelays();
}


