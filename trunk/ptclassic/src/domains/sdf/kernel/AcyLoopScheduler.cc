static const char file_id[] = "AcyLoopScheduler.cc";
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

 Programmer:  Praveen K. Murthy
 Date of creation: Feb/9/96

Acyclic loop scheduler to optimize for buffer sizes.
Implements algorithms and heuristics described in Chapters 6,7 of 

	"Software synthesis from dataflow graphs", by

Shuvra S. Bhattacharyya, Praveen K. Murthy, and Edward A. Lee,
Kluwer Academic Publishers, Norwood, MA, 1996

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "AcyLoopScheduler.h"
#include "AcyCluster.h"
#include "Geodesic.h"
#include "Target.h"
#include "Error.h"
#include "pt_fstream.h"
#include "DataFlowStar.h"
#include "Cluster.h"
#include <minmax.h>

// following defines are used for Blocks
#define PARTITION flags[2]
#define TMP_PARTITION flags[1]
#define NODE_NUM flags[0]

int isAcyclic(Galaxy* g, int ignoreDelayTags);
void findSources(Galaxy* g, int flagLoc, SequentialList& sources,DataFlowStar* deletedNode=0);
SequentialList findSinks(Galaxy* g, int flagLoc, DataFlowStar* deletedNode=0);

// Set a bunch of pointers to 0 (that will be allocated later).
AcyLoopScheduler :: AcyLoopScheduler()
{
    graph = 0;
    topSort = 0;
    topInvSort = 0;
    nodelist = 0;
    incMatrix = 0;
    reachMatrix = 0;
    delMatrix = 0;
    gcdMatrix = 0;
    splitMatrix = 0;
    costMatrix = 0;
    RPMCTopSort = 0;
    APGANTopSort = 0;
}


// Destroy a bunch of matrices and arrays used by the scheduler
AcyLoopScheduler::~AcyLoopScheduler()
{
    delete [] RPMCTopSort;
    delete [] APGANTopSort;
    delete [] topInvSort;
    delete [] nodelist;
    delete incMatrix;
    delete reachMatrix;
    delete delMatrix;
    delete gcdMatrix;
    delete splitMatrix;
    delete costMatrix;
    delete graph;
}

