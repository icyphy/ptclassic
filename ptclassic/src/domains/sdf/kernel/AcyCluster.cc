static const char file_id[] = "AcyCluster.cc";
/******************************************************************
Version identification:
@(#)AcyCluster.cc	1.9	08/12/96

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

  Programmer: Praveen K. Murthy

Acyclic cluster to implement graph cutting routines needed by the
RPMC algorithm in AcyLoopScheduler.  For details of this graph cutting
heuristic, see chapter 6 of

	"Software synthesis from dataflow graphs", by

Shuvra S. Bhattacharyya, Praveen K. Murthy, and Edward A. Lee,
Kluwer Academic Publishers, Norwood, MA, 1996

******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "AcyCluster.h"
#include <limits.h>

#define NODE_NUM_FLAG_LOC 0
#define TMP_PARTITION_FLAG_LOC 1
#define PARTITION_FLAG_LOC 2

// The following macros are used for Blocks

// PARTITION is used to hold TMP_PARTITION from the best cut
#define PARTITION(pp) ((pp)->flags[PARTITION_FLAG_LOC])

// TMP_PARTITION is boolean valued and is used to mark nodes as being
// either on the left or right side of the cut.  Since many cuts are
// examined, this flag will be re-marked for each cut
#define TMP_PARTITION(pp) ((pp)->flags[TMP_PARTITION_FLAG_LOC])

// NODE_NUM holds the number of the node produced by numberBlocks().
#define NODE_NUM(pp) ((pp)->flags[NODE_NUM_FLAG_LOC])



 
// Following macros are for PortHole
#define WEIGHT_FLAG_LOC 0
#define DELAY_TAG_FLAG_LOC 1

#define WEIGHT(ph) ((ph)->flags[WEIGHT_FLAG_LOC])
#define DELAY_TAG(ph) ((ph)->flags[DELAY_TAG_FLAG_LOC])



// TNSE = total number of samples exchanged in a complete period of an
// SDF schedule.

/****
Weight all portholes by the arcs weight: TNSE

@SideEffects the flag location <code>#defined</code> by <code>WEIGHT</code>
is set to TNSE for all PortHoles.

****/
void AcyCluster::weightArcs()
{
    SynDFCluster* c;
    int cost;
    SynDFClusterPort *outArc;
    SynDFClusterIter nextClust(*this);
    while ((c=nextClust++) != NULL) {
	SynDFClusterOutputIter nextO(*c);
	while ((outArc=nextO++) != NULL) {
	    // if far() is 0 then we are crossing a cluster boundary;
	    // hence, skip this arc.
	    if (!outArc->far()) continue;
	    cost = computeTNSE(c,(SynDFCluster*)outArc->far()->parent(),outArc);
	    if (loopFactor()) cost /= loopFactor();
	    WEIGHT(outArc) = cost;
	    WEIGHT(outArc->far()) = cost;
	}
    }
}

/****
Mark those portholes whose arcs have >= delays than the TNSE.

@Description This function marks those arcs that have more than or equal to
TNSE delays.  This distinction is important because for such arcs, there is
effectively no precedence constraint since all firings of the sink node on the
arc can be executed before any of the source arc.

@SideEffects flag location at <code>DELAY_TAG</code> is marked 1 for such
PortHole pairs

****/
void AcyCluster::tagDelayArcs()
{

    SynDFCluster* c;
    SynDFClusterPort *outArc;
    SynDFClusterIter nextClust(*this);
    while ((c=nextClust++) != NULL) {
	SynDFClusterOutputIter nextO(*c);
	while ((outArc=nextO++) != NULL) {
	    if (!outArc->far()) continue;
	    // determine if the arc has enough delays.
	    // Recall: WEIGHT(a) = TNSE(a)/loopFac
	    // Thus want to check if numInitDelays() > TNSE(a)=loopFac*WEIGHT
	    if (outArc->numInitDelays() >= loopFactor()* WEIGHT(outArc)) {
		DELAY_TAG(outArc) = 1;
		DELAY_TAG(outArc->far()) = 1;
	    } else {
		DELAY_TAG(outArc) = 0;
		DELAY_TAG(outArc->far()) = 0;
	    }
	}
    }
}

