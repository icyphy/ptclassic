static const char file_id[] = "AcyCluster.cc";
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

// following are for Blocks
// NODE_NUM holds the number of the node produced by numberBlocks().
#define NODE_NUM flags[0]
// TMP_PARTITION is boolean valued and is used to mark nodes as being
// either on the left or right side of the cut.  Since many cuts are
// examined, this flag will be re-marked for each cut
#define TMP_PARTITION flags[1]
// PARTITION is used to hold TMP_PARTITION from the best cut
#define PARTITION flags[2]
#define TMP_PARTITION_FLAG 1
#define PARTITION_FLAG 2

// following are for PortHole
#define WEIGHT flags[0]
#define DELAY_TAG flags[1]


#define DELAY_TAG_FLAG 1

// TNSE = total number of samples exchanged in a complete period of an
// SDF schedule.

void AcyCluster::weightArcs()
{
    // weight all portholes by the arcs weight: TNSE
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
	    outArc->WEIGHT = cost;
	    outArc->far()->WEIGHT = cost;
	}
    }
}

void AcyCluster::tagDelayArcs()
{
    // Mark those portholes whose arcs have >= delays than the TNSE.

    SynDFCluster* c;
    SynDFClusterPort *outArc;
    SynDFClusterIter nextClust(*this);
    while ((c=nextClust++) != NULL) {
	SynDFClusterOutputIter nextO(*c);
	while ((outArc=nextO++) != NULL) {
	    if (!outArc->far()) continue;
	    // determine if the arc has enough delays.
	    // 2* because WEIGHT has delays as cost already.
	    if (2*outArc->numInitDelays() >= outArc->WEIGHT) {
		outArc->DELAY_TAG = 1;
		outArc->far()->DELAY_TAG = 1;
	    }
	}
    }
}

int AcyCluster::markSuccessors(AcyCluster* c)
{
    // Mark c and all the successors of c and return the number of successors.
    // The marking is done by setting the TMP_PARTITION flag to 1

    // in computing the number of successors, we include the total number of
    // blocks inside all the clusters that might be inside us; however, only
    // the top-level clusters are actually marked
    
    int numSucc=c->totalNumberOfBlocks();
    SynDFClusterPort* out;
    AcyCluster* nc=0;
    SynDFClusterOutputIter nextO(*c);
    c->TMP_PARTITION = 1;
    // Below, we find those successors whose delay_tag_flag is 0
    // meaning that the arc to the successor does not have enough delays
    // to ignore as a precedence arc.
    while ((out = nextO.next(DELAY_TAG_FLAG,0)) != NULL) {
	// It should be the case that if out->far() is defined, then
	// so is out->far->parent(); the test below is conservative.
	// If out->far() is 0, then we are going across the cluster
	// boundary and hence we do nothing.
	if ( out->far() && (nc = (AcyCluster*)out->far()->parent()) != NULL) {
	    nc->TMP_PARTITION = 1;
	    numSucc += markSuccessors(nc);
	}
    }
    return numSucc;
}

int AcyCluster::markPredecessors(AcyCluster* c)
{
    // Note:  The following code is pretty much symmetric
    // to the one for markSuccessors.  Looking at comments above will help.
    
    int numPred = c->totalNumberOfBlocks();
    SynDFClusterPort* inp;
    AcyCluster* pc=0;
    SynDFClusterInputIter nextI(*c);
    c->TMP_PARTITION = 0;
    while ((inp = nextI.next(DELAY_TAG_FLAG,0)) != NULL) {
	// It should be the case that if out->far() is defined, then
	// so is out->far->parent(); the test below is conservative.
	// If out->far() is 0, then we are going across the cluster
	// boundary and hence we do nothing.
	if (inp->far() && (pc = (AcyCluster*)inp->far()->parent()) != NULL) {
	    pc->TMP_PARTITION = 0;
	    numPred += markPredecessors(pc);
	}
    }
    return numPred;
}

int AcyCluster::computeCutCost(int flag_loc, int leftFlagValue)
{
    // Compute the cost of a cut.  It is assumed here that the cut
    // is specified by marking nodes at flags[flag_loc] by leftFlagValue
    // (left side of cut).

    // The cost of the cut is defined to be sums of the costs of all
    // arcs crossing from left to right, plus the number of delays on
    // arcs crossing from right to left.  Recall that the cost of an arc
    // is the TNSE plus the number of delays on it.

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
		    cutVal += arc->WEIGHT;
		}
	    }
	    // Also check if there are reverse arcs, and add the delay
	    // values to the cut value
	    SynDFClusterInputIter inpArc(*c);
	    while ((arc=inpArc.next(DELAY_TAG_FLAG,1)) != NULL) {
		if (!arc->far()) continue;
		if (arc->far()->parent()->flags[flag_loc] != leftFlagValue) {
		    cutVal += arc->numInitDelays();
		}
	    }
	}
    }
    return cutVal;
}