/****

Create a reachability matrix; it is required by the APGAN function

@Description 
    // The reachability matrix R is an NxN matrix, where N = #of nodes
    // in the SDF graph, such that R(i,j) = 1 iff there is a directed
    // path from node i to node j in the SDF graph.  R(i,j) = 0 otherwise.
    //
    // We use a very simple algorithm to compute this matrix here; there
    // are probably faster algorithms around (notably, Strassen's algorithm
    // for multiplying 2 NxN matrices in O(N^2.81) time is asymptotically
    // better than the O(N^3) algorithm used here).
    // For each node i, we do a breadth-first search from i and set those
    // entries R(i,j) to 1 for which j gets visted during the search.
    // We do this for every node in the graph.  Since every node and edge
    // could be visited during the search from i, the running time is
    // O(N+E) for one node.  Since there are N nodes, the time is
    // O(N(N+E)) = O(N^2 + N*E) = O(N^3) in the worst case.  However, most
    // practical acyclic SDF graphs are sparse; hence, O(N^2) is more realistic.

@SideEffects Creates reachMatrix, a class data member.

****/
void AcyLoopScheduler :: createReachabilityMatrix(Galaxy& gal)
{
    DataFlowStar *s, *t, *u;
    GalTopBlockIter nextStar(gal);
    SequentialList nodesTobeVisited;
    delete reachMatrix;
    reachMatrix = new Matrix(graphSize, graphSize);

    while ((s=(DataFlowStar*)nextStar++) != NULL) {
	nodesTobeVisited.initialize();
	nodesTobeVisited.append(s);
	while ((t = (DataFlowStar*)nodesTobeVisited.getAndRemove()) != NULL) {
	    SuccessorIter nextSucc(*t);
	    while ((u=(DataFlowStar*)nextSucc++) != NULL) {
		if (reachMatrix->m[u->NODE_NUM][s->NODE_NUM]) {
		    Error::abortRun("Failed to create reachability matrix.
		                      Graph appears to be non-acyclic.");
		    return;
		}
		reachMatrix->m[s->NODE_NUM][u->NODE_NUM] = 1;
		nodesTobeVisited.append(u);
	    }
	}
    }
}

// following definition is for ports
#define DELETE flags[2]

/****

Create a list of edges in the graph

@SideEffects Initializes and appends SynDFClusterPort pointers to edgelist,
a class data member.

****/
void AcyLoopScheduler :: createEdgelist(AcyCluster* gr)
{
    AcyClusterIter nextClust(*gr);
    SynDFClusterPort *clustPort;
    AcyCluster *c;
    edgelist.initialize();
    nextClust.reset();
    while ((c=nextClust++) != NULL) {
	SynDFClusterPortIter nextClustPort(*c);
	while ((clustPort = nextClustPort++) != NULL) {
	    if (clustPort->isItInput()) continue;
	    clustPort->DELETE = 0;
	    edgelist.append(clustPort);
	}
    }
}

/****

Create an incidence matrix representation of the universe

@Description
    // Create an incidence matrix representation.  This representation
    // has the following form:  it is an NxN matrix Inc, where N is number
    // of nodes.  The (i,j)th element of the matrix represents the numbe
    // of tokens produced by node i on arc (i,j).
    // If Inc(i,j) > 0, then i produces Inc(i,j) tokens per firing on arc (i,j).
    // Del(i,j) is the number of delays on arc (i,j)
    // We do not need
    // the numbers consumed here since we already know the repetitions vector.
    // Also, since we can have a multi-graph, we collapse multiple
    // arcs into one arc here.  This is fine since this matrix is
    // used to compute buffering costs so an equivalent arc with the
    // correct numbers will be identical in cost to multiple arcs.

@SideEffects Creates incMatrix, delMatrix, and nodelist, three class data
members

*****/
void AcyLoopScheduler :: createIncidenceMatrix(Galaxy& gal)
{
    DataFlowStar *s, *t;
    DFPortHole* starPort;
    int numP, numD;
    GalTopBlockIter nextStar(gal);
    delete incMatrix;
    delete delMatrix;
    incMatrix = new Matrix(graphSize,graphSize);
    delMatrix = new Matrix(graphSize,graphSize);

    // Holds the nodes in galaxy as an array
    delete [] nodelist;
    nodelist = new DataFlowStar*[graphSize];
    nextStar.reset();
    while ( (s = (DataFlowStar*)nextStar++) != NULL ) {
	nodelist[s->NODE_NUM] = s;
	DFStarPortIter nextPort(*s);
	while ((starPort = nextPort++) != NULL) {
	    if (starPort->isItInput()) continue;
	    t = (DataFlowStar*)starPort->far()->parent();
	    numP = starPort->numXfer();
	    numD = starPort->numInitDelays();
	    // += to take care of multi-arcs
	    // Note: adding up the delays loses information because
	    // it can affect the single appearance schedule that is constructed.
	    // Basically, if the sum of the delays is less than the repetitions
	    // of the source actor times the sum of the numbers produced,
	    // then we know that in any single appearance schedule all firings
	    // of the source have to occur before the sink.  If the sum of the
	    // delays is greater, then we need to look at the original multi-arc
	    // since only if the delay on each arc between the two nodes
	    // is greater than the number of samples exchanged (repetions
	    // of source times number produce on that particular arc) can
	    // we fire all occurances of the sink before the source.  Since
	    // this is likely to be a rare event, in the buffer-computation
	    // algorithms, we will make sure of this if the sum of the delays
	    // happens to be greater.
	    incMatrix->m[s->NODE_NUM][t->NODE_NUM] += numP;
	    delMatrix->m[s->NODE_NUM][t->NODE_NUM] += numD;
	}
    }   
}

/****

Copy the flag value of the first cluster inside this cluster to its flags

@Description
// The following function steps through each cluster inside gr and sets
// flags[flagLocation] for that cluster by the value of flags[flagLocation]
// for the head() of that cluster.  When all of the clusters in gr are
// atomic; i.e, each has exactly one Star, this function's action simply
// results in the flag value of the Star being propagated to its cluster
// wrapper.  Hence, this function is usually called after a cluster hierarchy
// has been set up using <code>Cluster::initializeForClustering</code>
// so that things
// like node numbering can be kept consistent in the Cluster hierarchy
// as well.

@SideEffects <code>flags[flagLocation]</code> for each cluster in gr changes

****/
void AcyLoopScheduler :: copyFlagsToClusters(AcyCluster* gr, int flagLocation)
{
    AcyCluster* c;
    AcyClusterIter nextClust(*gr);
    while ((c=nextClust++) != NULL) {
	c->flags[flagLocation] = c->head()->flags[flagLocation];
    }
}

/*****

This is the main routine called from Scheduler::setup

@Description This routine computes the schedule.  It uses the better of
two heuristics, APGAN and RPMC to determine which schedule to use.

@Returns Int: TRUE or FALSE depending on whether the schedule was successfully
computed.

@SideEffects Main ones are that the input galaxy becomes a cluster
hierarchy, and several class data members are created.

****/
int AcyLoopScheduler :: computeSchedule(Galaxy& gal)
{
    DataFlowStar *d;
    int i=0, rpmc, apgan, rpmcDppo, apganDppo;
    SequentialList wellOrderedList;
    AcyCluster *clusterGraph=0;
    graphSize = gal.numberBlocks();
    delete [] RPMCTopSort;
    delete [] APGANTopSort;
    delete [] topInvSort;
    RPMCTopSort = new int[graphSize];
    APGANTopSort = new int[graphSize];
    topInvSort = new int[graphSize];
    if (!isAcyclic(&gal,0)) {
	StringList message;
	message << "Graph is not acyclic; this loop scheduler cannot be used.";
	message << "Set the looping level parameter in the target to 0, 1, or";
	message << "2 and rerun the universe";
	Error::abortRun(message);
	return FALSE;
    }
    isWellOrdered(&gal, wellOrderedList);

    numberBlocks(gal,0);  // Numbers top-level blocks in galaxy at flags[0]
    createIncidenceMatrix(gal);

    if (wellOrderedList.size() == graphSize) {
	// Means the graph is well ordered; hence, just fill up the
	// topSort array and call DPPO on it.
	i=0;
	ListIter nextStar(wellOrderedList);
	while ( (d=(DataFlowStar*)nextStar++) != NULL) {
	    RPMCTopSort[i] = d->NODE_NUM; i++;
	}
	topSort = RPMCTopSort;
	rpmcDppo = DPPO();
    } else {
	// Generate a topological sort.  The costs rpmc, apgan are irrelevant
	// here since DPPO will give the actual cost.

	// Reachability matrix is needed by APGAN.
	createReachabilityMatrix(gal);

	delete graph;
	graph = new AcyCluster;

	// Prepare galaxy for clustering
	graph->initializeForClustering(gal);
	gal.PARTITION = 0;
	clusterGraph = (AcyCluster*)(&gal);
	clusterGraph->settnob(-1);

	// The following function is a hack; see its comments for more info.
	if (!clusterSplicedStars(clusterGraph)) return FALSE;

	// First of the two main heuristics
	rpmc = RPMC(clusterGraph);
	if (rpmc < 0) return FALSE;
	topSort = RPMCTopSort;
	rpmcDppo = DPPO();

	// save results for rpmc
	Matrix RPMCGcdMatrix = *gcdMatrix;
	Matrix RPMCSplitMatrix = *splitMatrix;

	// Flatten everything for the second heuristic
	graph->initializeForClustering(gal);
	gal.PARTITION = 0;
	clusterGraph->settnob(-1);

	// APGAN requires the node numbers in the clusters as well; hence,
	// copy those numbers to the constituent clusters.  RPMC did not
	// want this so wasn't done before.
	copyFlagsToClusters(clusterGraph,0);

	// Second of the two main heuristics
	apgan = APGAN(clusterGraph);
	if (apgan<0) return FALSE;
	topSort = APGANTopSort;
	apganDppo = DPPO();

	// Compare the two and use the better one.
	if (rpmcDppo < apganDppo) {
	    // use RPMC results
	    *gcdMatrix = RPMCGcdMatrix;
	    *splitMatrix = RPMCSplitMatrix;
	    topSort = RPMCTopSort;
	}
    }

    // build inverse topSort array, topInvSort.  This array gives
    // the position in topSort for a node i.  So 
    // topSort[topInvSort[i]] == topInvSort[topSort[i]] == i

    for (i=0; i<graphSize; i++) topInvSort[topSort[i]] = i;

    // finally, set all the buffer sizes in accordance with the schedule.
    fixBufferSizes(0,graphSize-1);
    // FIXME: Should we check something here before returning?
    return TRUE;
}

/****

Cluster all spliced-stars together with their source or sink.

@Description
// The following function an unfortunate hack needed because in the CG
// domains, type-conversion and LinToCirc/CircToLin stars get spliced in
// BEFORE the schedule is computed.  We perform a clustering step here to
// ensure that these stars stay together with their source/sink so that
// they end up in the innermost loops of the looped schedule.  It turns out
// that clustering two actors A(a)--->(b)B(b) where a=b does not harm the
// objective of buffer minimization.  Since type conversion and LinToCirc/
// CircToLin stars are like B, this preprocessing step will work for any
// spliced star currently used.  Hence, the function below clusters together
// actors like B (that produce and consume the same amount as the source
// or sink that B is connected to) with their source/sink and does not need
// to explicitly check for spliced-in stars (since they are like B).

@Returns Int: TRUE or FALSE depending on whether the clustering was successfull.

@SideEffects gr gets modified because that is the point of the clustering.

****/
int AcyLoopScheduler::clusterSplicedStars(AcyCluster* gr)
{
    int sdfParam, flag;
    AcyCluster *c, *b, *nCTBG;
    AcyClusterIter nextClust(*gr);
    SynDFClusterPort *p, *bp;
    SequentialList clustersToBeGrouped;
    while ((c=nextClust++) != NULL) {
	clustersToBeGrouped.initialize();
	SynDFClusterPortIter nextPort(*c);
	while ((p=nextPort++) != NULL) {
	    flag=1;
	    if (!p->far()) continue;
	    if ((sdfParam=p->numXfer()) == p->far()->numXfer()) {
		b = (AcyCluster*)p->far()->parent();
		if (!b) continue;
		SynDFClusterPortIter nextPortb(*b);
		while ((bp = nextPortb++) != NULL) {
		    if (sdfParam != bp->numXfer()) {flag = 0; break;}
		}
		if (flag) {
		    clustersToBeGrouped.append(b);
		}
	    }
	}
	// Now we have determined all of c's neighbors that have the
	// same rate as c; we can start clustering them together.
	if (clustersToBeGrouped.size() > 0) {
	    ListIter nextNeigh(clustersToBeGrouped);
	    while ((nCTBG = (AcyCluster*)nextNeigh++) != NULL) {
		if (!c->absorb(*nCTBG)) return FALSE;
	    }
	}
    }
    return TRUE;
}

/****

Recursive Partitioning by Minimum Cuts heuristic

@Description This function constructs a topological sort by recursively
creating cuts of the graph so that the amount of data crossing the cut
is minimized.  <a href="#ClassAcyLoopScheduler>See the  book refered to in
the comments for the class for more details.</a>

<a href="http://ptolemy.eecs.berkeley.edu/papers/PganRpmcDppo/">
Also see the first paper at this site.</a>

@Returns Int: buffering cost of the entire schedule as determined by RPMC.  If
it is -1, then something went wrong and schedule was not computed.

@SideEffects Fills the RPMCTopSort array by the node numbers according to
their lexical order in the schedule.  Also modifies gr; at the termination
of this function, gr will have only two clusters in it.

****/
int AcyLoopScheduler::RPMC(AcyCluster* gr)
{
    int cost = 0, leftCost=0, rightCost=0, totalCost=0;
    int N, lflag=0, rflag=0;
    AcyCluster *c, *leftSubGraph=0,*rightSubGraph=0, *newL=0, *newR=0;
    SequentialList leftGroup, rightGroup;

    // Call graph splitter if number of nodes is more than 1.
    if ((N = gr->totalNumberOfBlocks()) > 1) {
	if ( (cost=gr->legalCutIntoBddSets(3*N/4)) == -1) {
	    // Means that a bounded cut was not found; call again
	    // relaxing the bound.  Note that the splitter returns
	    // -1 iff a cut respecting the bound was not found.
	    cost = gr->legalCutIntoBddSets(N);
	}
	if (cost == -1) {
	    // Something is wrong.  We have this check for debugging
	    // since normally the graph splitter should never return -1
	    // if it can find a bounded cut, and the code above takes
	    // care of that.
	    Error::abortRun("A bug detected in legalCutIntoBddSets");
	    return cost;
	}
	// Now build up the right and left subgraphs.
	AcyClusterIter nextClust(*gr);
	while ((c = nextClust++) != NULL && lflag+rflag < 2) {
	    // first start with some cluster in the left partition
	    if (c->PARTITION == c->parent()->PARTITION && lflag == 0) {
		leftSubGraph = c;
		lflag = 1;
		continue;
	    }
	    if (c->PARTITION != c->parent()->PARTITION && rflag == 0) {
		rightSubGraph = c;
		rflag = 1;
	    }
	}
	// Now we start clustering using leftSubGraph and rightSubGraph
	// clusters as starting points.
	nextClust.reset();
	while ((c = nextClust++) != NULL) {
	    if (c == leftSubGraph || c == rightSubGraph) continue;
	    if (c->PARTITION == c->parent()->PARTITION) {
		leftGroup.append(c);
	    } else {
		rightGroup.append(c);
	    }
	}
	if (leftGroup.size() > 0) {
	    ListIter nextLeft(leftGroup);
	    c = (AcyCluster*)nextLeft++;
	    newL = (AcyCluster*)gr->group(*leftSubGraph, *c);
	    if (!newL) return -1;
	    newL->PARTITION = leftSubGraph->PARTITION;
	    while ((c = (AcyCluster*)nextLeft++) != NULL) {
		if (!newL->absorb(*c)) return -1;
	    }
	} else newL = leftSubGraph;
	if (rightGroup.size() > 0) {
	    ListIter nextRight(rightGroup);
	    c = (AcyCluster*)nextRight++;
	    newR = (AcyCluster*)gr->group(*rightSubGraph, *c);
	    if (!newR) return -1;
	    newR->PARTITION = rightSubGraph->PARTITION;
	    while ((c = (AcyCluster*)nextRight++) != NULL) {
		if (!newR->absorb(*c)) return -1;
	    }
	} else newR = rightSubGraph;
	// Now call RPMC again on the left and right subgraphs.
	leftCost = RPMC(newL);
	rightCost = RPMC(newR);
	if (leftCost < 0 || rightCost < 0) return -1;
	totalCost = leftCost + cost + rightCost;
    } else {
	// Else, there is only one node and its PARTITION value the same as
	// it ever was.  So add it's PARTITION value to top sort.
	// NOTE!!! Here we assume that the numbering produced by 
	// numberBlocks() that was called
	// earlier is still valid.
	
	RPMCTopSort[gr->PARTITION] = gr->head()->NODE_NUM;
    }
    return totalCost;
}

/****

Acyclic Pairwise Grouping of Adjacent nodes heuristic

@Description This algorithm builds up a schedule by repeatedly clustering
pairs of actors according to a particular cost function.
<a href="#ClassAcyLoopScheduler>See the  book refered to in
the comments for the class for more details.</a>

<a href="http://ptolemy.eecs.berkeley.edu/papers/PganRpmcDppo/">
Also see the second paper at this site.</a>

@Returns Int: either postive or -1; -1 means that something went wrong and
schedule was not computed.  Positive integer should equal graphSize+1 if
schedule was computed successfully.

@SideEffects gr gets modified due to clustering operations.  reachMatrix,
edgelist are also modified.

****/
int AcyLoopScheduler::APGAN(AcyCluster* gr)
{
    // APGAN heuristic.  In this heuristic, we build up a cluster hierarchy
    // by clustering pairs of actors together at each step.  The pair chosen
    // for clustering should a) not introduce any cycles in the graph,
    // and b) maximizes the gcd of the repetitions of the two actors
    // over all pairs that can be clustered together without introducing
    // cycles.  See chapter 7 of the book
    // "Software Synthesis from Dataflow graphs" for more details.
    // Also note that for APGAN, we do not need a pre-pass to
    // cluster the spliced-in stars with their sources/sinks since
    // the APGAN algorithm, by its very nature, will keep spliced actors
    // together with their source/sink in the same innermost loop always.
    int N, rho, flag, tmp, i;
    SynDFClusterPort *p, *clusterEdge=0;
    AcyCluster *src, *snk, *clusterSrc=0, *clusterSnk=0, *omega, *superOmega;
    createEdgelist(gr);
    while ((N=gr->numberBlocks()) > 1) {
	// explore each edge in the graph.
	rho = 1;
	ListIter nextEdge(edgelist);
	while ((p=(SynDFClusterPort*)nextEdge++) != NULL) {
	    if (p->DELETE) {
		nextEdge.remove();
		continue;
	    }
	    src = (AcyCluster*)p->parent();
	    snk = (AcyCluster*)p->far()->parent();
	    // first check whether this arc is part of a multiarc that
	    // has already been clustered previously.
	    if (reachMatrix->m[src->NODE_NUM][snk->NODE_NUM] == -1) {
		nextEdge.remove();
		continue;
	    }
	    // Check whether clustering these will create a cycle.
	    // It will create a cycle if some node that is reachable from
	    // src can reach snk.
	    flag = 1;
	    for (i=0; i<graphSize; i++) {
		if (reachMatrix->m[src->NODE_NUM][i] == 1 &&
		    reachMatrix->m[i][snk->NODE_NUM] == 1) {flag=0; break;}
	    }
	    if (flag) {
		tmp = gcd(src->loopFactor(), snk->loopFactor());
		rho = max(rho, tmp);
		if (rho == tmp) {
		    clusterSrc = src;
		    clusterSnk = snk;
		    clusterEdge = p;
		}
	    }
	}
	// cluster clusterSrc and clusterSnk.  Also update the reachability
	// matrix and the edge list.  The edgelist is updated next time around;
	// now it is simply marked to be deleted.
	omega = (AcyCluster*)gr->group(*clusterSrc, *clusterSnk);
	omega->NODE_NUM = clusterSrc->NODE_NUM;
	// the following two statements are to make it easy to determine
	// which way the arc is going between clusterSrc and clusterSnk.
	// Even though it is obvious here, it won't be obvious when we
	// retrieve the clusters from omega using ClusterIter.  Then, rather
	// than physically checking which direction the arcs are going, we
	// can just look at TMP_PARTITION
	clusterSrc->TMP_PARTITION = 0;
	clusterSnk->TMP_PARTITION = 1;
	// mark this edge (that we just clustered) for deletion
	clusterEdge->DELETE = 1;
	// mark all the output ports from clusterSrc and clusterSnk
	// for deletion as well.  Finally, add all of the output ports
	// of omega to the list.
	SynDFClusterOutputIter nextO1(*clusterSrc);
	while ((p=nextO1++) != NULL) p->DELETE = 1;
	SynDFClusterOutputIter nextO2(*clusterSnk);
	while ((p=nextO2++) != NULL) p->DELETE = 1;
	SynDFClusterOutputIter nextO3(*omega);
	while ((p=nextO3++) != NULL) edgelist.append(p);
	// Update the reachability matrix.  The rows and columns corresponding
	// to clusterSnk are all marked -1 so that we can detect that
	// clusterSnk is not in the clustered graph anymore.  clusterSrc
	// becomes the new clustered node so its rows (and columns) are updated
	// as the OR of the rows (and columns) of the previous clusterSrc
	// and clusterSnk.
	for (i=0; i<graphSize; i++) {
	    reachMatrix->m[clusterSrc->NODE_NUM][i] = 
	       (reachMatrix->m[clusterSrc->NODE_NUM][i]==1) |
	       (reachMatrix->m[clusterSnk->NODE_NUM][i]==1);
	    reachMatrix->m[i][clusterSrc->NODE_NUM] =
	       (reachMatrix->m[i][clusterSrc->NODE_NUM]==1) |
	       (reachMatrix->m[i][clusterSnk->NODE_NUM]==1);
	       // The above update can cause self-loops to occur
	       // in the reachability matrix since if i=clusterSrc->NODE_NUM,
	       // then that entry will be 1 by the above test; hence, we
	       // set it to 0 below.
	    reachMatrix->m[i][i] = 0;
	}
	for (i=0; i<graphSize; i++) {
	    reachMatrix->m[clusterSnk->NODE_NUM][i] = -1;
	    reachMatrix->m[i][clusterSnk->NODE_NUM] = -1;
	}
    }
    // graph has been completely clustered now.  Build up the schedule
    // tree by traversing the clustering hierarchy backwards.
    // First check that there is only one node in gr.
    if (gr->numberBlocks() != 1) {
	// This means there is a bug either in the implementation above,
	// or in the clustering routines that it calls.
	Error::abortRun("Bug in APGAN implementation; didn't cluster properly.");
	return -1;
    }
    superOmega = (AcyCluster*)gr->head();
    return buildAPGANTopsort(superOmega, 0);
}

/****

Builds up the topological sort by traversing the APGAN cluster hierarchy

@Description Traverse the cluster hierarchy to determine the order in which
nodes should be scheduled.

@Returns Int: -1 if error, graphSize+1 if success.

@SideEffects APGANTopSort gets filled with node numbers.

****/
int AcyLoopScheduler::buildAPGANTopsort(AcyCluster* gr, int ti)
{
    AcyCluster *tmp,*leftOmega, *rightOmega;
    int lt,rt;
    if (gr->numberBlocks() > 2) {
	StringList message = "Error in APGAN function in AcyLoopScheduler.";
	message << "Clustering process did not cluster properly.";
	Error::abortRun(message);
	return -1;
    }
    if (gr->numberBlocks() == 1) {
	// Atomic block; hence, a leaf node in the schedule tree.  We can
	// label its PARTITION flag by its index in the toplogical sort.
	gr->head()->PARTITION = ti;
	APGANTopSort[ti++] = gr->head()->NODE_NUM;
	return ti;
    } else {
	AcyClusterIter nextClust(*gr);
	tmp = nextClust++;
	if (tmp->TMP_PARTITION) {
	    rightOmega = tmp;
	    leftOmega = nextClust++;
	} else {
	    leftOmega = tmp;
	    rightOmega = nextClust++;
	}
	lt=buildAPGANTopsort(leftOmega,ti);
	rt=buildAPGANTopsort(rightOmega,lt);
	return rt;
    }
}

/****

Dynamic Programming Post Optimization

@Description Take a topological sort and create an optimal looping hierarchy.
Uses 3 matrices to store data.

See
<a href="ptolemy.eecs.berkeley.edu/papers/chainmemman.ps.Z">the Icassp 94 paper
</a> for details.

<a href="#ClassAcyLoopScheduler>Also see the  book refered to in
the comments for the class.</a>

<a href="http://ptolemy.eecs.berkeley.edu/papers/PganRpmcDppo/">
Finally, the vlsi signal processing paper at this site has even more on this
algorithm.</a>

@Returns Int: optimal buffering cost for the nested schedule, -1 if error.

@SideEffects creates costMatrix, splitMatrix, gcdMatrix, 3 class data members.

****/
int AcyLoopScheduler::DPPO()
{
    // We use the incidence matrix representation for this
    // algorithm.  Reason: When we compute costs, we need to be able to tell
    // quickly if there is an arc between two arbitrary nodes A and B.  Without
    // the incidence matrix representation, we have to search all of the output
    // ports of A to see if any of them is connected to B.  Needless to say,
    // this is grossly inefficient compared to checking whether the (A,B)th
    // entry is 0 or not in the incidence matrix.

    int cur, left, bottom, btmlft;
    int costij, cst, costTmp;
    int tmpInc1, tmpInc2, tmpDel1, tmpDel2;
    int i, ke, g;
    int N2 = graphSize*graphSize;
    int N3 = N2*graphSize;
    int* costs = new int[N3];
    for (int j=0; j< N3; j++) costs[j] = 0;
    int* reps = new int[graphSize];

    delete costMatrix;
    delete splitMatrix;
    delete gcdMatrix;
    costMatrix = new Matrix(graphSize,graphSize);
    splitMatrix = new Matrix(graphSize,graphSize);
    gcdMatrix = new Matrix(graphSize,graphSize);
    for (j=0; j<graphSize; j++) {
	gcdMatrix->m[j][j] = nodelist[topSort[j]]->reps();
	reps[j] = gcdMatrix->m[j][j];
    }
    for (j = 0; j < graphSize-1; j++) {
	for (i = 0; i < graphSize-1-j; i++) {
	    ke = i + j + 1;
	    costMatrix->m[i][ke] = (int)HUGE_VAL;
	    costTmp = (int)HUGE_VAL;
	    g = gcd(gcdMatrix->m[i][ke-1], reps[ke]);
	    gcdMatrix->m[i][ke] = g;
	    // Define these indices here to avoid computing in the
	    // inner loop
	    cur = N2*i + graphSize*ke;
	    left = cur - graphSize;
	    bottom = cur + N2;
	    btmlft = bottom - graphSize;
	    costij=0;
	    tmpInc1 = incMatrix->m[topSort[i]][topSort[ke]];
	    tmpInc2 = incMatrix->m[topSort[ke]][topSort[i]];
	    tmpDel1 = delMatrix->m[topSort[i]][topSort[ke]];
	    tmpDel2 = delMatrix->m[topSort[ke]][topSort[i]];
	    if ( tmpInc1 > 0) {
		costij = reps[i]*tmpInc1/g + tmpDel1;
	    }
	    if (tmpInc2 > 0) {
		// First check if the number of delays are sufficiently high
		// since this is a feedback arc
		if (tmpDel2 < tmpInc2*reps[ke]) {
		    // FIXME: Improve the foll. error message.
		    StringList message;
		    message << "Topological sort constructed by RPMC or APGAN"
		            << "has reverse arcs with insufficient delay."
			    << "Either that or there is a bug in the DPPO"
			    << "function where this message is being generated";
		    Error::abortRun(message);
		    return -1;
		} else {
		    costij = tmpDel2;
		}
	    }
	    for (int k = i; k<ke; k++) {
		cst = costs[left+ k] * gcdMatrix->m[i][ke-1];
		cst += costs[bottom + k] * gcdMatrix->m[i+1][ke];
		cst -= costs[btmlft + k] * gcdMatrix->m[i+1][ke-1];
		cst = cst/g;
		cst += costij;
		costs[cur+k] = cst;
		costMatrix->m[i][ke] = min(costMatrix->m[i][ke],
			costMatrix->m[i][k]+ costMatrix->m[k+1][ke]+cst);
		if (costMatrix->m[i][ke] != costTmp) splitMatrix->m[i][ke] = k;
		costTmp = costMatrix->m[i][ke];
	    }
	}
    }
    delete [] reps;
    delete [] costs;
    return costMatrix->m[0][graphSize-1];
}

/****

Recursive function to establish buffer sizes.

@Description Establish the buffer size for all geodesics according to the
schedule computed by <code>computeSchedule</code>.

@SideEffects Geodesics bufferSize is changed.

****/
void AcyLoopScheduler::fixBufferSizes(int i, int j) {
 
    // function that establishes the maximum buffer size for
    // each arc based on the schedule computed by RPMC+DPPO
    // or APGAN+DPPO
    int N = j-i+1;
    int g_ij = gcdMatrix->m[i][j];
    int split, repsij;
    DataFlowStar *s, *t;
    DFPortHole *p;
    if (N > 1) {
	split = splitMatrix->m[i][j];
	// fix buffer sizes for all arcs crossing this split
	for (int k=i; k<=split; k++) {
	    s = nodelist[topSort[k]];
	    repsij = (s->reps())/g_ij;
	    DFStarPortIter nextPort(*s);
	    while ((p=nextPort++) != NULL) {
		t = (DataFlowStar*)p->far()->parent();
		if(t && topInvSort[t->NODE_NUM] > split
		     && topInvSort[t->NODE_NUM] <= j) {
		    if (p->geo())
		    p->geo()->setMaxArcCount(repsij*(p->numXfer()));
		}
	    }
	}
	fixBufferSizes(i,split);
	fixBufferSizes(split+1,j);
    }
}

///// Run methods ////
///// runOnce is called from SDFScheduler::run ////
///// AcyLoopScheduler does not need to redefine run(); SDFScheduler::run
///// is used.

// this is called from SDFScheduler::run
void AcyLoopScheduler::runOnce()
{
    runOnce(0,graphSize-1,1);
}

/****

Special runOnce to run the schedule determined by this scheduler.

@Description Recursively traverse the splitMatrix and run the stars according
to the schedule.

****/
void AcyLoopScheduler::runOnce(int i, int j, int g)
{
    int N = j-i+1;
    int g_ij = gcdMatrix->m[i][j];
    int loopFac = g_ij/g;
    int split;
    DataFlowStar* s;
    if (N==1) {
	s = nodelist[topSort[i]];
	for (int k=0; k<loopFac; k++) {
	    invalid = !s->run();
	    if (invalid) break;
	}
	return;
    } else {
	split = splitMatrix->m[i][j];
	for (int k=0; k<loopFac; k++) {
	    runOnce(i,split,g_ij);
	    runOnce(split+1,j,g_ij);
	}
	return;
    }
}

// The compileRun method is used by code-generation targets
void AcyLoopScheduler::compileRun()
{
    Target& targ = target();
    genCode(targ,1,0,graphSize-1,1);
}

// Similar to runOnce, except <code>Target::writeFiring</code> called (not run)
void AcyLoopScheduler::genCode(Target& t, int depth, int i, int j, int g)
{
    int N = j-i+1;
    int g_ij = gcdMatrix->m[i][j];
    int loopFac = g_ij/g;
    int split;
    DataFlowStar *s;
    if (N==1) {
	s = nodelist[topSort[i]];
	if (loopFac > 1) t.beginIteration(loopFac,depth);
	t.writeFiring(*s, depth+1);
	if (loopFac > 1) t.endIteration(loopFac, depth);
    } else {
	split = splitMatrix->m[i][j];
	if (loopFac > 1) t.beginIteration(loopFac,depth);
	genCode(t, depth+1, i, split, g_ij);
	genCode(t, depth+1, split+1, j, g_ij);
	if (loopFac > 1) t.endIteration(loopFac,depth);
    }
}


///////  Display Schedule routines.                      ///////
///////  These methods are called when display-schedule  ///////
///////  is called in pigi or the "schedule" command is  ///////
///////  used in ptcl.

// Display schedule in standard format; called by "schedule" command (pigi,ptcl)
StringList AcyLoopScheduler::displaySchedule()
{
    StringList out;
    out << "{\n";
    out << "{ scheduler \"Murthy and Bhattacharyya's SDF Loop scheduler\" }\n";
    out << dispNestedSchedules(0,0,graphSize-1,1);
    out << "}\n";
    return out;
}

// called by displaySchedule; this one actually does the work.
StringList AcyLoopScheduler::dispNestedSchedules(int depth,int i,int j,int g)
{
    StringList sch;
    int split;
    int N = j-i+1;
    int g_ij = gcdMatrix->m[i][j];
    int loopFac = g_ij/g;
    if (N==1) {
	sch << tab(depth);
	if (loopFac > 1) {
	    sch << "{ repeat " << loopFac << " {\n" << tab(depth+1);
	}
	sch << "{ fire " << nodelist[topSort[i]]->name() << " }\n";
	if (loopFac > 1) {
	    sch << tab(depth) << "}\n";
	}
    } else {
	split = splitMatrix->m[i][j];
	if (loopFac > 1) {
	    sch << tab(depth) << "{ repeat " << loopFac << " {\n";
	    depth++;
	}
	sch += dispNestedSchedules(depth, i, split, g_ij);
	sch += dispNestedSchedules(depth, split+1, j, g_ij);
	if (loopFac > 1) {
	    depth--;
	    sch << tab(depth) << "} }\n";
	}
    }
    return sch;
}

int isAcyclic(Galaxy* g, int ignoreDelayTags)
{
    // Done by computing breath first search (bfs) on g.
    // passNum keeps track of the total number of passes through
    // the graph.  Each pass starts by doing bfs on the graph starting
    // from a node that has not been ever visited in previous passes.
    // During the bfs, we add nodes that have never been visited before
    // to the nodesTobeVisited list.  If we encounter a node that has been
    // visited already on this pass, then we have found a cycle and return
    // FALSE.  If we encounter a node that was visited in a previous pass,
    // we do nothing and continue the search since all output edges from
    // that node would have been explored on the previous pass.
    // The flags[flagLoc] flag is set to passNum whenever it is visited
    // for the first time.

    // If ignoreDelayTags is 1, then every arc is taken into account.
    // if ignoreDelayTags is 0, then arcs that have been tagged (at
    // location flags[ignoreDelayTags-1]) non-zero are ignored as precedence
    // arcs.

    int flagLoc = 0, passNum=0;
    Block *node, *succ, *nextNode;
    resetFlags(*g,flagLoc); // reset flags[flagLoc] for all blocks
    GalTopBlockIter nextBlock(*g);
    SequentialList nodesTobeVisited;
    while ( (node=nextBlock++) != NULL && !node->flags[flagLoc]) {
	nodesTobeVisited.append(node);
	passNum++;
	while ((nextNode = (Block*)nodesTobeVisited.getAndRemove()) != NULL ) {
	    nextNode->flags[flagLoc] = passNum;
	    SuccessorIter nextSucc(*nextNode);
	    if (ignoreDelayTags) {
		succ = nextSucc.next(ignoreDelayTags-1, TRUE);
	    } else {
		succ = nextSucc++;
	    }
	    while (succ != NULL){
		if (succ->flags[flagLoc] == 0) {
		    nodesTobeVisited.append(succ);
		} else {
		    if (succ->flags[flagLoc] == passNum) {
			return FALSE; // Cycle found; not acyclic
		    }
		}
		if (ignoreDelayTags) {
		    succ = nextSucc.next(ignoreDelayTags-1, TRUE);
		} else {
		    succ = nextSucc++;
		}
	    }
	}
    }
    return TRUE;
}

// move over to Galaxy later
void AcyLoopScheduler::isWellOrdered(Galaxy* g, SequentialList& topsort)
{
    // Returns a list containing ALL the nodes in the graph in topological
    // ordering if the graph is
    // well ordered.  This means that the
    // following are equivalent for this graph
    // a) there is only 1 topological sort
    // b) there is a hamiltonian path
    // c) there is a total precedence ordering rather than a partial one,
    // d) For any two nodes u,v, it is either the case that u->v is a path
    //     or v->u is a path

    // The algorithm is to repeatedly find sources, and return the incomplete
    // list if there is more than 1 source node at any stage.  If there is
    // only one source at any stage, we mark that node (meaning it is
    // "removed" from the graph) and find sources on the resulting
    // graph.  Worst case running time: O(N+E) where N=graphSize,
    // E is the number of edges.


    int flagLoc = 0, numPasses=0;
    resetFlags(*g,flagLoc); // reset flags[flagLoc] for all blocks
    SequentialList sources;
    findSources(g,flagLoc, sources);
    while (numPasses < graphSize) {
	if (sources.size() == 1) {
	    DataFlowStar* src = (DataFlowStar*)sources.getAndRemove();
	    topsort.append(src);
	    src->flags[flagLoc] = 1;
	    numPasses++;
	    findSources(g,flagLoc,sources,src);
	} else {
	    return;
	}
    }
}

void findSources(Galaxy* g, int flagLoc, SequentialList& sources, DataFlowStar* deletedNode)
{
    // A source node here is any node that has all of its input
    // nodes marked.  This method is used in order to simulate
    // the removal of nodes from the graph; a node is "removed"
    // by marking it at flags[flagLoc] (by the function that calls
    // this function).  If no node is marked, then this function will
    // return the sources in the graph.  If nodes are marked, then
    // the list returned by the function represents the sources
    // in the graph that would result from removing the marked nodes
    // and all their edges from the graph.
    // If deletedNode is specified, then the algorithm will assume that
    // the only node deleted from the graph (from the last time this function
    // was called ) was deletedNode and explore
    // only the edges going out of deletedNode.  This will make this computation
    // much more efficient when it is used in conjunction with isWellOrdered
    // because isWellOrdered deletes one node at a time and calls findSources
    // after each deletion.  IN THIS CASE NOTE THAT THE FUNCTION WILL ONLY
    // RETURN THOSE SOURCES FORMED BY THE DELETION OF deletedNode AND NOT
    // ALL SOURCES IN THE GRAPH.
    // To generalize this function, we could give a
    // list of deleted nodes (instead of just one) and only explore the edges
    // out of those from the list. 
    // Worst case running time: O(E) (when deletedNode is specified)
    
    Block *pred, *node;
    int notSource;
    if (deletedNode) {
	SuccessorIter nextNeigh(*deletedNode);
	while ((node = nextNeigh++) != NULL) {
	    notSource = 0;
	    PredecessorIter nextPred(*node);
	    while ((pred=nextPred++) != NULL) {
		if (!pred->flags[flagLoc]) {notSource = 1; break;}
	    }
	    if (!notSource) sources.append(node);
	}
    } else {
	GalTopBlockIter nextBlock(*g);
	while ((node = nextBlock++) != NULL && !node->flags[flagLoc]) {
	    notSource = 0;
	    PredecessorIter nextPred(*node);
	    while ((pred=nextPred++) != NULL) {
		if (!pred->flags[flagLoc]) {notSource = 1; break; }
	    }
	    if (!notSource) sources.append(node);
	}
    }
}

SequentialList findSinks(Galaxy* g, int flagLoc, DataFlowStar* deletedNode)
{
    // symmetric to findSources; see comments therein
    // FIXME: Make this like findSources above after everything's been debugged
    SequentialList sinks;
    Block *succ, *node;
    int notSink = 0;
    if (deletedNode) {
	PredecessorIter nextNeigh(*deletedNode);
	while ((node = nextNeigh++) != NULL) {
	    SuccessorIter nextSucc(*node);
	    while ((succ=nextSucc++) != NULL) {
		if (!succ->flags[flagLoc]) notSink = 1;
	    }
	    if (!notSink) sinks.append(node);
	}
    } else {
	GalTopBlockIter nextBlock(*g);
	while ((node=nextBlock++) != NULL && !node->flags[flagLoc]) {
	    SuccessorIter nextSucc(*node);
	    while ((succ=nextSucc++) != NULL) {
		if (!succ->flags[flagLoc]) notSink = 1;
	    }
	    if (!notSink) sinks.append(node);
	}
    }
    return sinks;
}