/****
Mark c and all the successors of c and return the number of successors.

@Description
The marking is done by setting the <code>TMP_PARTITION</code> flag to 1.
In computing the number of successors, we include the total number of
blocks inside all the clusters that might be inside us; however, only
the top-level clusters are actually marked.

@SideEffects flag location at <code>TMP_PARTITION</code> is marked 1
for nodes that are successors.

****/
int AcyCluster::markSuccessors(AcyCluster* c)
{
    // Depth first search.  2 means not done exploring that
    // subgraph yet, 1 means done, and 0 means not started yet.
    int numSucc=c->totalNumberOfBlocks();
    SynDFClusterPort* out;
    AcyCluster* nc=0;
    SynDFClusterOutputIter nextO(*c);
    TMP_PARTITION(c) = 2;
    // Below, we find those successors whose delay_tag_flag is 0
    // meaning that the arc to the successor does not have enough delays
    // to ignore as a precedence arc.
    while ((out = nextO.next(DELAY_TAG_FLAG_LOC,0)) != NULL) {
	// It should be the case that if out->far() is defined, then
	// so is out->far->parent(); the test below is conservative.
	// If out->far() is 0, then we are going across the cluster
	// boundary and hence we do nothing.
	if ( out->far() && (nc = (AcyCluster*)out->far()->parent()) != NULL) {
	    if (TMP_PARTITION(nc) == 2) {
		StringList message;
		message << "Cluster presented to AcyCluster::markSuccessors "
			<< "appears to be cyclic.\n This could be due to an "
			<< "earlier clustering step.  Aborting...\n";
		Error::abortRun(message);
		return 0;
	    }
	    if (TMP_PARTITION(nc) == 0) numSucc += markSuccessors(nc);
	}
    }
    TMP_PARTITION(c) = 1;
    return numSucc;
}

// Symmetric to <code>markSuccessors(AcyCluster*)</code>
int AcyCluster::markPredecessors(AcyCluster* c)
{
    // Note:  The following code is pretty much symmetric
    // to the one for markSuccessors.  Looking at comments above will help.
    // Here, 2 means not done yet, 0 means done, 1 means not started yet.
    
    int numPred = c->totalNumberOfBlocks();
    SynDFClusterPort* inp;
    AcyCluster* pc=0;
    SynDFClusterInputIter nextI(*c);
    TMP_PARTITION(c) = 2;
    while ((inp = nextI.next(DELAY_TAG_FLAG_LOC,0)) != NULL) {
	// It should be the case that if out->far() is defined, then
	// so is out->far->parent(); the test below is conservative.
	// If out->far() is 0, then we are going across the cluster
	// boundary and hence we do nothing.
	if (inp->far() && (pc = (AcyCluster*)inp->far()->parent()) != NULL) {
	    if (TMP_PARTITION(pc) == 2) {
		StringList message;
		message << "Cluster presented to AcyCluster::markPredecessors "
			<< "appears to be cyclic.\n This could be due to an "
			<< "earlier clustering step.  Aborting...\n";
		Error::abortRun(message);
		return 0;
	    }
	    if (TMP_PARTITION(pc) == 1) numPred += markPredecessors(pc);
	}
    }
    TMP_PARTITION(c) = 0;
    return numPred;
}

/****
Compute the cost of a cut.

@Description
It is assumed here that the cut
is specified by marking nodes at <code>flags[flag_loc]</code> by
<code>leftFlagValue</code> (left side of cut).

The cost of the cut is defined to be sums of the costs of all
arcs crossing from left to right, plus the number of delays on
arcs crossing from right to left.  Recall that the cost of an arc
is the TNSE plus the number of delays on it.

@Returns Int: cost of the cut

****/
int AcyCluster::computeCutCost(int flag_loc, int leftFlagValue)
{

    SynDFCluster* c;
    int cutVal = 0;
    SynDFClusterPort* arc;
    SynDFClusterIter nextClust(*this);

    while ((c = nextClust++) != NULL) {
	if (c->flags[flag_loc] == leftFlagValue) {
	    // c is on the left side of the cut.
	    // Now we have to check whether there is an arc
	    // from this node to a node that is on the right side.
	    SynDFClusterOutputIter nextArc(*c);
	    while ((arc=nextArc++) != NULL) {
		// arc->far() == 0 ==> arc going across parent cluster boundary
		if (!arc->far()) continue;
		if (arc->far()->parent()->flags[flag_loc] != leftFlagValue) {
		    cutVal += WEIGHT(arc) + arc->numInitDelays();
		}
	    }
	    // Also check if there are reverse arcs, and add the delay
	    // values to the cut value
	    SynDFClusterInputIter inpArc(*c);
	    while ((arc=inpArc.next(DELAY_TAG_FLAG_LOC,1)) != NULL) {
		if (!arc->far()) continue;
		if (arc->far()->parent()->flags[flag_loc] != leftFlagValue) {
		    cutVal += arc->numInitDelays();
		}
	    }
	}
    }
    return cutVal;
}