void AcyCluster::findIndepBndryNodes(int type, Cluster* c, SequentialList& indepBndryNodes)
{
    // Find the independent boundary nodes after a cut formed by
    // markSuccessors or markPredecessors.  A cut of type = 0 is the cut
    // where the nodes {c+successors_of_c} are on the right side.
    // type = 1 is the cut where the set of nodes {c+predecessors_of_c} are
    // on the left side of the cut.
    // It is assumed that mark{Successors,Predecessors} was called
    // for cluster c.
    
    // Independent boundary nodes are nodes that are neither
    // predecessors nor successors of c
    // (*independent*)
    // and are such that all their successors are successors of c (type=0)
    // (*boundary*) (or all their predecessors are predecessors of c for
    // type=1 cuts).
    
    Cluster *tmp;
    ClusterPort* arc;
    int flag, flag2;
    ClusterIter nextTmp(*this);
    if (type == 0) {
	while ((tmp = nextTmp++) != NULL) {
	    if (!tmp->TMP_PARTITION) {
		// tmp is not a successor of c
		// Check if all of tmp's successors are successors of c
		ClusterOutputIter nextO(*tmp);
		flag=1;
		while ((arc = nextO.next(DELAY_TAG_FLAG,0)) != NULL && flag) {
		    if (!arc->far()) continue;
		    if (!arc->far()->parent()->TMP_PARTITION) flag = 0;
		}
		if (flag) {
		    // tmp is a boundary node; now check if it is a predecessor
		    // of c
		    ClusterInputIter nextI(*c);
		    flag2 = 1;
		    while ((arc=nextI.next(DELAY_TAG_FLAG,0))!=NULL && flag2) {
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
	    if (tmp->TMP_PARTITION) {
		// tmp is not a predecessor of c
		// Check if all of tmp's predecessors are predecessors of c
		// Use arc iterators to get predecessors connected through
		// non-delay-tagged arcs
		ClusterInputIter nextI(*tmp);
		flag=1;
		while ((arc = nextI.next(DELAY_TAG_FLAG,0)) != NULL && flag) {
		    if (!arc->far()) continue;
		    if (arc->far()->parent()->TMP_PARTITION) flag = 0;
		}
		if (flag) {
		    // tmp is a boundary node; now check if it is a successor
		    // of c
		    ClusterOutputIter nextO(*c);
		    flag2 = 1;
		    while ((arc=nextO.next(DELAY_TAG_FLAG,0))!=NULL && flag2) {
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

int AcyCluster::legalCutIntoBddSets(int K)
{
    // Produce a legal cut into bounded sets while minimizing weight of arcs
    // crossing the cut.
    
    int minCutVal = (int)HUGE_VAL;
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
	
	resetFlags(*this,1,0);
	numSucc = markSuccessors(c);

	// If set has too many nodes; consider the next cut
	// Caveat: this could be the case with all cuts of this type (and
	// of the {c + predecessors} type also).  Hence, the routine
	// might return FALSE saying on cut that respects the bound was
	// found.  This does not mean that no such cut exists.  An alternative
	// would be see if the optimization step below moves some nodes
	// across thereby meeting the bound perhaps.   However, even this
	// might not work since no node may be moved across.  In general,
	// we would have to modify the optimization step to move nodes
	// in even if they increase the cost so that the bound can be met.
	// The whole trade-off is between meeting the bound versus cost of
	// the cut.  Here, we would rather take lopsided cuts with low cost
	// (if no cuts meeting the bound exist) rather than take bounded cuts
	// with higher costs.

	if (numSucc > K) continue;
	cutVal = computeCutCost(TMP_PARTITION_FLAG,0);
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
		tmp->TMP_PARTITION = 1;
		numSucc += tmp->totalNumberOfBlocks();
	    }
	}
	
	// Update best cut.  Also, use the following numbering scheme
	// to indicate the partition.  Nodes on the left side of the
	// cut get the value of their parent while the nodes on the
	// right side of the cut get the value of parent + number of
	// nodes on left hand side.  This is to make it easy to deduce the
	// ordering if this function is called recursively many times.
	
	if (minCutVal > cutVal && numSucc <= K && totalNumberOfBlocks()-numSucc <= K) {
	    updateBestCut(totalNumberOfBlocks()-numSucc);
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

	resetFlags(*this,1,1);
	numPred = markPredecessors(c);
	if (numPred > K) continue;
	cutVal = computeCutCost(TMP_PARTITION_FLAG,0);
	
	SequentialList indepBndryNodes;
	findIndepBndryNodes(1,c, indepBndryNodes);
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
		tmp->TMP_PARTITION = 0;
		numPred += tmp->totalNumberOfBlocks();
	    }
	}
	// Update best cut.	
	if (minCutVal > cutVal && numPred <= K && totalNumberOfBlocks()-numPred <= K) {
	    minCutVal = cutVal;
	    updateBestCut(numPred);
	}
    }
    if (minCutVal == (int)HUGE_VAL) {
	// There is no legal cut that obeys the bound; hence,
	// return -1.  See the comments in the optimization step in the 
	// Successors section of the code above.
	return -1;
    }
    //
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
	if (arc->far()->parent()->TMP_PARTITION) {
	    Atmp += arc->WEIGHT;
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
	if (arc->far()->parent()->TMP_PARTITION) {
	    // Not a predecessor meaning that the arc must have enough
	    // delays (since tmp is a boundary node and if it has
	    // an incoming arc from a node that is not a predecessor
	    // of c, then the arc has enough delays on it to be
	    // not considered a precedence arc.
	    Atmp += arc->numInitDelays();
	} else {
	    // It is a predecessor meaning that the arc is external
	    Btmp += arc->WEIGHT;
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

void AcyCluster::updateBestCut(int numOnLeftSide)
{    
    // Update best cut.  Also, use the following numbering scheme
    // to indicate the partition.  Nodes on the left side of the
    // cut get the value of their parent while the nodes on the
    // right side of the cut get the value of parent (that is, this->PARTITION)
    // + number of
    // nodes on left hand side.  This is to make it easy to deduce the
    // ordering if this function is called recursively many times.
    // numOnLeftSide is the number of nodes on the left side of the cut.
    
    Cluster* tmp;
    ClusterIter nextTmp(*this);
    while((tmp = nextTmp++) !=NULL) {
	tmp->PARTITION=PARTITION*(!tmp->TMP_PARTITION)+
	(PARTITION + numOnLeftSide)*(tmp->TMP_PARTITION);
    }
}

int AcyCluster::checkLegalCut(int cutValue, int bdd)
{
    // This procedure checks the answer given by the procedure
    // legalCutIntoBddSets.  The assumptions are that legalCutIntoBddSets
    // has marked the nodes in *this by which side of the partition they
    // are on.  Hence, the check should be called before those flags
    // are potentially reset by some other method.  In this method, we
    // go through the blocks and ensure that
    // a) the number of nodes on
    // either side of the partition does not exceed K (Bounded sets)
    // b) All arcs cross the cut in the same direction (legality)
    // c) the cut value is equal to the supplied cutValue (to ensure that
    // the heuristic was minimizing the correct metric.

    int K=0;
    int check = TRUE;
    AcyCluster *c;
    SynDFClusterPort* arc;
    // Recall that nodes on the left side of the cut get the value of the
    // parent.
    int leftFlag = PARTITION;
    AcyClusterIter nextClust(*this);
    // Check that sets are bounded
    while ((c=nextClust++) != NULL) {
	if (c->PARTITION != leftFlag) K += c->totalNumberOfBlocks();
    }
    if (K > bdd || totalNumberOfBlocks()-K > bdd) {
	Error::warn("Cut does not respect the bound");
	check = FALSE;
    }
    // Check that cut is legal
    nextClust.reset();
    while ((c=nextClust++) != NULL) {
	if (c->PARTITION != leftFlag) {
	    SynDFClusterOutputIter nextO(*c);
	    while ((arc=nextO.next(DELAY_TAG_FLAG,0)) != NULL) {
		if (!arc->far()) continue;
		if (arc->far()->parent()->PARTITION == leftFlag) {
		    // We abort since if legality is violated
		    // then schedule might not be valid.
		    // This means that legalCutIntoBddSets has not
		    // produced a legal cut.
		    Error::abortRun("Cut of the graph is not legal; aborting");
		    check = FALSE;
		}
	    }
	}
    }
    // Check that the cut value claimed by heuristic jives with actual cut
    // value induced by the partition
    if (cutValue != computeCutCost(PARTITION_FLAG,leftFlag)) {
   	Error::warn("Cut value computed by heuristic does not agree with check");
	check = FALSE;
    }
    return check;
}


