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

// Initialize function sets <code>numberTokens</code> and pointer to the Geodesic.
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

/****

This updates ports after clustering operations typically.

@Description
It will re-compute the numbers produced or consumed based on the far side
clusters <code>loopFac</code> (i.e, its repetitions).

@SideEffects
<code>numberTokens</code> is updated.

****/
void SynDFClusterPort :: update()
{
    int farClustLoopFac, myClustLoopFac;
    ClusterPort :: update();
    if (farSidePort && farSidePort->parent()) {
	farClustLoopFac = ((SynDFCluster*)farSidePort->parent())->loopFactor();
	myClustLoopFac = ((SynDFCluster*)parent())->loopFactor();
	if (myClustLoopFac == 0) {
	    StringList message;
	    message << "This SynDFCluster has a loopFac of 0.  This could";
	    message << "mean that it does not contain a valid DataFlowStar";
	    message << "inside.";
	    Error::abortRun(parent(), message);
	    return;
	}
	numberTokens = farSidePort->numXfer() * farClustLoopFac/myClustLoopFac;
    }
}

/****
Set the <code>tnob</code> private member of <code>SynDFCluster</code>.

@Description
<code>Galaxy::numberBlocks()</code> returns the number of clusters in
<code>*this</code>.  Sometimes
we would like to know the TOTAL number of atomic actors that are
there in the hierarchy, not just the top-level number of clusters.
Hence, this function goes through and computes this quantity.  Ideally,
(FIXME) we should make <code>Galaxy::addBlock</code> a virtual function
and redefine that to keep track of this number rather than computing
it this way.
For now, until we feel bored enough to want to recompile all of Ptolemy
again, we do it this way.
<p>
WARNING: THIS FUNCTION SHOULD ONLY BE CALLED AFTER ALL CLUSTERING
OPERATIONS HAVE BEEN DONE SINCE OTHERWISE THE NUMBER WILL HAVE TO BE
RECOMPUTED.
<p>
Also, <code>SynDFCluster::totalNumberOfBlocks</code> calls this function
if <code>tnob</code> < 0.  Hence,
if <code>totalNumberOfBlocks</code> is called and the cluster is modified
later, this function should be called explicitly to set tnob again.
<p>
In general, the Cluster methods <code>absorb, merge, group</code> will change
this number and hence it should be recomputed after any of those
operations.

****/
void SynDFCluster::setTotalNumberOfBlocks()
{
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

/****
Calls <code>Cluster::convertStar</code> and sets the <code>loopFac</code> for the cluster.

****/
Cluster* SynDFCluster::convertStar(Star& s)
{
    SynDFCluster* a = (SynDFCluster*)Cluster::convertStar(s);
    a->setLoopFac(((DataFlowStar&)s).reps());
    return a;
}

/****
Again, just updates <code>loopFac, tnob</code> after <code>Cluster::absorb</code>.

****/
int SynDFCluster::absorb(Cluster& c, int removeFlag)
{
    setLoopFac(gcd(loopFactor(), ((SynDFCluster&)c).loopFactor()));
    // attempt to keep tnob consistent if it is defined for this
    // cluster already.  If not, do nothing; let it be computed
    // when something else requires it.
    if (tnob != -1) tnob += ((SynDFCluster&)c).totalNumberOfBlocks();
    return Cluster::absorb(c, removeFlag);
}

/****
Again, just updates <code>loopFac, tnob</code> after <code>Cluster::absorb</code>.

****/
int SynDFCluster::merge(Cluster& c, int removeFlag)
{
    setLoopFac(gcd(loopFactor(), ((SynDFCluster&)c).loopFactor()));
    // attempt to keep tnob consistent if it is defined for this
    // cluster already.  If not, do nothing; let it be computed
    // when something else requires it.
    if (tnob != -1) tnob += ((SynDFCluster&)c).totalNumberOfBlocks();
    return Cluster::merge(c, removeFlag);
}

// constructor
SynDFCluster :: SynDFCluster() : loopFac(0), tnob(-1) , Cluster() {}

/****

TNSE = Total Number of Samples Exchanged in a complete period of an SDF schedule.

****/
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