/****
Find the independent boundary nodes after a cut formed by markSuccessors or markPredecessors.

@Description
A cut of <code>type</code> = 0 is the cut
where the nodes {c+successors_of_c} are on the right side.
<code>type</code> = 1 is the cut where the set of nodes
{c+predecessors_of_c} are on the left side of the cut.
It is assumed that <code>mark{Successors,Predecessors}</code> was called
for cluster c.
    
Independent boundary nodes are nodes that are neither
predecessors nor successors of c
<b>(*independent*)</b>
and are such that all their successors are successors of c (type=0)
<b>(*boundary*)</b> (or all their predecessors are predecessors of c for
type=1 cuts).

@SideEffects the input list indepBndryNodes is filled with such nodes
****/
void AcyCluster::findIndepBndryNodes(int type, Cluster* c, SequentialList& indepBndryNodes)
{    
    Cluster *tmp;
    ClusterPort* arc;
    int flag, flag2;
    ClusterIter nextTmp(*this);
    if (type == 0) {
	while ((tmp = nextTmp++) != NULL) {
	    if (!TMP_PARTITION(tmp)) {
		// tmp is not a successor of c
		// Check if all of tmp's successors are successors of c
		ClusterOutputIter nextO(*tmp);
		flag=1;
		while ((arc = nextO.next(DELAY_TAG_FLAG_LOC,0)) != NULL && flag) {
		    if (!arc->far()) continue;
		    if (!TMP_PARTITION(arc->far()->parent())) flag = 0;
		}
		if (flag) {
		    // tmp is a boundary node; now check if it is a predecessor
		    // of c
		    ClusterInputIter nextI(*c);
		    flag2 = 1;
		    while ((arc=nextI.next(DELAY_TAG_FLAG_LOC,0))!=NULL && flag2) {
			// If arc does not have a farSidePort, then it means
			// that it is connecting across this cluster's boundary.
			// Hence we can skip it.
			if (!arc->far()) continue;
			if (arc->far()->parent() == tmp) flag2=0;
		    }
		    if (flag2) indepBndryNodes.append(tmp);
		}
	    }
	}
    } else {
	while ((tmp = nextTmp++) != NULL) {
	    if (TMP_PARTITION(tmp)) {
		// tmp is not a predecessor of c
		// Check if all of tmp's predecessors are predecessors of c
		// Use arc iterators to get predecessors connected through
		// non-delay-tagged arcs
		ClusterInputIter nextI(*tmp);
		flag=1;
		while ((arc = nextI.next(DELAY_TAG_FLAG_LOC,0)) != NULL && flag) {
		    if (!arc->far()) continue;
		    if (TMP_PARTITION(arc->far()->parent())) flag = 0;
		}
		if (flag) {
		    // tmp is a boundary node; now check if it is a successor
		    // of c
		    ClusterOutputIter nextO(*c);
		    flag2 = 1;
		    while ((arc=nextO.next(DELAY_TAG_FLAG_LOC,0))!=NULL && flag2) {
			// If arc does not have a farSidePort, then the
			// connection is going across this Cluster boundary.
			// Hence, we can skip it.
			if (!arc->far()) continue;
			if (arc->far()->parent() == tmp) flag2=0;
		    }
		    if (flag2) indepBndryNodes.append(tmp);
		}
	    }
	}
    }
}

/****
The most important routine in AcyCluster; finds a legal bounded cut of the graph

@Description
Produce a legal cut into bounded sets while minimizing weight of arcs
crossing the cut.
The following conditions have to hold for cuts found by this method:
<li> a) the number of nodes on
either side of the partition does not exceed K (Bounded sets)
<li> b) All arcs cross the cut in the same direction (legality)
<li> c) the cut value is equal to the supplied cutValue (to ensure that
he heuristic was minimizing the correct metric.
<p>
<a href="#ClassAcyCluster">See the  book refered to in
the comments for the class for more details.</a>
<p>
<a href="http://ptolemy.eecs.berkeley.edu/papers/PganRpmcDppo/">
Also see the first paper at this site.</a>

@SideEffects the flag at <code>PARTITION</code> is marked
for all nodes by the parents value
at that location if the node is on the left side of the cut and by the number
of nodes on the left side of the cut of the node is on the right side of the
cut.

@Returns Int: value of the cut if successful, -1 else.  <code>
AcyLoopScheduler::RPMC</code>, the only function that uses this function
currently, the value of the cut returned her is not really used except to
ensure that it is not -1.

****/
int AcyCluster::legalCutIntoBddSets(int K)
{ 
    int minCutVal = INT_MAX;
    int numSucc=0, numPred=0, cutVal;
    int Dtmp;
    AcyCluster* c;
    AcyCluster* tmp;
    AcyClusterIter nextClust(*this);

    // The following function  marks all of the portholes with weight TNSE
    // (plus the number of delays on the arc)
    // so that we don't have to repeatedly compute those values.
    
    weightArcs();
    
    // Now we need to tag all ports that connect arcs having sufficient
    // delays because we do not want to consider these arcs as precedence
    // constraints.  Sufficient delays means more delays than the total
    // number of samples exchanged (TNSE) in a complete SDF schedule period.
    // So a single appearance schedule can have the sink node executing before
    // the source node on such arcs.
    
    tagDelayArcs();
    

    // Iterate over all nodes in the cluster and examine all legal cuts
    // of the form {c+successors_of_c} on the right side of the cut and
    // {c+predecessors_of_c} on the left side of the cut.  For each such
    // cut, an optimization step is employed to further reduce the cost
    // of the cut; this step is explained later on.

    /////////////////////////
    ///// Successors ////////
    /////////////////////////
    
    while ((c = nextClust++) != NULL) {

	// reset the flag flags[1] to 0 for all blocks inside us.
	
	resetFlags(*this,TMP_PARTITION_FLAG_LOC,0);
	numSucc = markSuccessors(c);
	if (!numSucc) return -1;

	// If set has too many nodes; consider the next cut
	// Caveat: this could be the case with all cuts of this type (and
	// of the {c + predecessors} type also).  Hence, the routine
	// might return FALSE saying no cut that respects the bound was
	// found.  This does not mean that no such cut exists.  An alternative
	// would be to see if the optimization step below moves some nodes
	// across thereby meeting the bound perhaps.   However, even this
	// might not work since no node may be moved across.  In general,
	// we would have to modify the optimization step to move nodes
	// in even if they increase the cost so that the bound can be met.
	// The whole trade-off is between meeting the bound versus cost of
	// the cut.  Here, we would rather take lopsided cuts with low cost
	// (if no cuts meeting the bound exist) rather than take bounded cuts
	// with higher costs.

	if (numSucc > K) continue;
	cutVal = computeCutCost(TMP_PARTITION_FLAG_LOC,0);

	// Apply optimization step here.
	// Nodes that are moved across will have their TMP_PARTITION flag
	// marked as 1 also.
	
	// That is, a) find the independent boundary nodes w.r.t. c
	// b) Determine if moving such a node across decreases cost.
	// c) Move only those that decrease the cost.
	// d) Since we only consider boundary nodes, the order in
	// which these nodes are considered or moved does not matter.
		
	// Refer to the book for more details on this heuristic.

	SequentialList indepBndryNodes;
	findIndepBndryNodes(0,c, indepBndryNodes);
	
	// Now we have all the boundary, independent nodes.  Determine
	// whether we can move any across and lower the cut value.
	// We do this by computing D(a) = I(a) - E(a) where I(a) is the
	// sum of the weights of all input arcs of node a and E(a) is the
	// sum of the weights of all output arcs from node a.  a is a boundary
	// independent node.  So all a for which D(a) < 0 are moved across
	// into the right side.
	
	ListIter nextBndryNode(indepBndryNodes);
	while ((tmp = (AcyCluster*)nextBndryNode++) != NULL) {
	    Dtmp = costOfMovingAcross(tmp,1);
	    if (Dtmp<0 && numSucc < K) {
		cutVal += Dtmp;
		TMP_PARTITION(tmp) = 1;
		numSucc += tmp->totalNumberOfBlocks();
	    }
	}
	
	// Update best cut.  
	
	if (minCutVal > cutVal && numSucc <= K && totalNumberOfBlocks()-numSucc <= K) {
	    updateBestCut();
	    minCutVal = cutVal;
	}
    }
    
    ////////////////////////
    //// Predecessors //////
    ////////////////////////
    
    nextClust.reset();
    while ((c=nextClust++) != NULL) {

	// The code here is symmetric to the code for successors;
	// extensive comments can be found there.

	resetFlags(*this,TMP_PARTITION_FLAG_LOC,1);
	numPred = markPredecessors(c);
	if (!numPred) return -1;

	if (numPred > K) continue;
	cutVal = computeCutCost(TMP_PARTITION_FLAG_LOC,0);
	
	SequentialList indepBndryNodes;
	findIndepBndryNodes(1, c, indepBndryNodes);

	// Now we have all the boundary, independent nodes.  Determine
	// whether we can move any across and lower the cut value.
	// We do this by computing D(a) = I(a) - E(a) where I(a) is the
	// sum of the weights of all output arcs of node a and E(a) is the
	// sum of the weights of all input arcs to node a.  a is a boundary
	// independent node.  So all a for which D(a) < 0 are moved across
	// into the predecessor set.
	
	ListIter nextBndryNode(indepBndryNodes);
	while ((tmp = (AcyCluster*)nextBndryNode++) != NULL) {
	    Dtmp = costOfMovingAcross(tmp,-1);
	    if (Dtmp<0 && numPred < K) {
		cutVal += Dtmp;
		TMP_PARTITION(tmp) = 0;
		numPred += tmp->totalNumberOfBlocks();
	    }
	}
	// Update best cut.	
	if (minCutVal > cutVal && numPred <= K && totalNumberOfBlocks()-numPred <= K) {
	    minCutVal = cutVal;
	    updateBestCut();
	}
    }
    if (minCutVal == INT_MAX) {
	// There is no legal cut that obeys the bound; hence,
	// return -1.  See the comments in the optimization step in the 
	// Successors section of the code above.
	return -1;
    }

    // Now we check the above.  We follow Blum's advice: programs should
    // always check their work since the check is usually less expensive
    // and uses a different algorithm than the one used to compute the result.
    // This is useful for catching bugs and
    // hence should *always* be done, and not just during debugging.
    
    if (!checkLegalCut(minCutVal,K)) {
	Error::warn("Function legalCutIntoBddSets computed false answer");
    }
    return minCutVal;
}

/****
Compute the cost of moving an independent boundary node across the cut.

@Description
If <code>direction</code> is 1, then the node is moved
from left to right; if <code>direction</code> is -1, then it is moved
moved from right to left.  Hence, in one case, <code>Atmp</code> represents
the cost of all the internal arcs that will become external if
the node is moved across (<code>direction = -1</code>), and in the other
case it represents the cost of all external arcs (ie, arcs crossing
the cut) which will become internal if the node is moved across.
If <code>Atmp</code> is the internal cost, then <code>Btmp</code> is the
external cost and vice-versa.

@Returns Int, the cost

****/
int AcyCluster::costOfMovingAcross(Cluster* bndryNode, int direction)
{
    // Compute the cost of moving an independent boundary node
    // across the cut.  If direction is 1, then the node is moved
    // from left to right; if direction is -1, then it is moved
    // moved from right to left.  Hence, in one case, Atmp represents
    // the cost of all the internal arcs that will become external if
    // the node is moved across (direction = -1), and in the other
    // case it represents the cost of all external arcs (ie, arcs crossing
    // the cut) which will become internal if the node is moved across.
    // If Atmp is the internal cost, then Btmp is the external cost and
    // vice-versa.
    
    int Atmp=0, Btmp=0;
    ClusterPort* arc;
    ClusterOutputIter nextO(*bndryNode);
    ClusterInputIter nextI(*bndryNode);
    while ((arc=nextO++) != NULL) {
	if (!arc->far()) continue;
	if (TMP_PARTITION(arc->far()->parent())) {
	    Atmp += WEIGHT(arc) + arc->numInitDelays();
	} else {
	    // It is a predecessor meaning that the reverse
	    // arc has enough delays on it (since only arcs with
	    // enough delays are allowed to cross the cut in the
	    // reverse direction.
	    Btmp += arc->numInitDelays();
	}
    }
    while ((arc=nextI++) !=NULL ) {
	if (!arc->far()) continue;
	if (TMP_PARTITION(arc->far()->parent())) {
	    // Not a predecessor meaning that the arc must have enough
	    // delays (since tmp is a boundary node and if it has
	    // an incoming arc from a node that is not a predecessor
	    // of c, then the arc has enough delays on it to be
	    // not considered a precedence arc.
	    Atmp += arc->numInitDelays();
	} else {
	    // It is a predecessor meaning that the arc is external
	    Btmp += WEIGHT(arc) + arc->numInitDelays();
	}
    }
    if (direction == 1) {
	return Btmp - Atmp;
    }
    if (direction == -1) {
	return Atmp - Btmp;
    }
    return 0;
}

/****
Update best cut.

@Description
Just copies the 0/1 values from the TMP_PARTITION location.

@SideEffects see description above

****/    
void AcyCluster::updateBestCut()
{    
    Cluster* tmp;
    ClusterIter nextTmp(*this);
    while((tmp = nextTmp++) !=NULL) {
	PARTITION(tmp) = TMP_PARTITION(tmp);
    }
}

/****
This procedure checks the answer given by <code>legalCutIntoBddSets</code>.

@Description
The assumptions are that <code>legalCutIntoBddSets</code>
has marked the nodes in <code>*this</code> by which side of the partition
they are on.  Hence, the check should be called before those flags
are potentially reset by some other method.  In this method, we
go through the blocks and ensure that
<li>a) the number of nodes on
either side of the partition does not exceed K (Bounded sets)
<li>b) All arcs cross the cut in the same direction (legality)
<li>c) the cut value is equal to the supplied cutValue (to ensure that
the heuristic was minimizing the correct metric.

@Returns Int: TRUE or FALSE depending on success.

****/
int AcyCluster::checkLegalCut(int cutValue, int bdd)
{

    int K=0;
    int check = TRUE;
    AcyCluster *c;
    SynDFClusterPort* arc;
    // Recall that nodes on the left side of the cut get the value of the
    // parent.
    //int leftFlag = PARTITION(this);
    int leftFlag = 0;
    AcyClusterIter nextClust(*this);
    // Check that sets are bounded
    while ((c = nextClust++) != NULL) {
	if (PARTITION(c) != leftFlag) K += c->totalNumberOfBlocks();
    }
    if (K > bdd || totalNumberOfBlocks()-K > bdd) {
	Error::warn("Cut does not respect the bound");
	check = FALSE;
    }
    // Check that cut is legal
    nextClust.reset();
    while ((c = nextClust++) != NULL) {
	if (PARTITION(c) != leftFlag) {
	    SynDFClusterOutputIter nextO(*c);
	    while ((arc=nextO.next(DELAY_TAG_FLAG_LOC,0)) != NULL) {
		if (!arc->far()) continue;
		if (PARTITION(arc->far()->parent()) == leftFlag) {
		    // We abort since if legality is violated
		    // then schedule might not be valid.
		    // This means that legalCutIntoBddSets has not
		    // produced a legal cut.
		    Error::abortRun("Cut of the graph is not legal; aborting");
		    return FALSE;
		}
	    }
	}
    }

    // Check that the cut value claimed by heuristic jives with actual cut
    // value induced by the partition

    if (cutValue != computeCutCost(PARTITION_FLAG_LOC, leftFlag)) {
   	Error::warn("Cut value computed by heuristic does not agree with check");
	check = FALSE;
    }
    return check;
}
