static const char file_id[] = "AcyLoopScheduler.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1996-%Q% The Regents of the University of California.
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
#include "GraphUtils.h"
#include <minmax.h>
#include <iostream.h>
#include <limits.h>

// The following macros are used for Blocks
#define NODE_NUM_FLAG_LOC 0
#define TMP_PARTITION_FLAG_LOC 1
#define PARTITION_FLAG_LOC 2

#define PARTITION(pp) ((pp)->flags[PARTITION_FLAG_LOC])
#define TMP_PARTITION(pp) ((pp)->flags[TMP_PARTITION_FLAG_LOC])
#define NODE_NUM(pp) ((pp)->flags[NODE_NUM_FLAG_LOC])


// The following macros are used for Galaxies
#define GALAXY_PARTITION(gg) ((gg).flags[2])

/****

*************** Class SimpleIntMartrix methods ******************

****/

// Constructor
SimpleIntMatrix::SimpleIntMatrix (int nr, int nc) : nrows(nr), ncols(nc) {
    m = new int *[nr];
    for (int i=0; i<nr; i++) {
	m[i] = new int[nc];
	for (int j=0; j<nc; j++) m[i][j] = 0;
    }
}

// Destructor
SimpleIntMatrix :: ~SimpleIntMatrix() {
    for(int i=0;i<nrows;i++) delete [] m[i];
    delete [] m;
}

// Resize the matrix
void SimpleIntMatrix :: resize(int nr, int nc) {
    SimpleIntMatrix tmp(nr,nc);
    *this = tmp;
}
							  
// equality operator
SimpleIntMatrix& SimpleIntMatrix :: operator=(const SimpleIntMatrix& a) {
  for(int i=0; i<nrows; i++) delete [] m[i];
    delete [] m;
    m = new int *[nrows=a.nrows];
    ncols=a.ncols;
    for (i=0; i<nrows; i++) {
	m[i] = new int[ncols];
	for (int j=0; j<ncols; j++) m[i][j] = a.m[i][j];
    }
    return *this;
}

// Copy constructor
SimpleIntMatrix :: SimpleIntMatrix(const SimpleIntMatrix& a) {
    m = new int *[nrows=a.nrows];
    ncols=a.ncols;
    for(int i=0;i<nrows;i++) {
	m[i] = new int[ncols];
	for(int j=0;j<ncols;j++) m[i][j]=a.m[i][j];
    }
}

/****

*************** Class AcyLoopScheduler methods ******************

****/

// Constructor. Set all data member pointers to 0.
AcyLoopScheduler :: AcyLoopScheduler(const char* log) : logFile(log)
{
    // NOTE: topSort is not allocated as an array; it is just a pointer.
    // Hence, it is not deleted in the destructor.
    topSort = 0;
    topInvSort = 0;
    nodelist = 0;
    RPMCTopSort = 0;
    APGANTopSort = 0;
    // flag to indicate whether schedule was successfully computed.
    schedSuccess = 0;
}

// Destroy a bunch of matrices and arrays used by the scheduler
AcyLoopScheduler::~AcyLoopScheduler()
{
    delete [] topInvSort;
    delete [] nodelist;
    delete [] RPMCTopSort;
    delete [] APGANTopSort;
}

/****

Create a reachability matrix; it is required by the APGAN function

@Description 
The reachability matrix R is an NxN matrix, where N = #of nodes
in the SDF graph, such that R(i,j) = 1 iff there is a directed
path from node i to node j in the SDF graph.  R(i,j) = 0 otherwise.
<p>
We use a very simple algorithm to compute this matrix here; there
are probably faster algorithms around (notably, Strassen's algorithm
for multiplying 2 NxN matrices can be used to get an O(lg(N)*N^2.81)
time algorithm for computing this matrix; it is asymptotically
better than the O(N^3) algorithm used here).
<p>
For each node i, we do a depth-first search from i and set those
entries R(i,j) to 1 for which j gets visted during the search.
Starting from a source node, we will visit every edge and node in
the subgraph that can be reached from that source node.  For each
edge, there is a potential O(N) update step when the reachability
matrix is updated.  Hence, if V_i,E_i is the set of nodes and edges
visited from a source node i, the total time is O( (|E_i|+|V_i|)*N )
starting from that source node.  Since a node is not visited again if
it has been visited already, we will only visit all the subgraphs that
form a disjoint union of the graph.
Adding up over the source nodes just gives the total running time of
O(|E|*N+|V|*N) = O(N^3) in general.
<p>
However, most practical acyclic SDF graphs are sparse; hence, O(N^2)
is more realistic.
<p>
NOTE: This function assumes that the blocks inside the Galaxy have been
numbered at NODE_NUM_FLAG_LOC (=0 for this file).  It also assumes that
the galaxy is totally flat; i.e, there are only stars or atomic galaxies
(galaxies that have only one star) inside.

@SideEffects
Creates <code>reachMatrix</code> a class data member.
Also, uses <code>flags[TMP_PARTITION_FLAG_LOC]</code> for marking nodes.
<code>TMP_PARTITION_FLAG_LOC</code> = 1 in this file.

@Returns Int: TRUE or FALSE depending on success.

****/
int AcyLoopScheduler::createReachabilityMatrix(Galaxy& gal)
{
    Block *s;
    // reset flags[1] to 0 for all blocks in gal
    resetFlags(gal, TMP_PARTITION_FLAG_LOC, 0);
    graphSize = gal.numberBlocks();
    reachMatrix.resize(graphSize, graphSize);
    GalTopBlockIter nextBlock(gal);
    while ((s=nextBlock++) != NULL) {
	if (s->flags[TMP_PARTITION_FLAG_LOC]) continue;
	if (!visitSuccessors(s,1)) return FALSE;
    }
    return TRUE;
}

// Called by <code>createReachabilityMatrix</code>; this does the actual DFS.
int AcyLoopScheduler::visitSuccessors(Block* s, int flagLoc)
{
    Block *succ;
    int fl;
    s->flags[flagLoc] = 1;
    SuccessorIter nextSucc(*s);
    while ((succ=nextSucc++) != NULL) {
	fl = succ->flags[flagLoc];
	if (fl == 1) {
	    StringList message = "AcyLoopScheduler::visitSuccessors:\n";
	    message << "Graph is cyclic.  Aborting...";
	    Error::warn(message);
	    return FALSE;
	}
	if (fl == 0) {
	    if (!visitSuccessors(succ, flagLoc)) return FALSE;
	}
	// Now we have visited and updated the row in the reachability
	// matrix corresponding to succ.  Hence, s can visit anything that
	// succ can; we simply OR succ's row with s's.  However, if
	// we already know we can visit succ, then we do not have to
	// update since it would have happened before.
	if (!reachMatrix.m[NODE_NUM(s)][NODE_NUM(succ)]) {
	    for (int i = 0; i < graphSize; i++) {
		reachMatrix.m[NODE_NUM(s)][i] = reachMatrix.m[NODE_NUM(s)][i] |
			reachMatrix.m[NODE_NUM(succ)][i];
	    }
	    reachMatrix.m[NODE_NUM(s)][NODE_NUM(succ)] = 1;
	}
    }
    s->flags[flagLoc] = 2;
    return TRUE;
}


// following definition is for ports
#define DELETE flags[2]

/****

Create a list of edges in the graph

@SideEffects Initializes and appends SynDFClusterPort pointers to edgelist,
a class data member.

****/
void AcyLoopScheduler :: createEdgelist(Galaxy* gr)
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
Create an incidence matrix representation.  This representation
has the following form:  it is an NxN matrix Inc, where N is number
of nodes.  The (i,j)th element of the matrix represents the number
of tokens produced by node i on arc (i,j).
<p>
If Inc(i,j) > 0, then i produces Inc(i,j) tokens per firing on arc (i,j).
Del(i,j) is the number of delays on arc (i,j)
<p>
We do not need
the numbers consumed here since we already know the repetitions vector.
Also, since we can have a multi-graph, we collapse multiple
arcs into one arc here.  This is fine since this matrix is
used to compute buffering costs so an equivalent arc with the
correct numbers will be identical in cost to multiple arcs.
<p>
This function assumes that the galaxy consists of SynDFClusters connecting
to each other by SynDFClusterPorts that
had their numberTokens and delays set properly.

@SideEffects Creates incMatrix, delMatrix, two class data
members

*****/
void AcyLoopScheduler :: createIncidenceMatrix(Galaxy& gal)
{
    SynDFCluster *s, *t;
    SynDFClusterPort* clustPort;
    int numP, numD;
    graphSize = gal.numberBlocks();
    SynDFClusterIter nextClust(gal);
    incMatrix.resize(graphSize,graphSize);
    delMatrix.resize(graphSize,graphSize);

    while ( (s = nextClust++) != NULL ) {
	SynDFClusterPortIter nextPort(*s);
	while ((clustPort = nextPort++) != NULL) {
	    if (clustPort->isItInput()) continue;
	    if (!clustPort->far() || !clustPort->far()->parent()) continue;
	    t = (SynDFCluster*)clustPort->far()->parent();
	    numP = clustPort->numXfer();
	    numD = clustPort->numInitDelays();
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
	    incMatrix.m[NODE_NUM(s)][NODE_NUM(t)] += numP;
	    delMatrix.m[NODE_NUM(s)][NODE_NUM(t)] += numD;
	}
    }   
}

/****

Copy the flag value of the first cluster inside this cluster to its flags

@Description
The following function steps through each cluster inside gr and sets
flags[flagLocation] for that cluster by the value of flags[flagLocation]
for the head() of that cluster.  When all of the clusters in gr are
atomic; i.e, each has exactly one Star, this function's action simply
results in the flag value of the Star being propagated to its cluster
wrapper.  Hence, this function is usually called after a cluster hierarchy
has been set up using <code>Cluster::initializeForClustering</code>
so that things like node numbering can be kept consistent in the Cluster
hierarchy as well.

@SideEffects <code>flags[flagLocation]</code> for each cluster in gr changes

****/
void AcyLoopScheduler :: copyFlagsToClusters(Galaxy* gr, int flagLocation)
{
    // FIXME: Should we always check whether the Galaxy does in fact have
    // AcyClusters inside?
    AcyCluster* c;
    AcyClusterIter nextClust(*gr);
    while ((c=nextClust++) != NULL) {
	c->flags[flagLocation] = c->head()->flags[flagLocation];
    }
}

// Create the array of all the DataFlowStars in the galaxy.
// It is assumed here that gal consists of atomic clusters; the star
// is just the head() of each such cluster.
int AcyLoopScheduler :: createNodelist(Galaxy& gal)
{
    // Holds the nodes in galaxy as an array
    delete [] nodelist;
    nodelist = new DataFlowStar*[graphSize];
    ClusterIter nextClust(gal);
    Cluster* c;
    DataFlowStar* s;
    while ((c=nextClust++) != NULL) {
	if (c->numberBlocks() > 1 || !c->head()->isItAtomic()) {
	    StringList message = "AcyLoopScheduler::createNodelist:\n";
	    message << "Got galaxy with non-atomic clusters.\n";
	    Error::warn(message);
	    return FALSE;
	}
	s = (DataFlowStar*)(c->head());
	nodelist[NODE_NUM(c)] = s;
	NODE_NUM(s) = NODE_NUM(c);
    }
    return TRUE;
}

int AcyLoopScheduler :: computeBMLB()
{
    DataFlowStar *s, *t;
    DFPortHole* p;
    int BMLB = 0, eta;
    // iterate over all stars in nodelist
    for (int i=0; i<graphSize; i++) {
	s = nodelist[i];
	DFStarPortIter nextPort(*s);
	while ((p = nextPort++) != NULL) {
	    if (p->isItInput() || !p->far() || !p->far()->parent()) continue;
	    t = (DataFlowStar*)(p->far()->parent());
	    eta = (s->reps() * p->numXfer()) / gcd(s->reps(), t->reps());
	    if (p->numInitDelays() > eta) BMLB += p->numInitDelays();
	    else BMLB += eta;
	}
    }
    return BMLB;
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
    AcyCluster* clusterGraph;
    Block *d;
    int i = 0, rpmc, apgan, rpmcDppo=-1, apganDppo=-1;
    SequentialList wellOrderedList;
    StringList message = "AcyLoopScheduler::computeSchedule:\n";

    if (!isAcyclic(&gal,0)) {
	message << "Graph is not acyclic; this loop scheduler cannot be used. "
		<< "Set the looping level parameter in the target to DEF, "
		<< "CLUST, or SJS and rerun the universe.\n";
	Error::abortRun(message);
	return FALSE;
    }
    // log file stuff
    ostream* logstrm = 0;
    if (logFile && *logFile) {
	StringList tmp = logFile;
	tmp << ".acySchedLogs." << gal.name();
	logstrm = new pt_ofstream(tmp.chars());
	if (!*logstrm) {
	    delete logstrm;
	    return FALSE;
	}
    }
    if (logstrm) {
    	*logstrm << "Logs of AcyLoopScheduler computations for "
	    << gal.name() << ".\n";
    }

    // Prepare the galaxy for clustering; this flattens all internal galaxies
    // and creates a cluster wrapper around each star in the galaxy.
    AcyCluster dummy;
    dummy.initializeForClustering(gal);

    graphSize = gal.numberBlocks();

    // Add a top level cluster inside the galaxy to hold all the
    // clusters inside.  This is needed because gal is of type
    // galaxy and everything inside is of type cluster.  Since
    // RPMC works recursively from the top, it doesn't
    // make sense to have a dichotomy between the top level and everything
    // below it.

    if (!addTopLevelCluster(&gal)) return FALSE;

    clusterGraph = (AcyCluster*)(gal.head());
    clusterGraph->settnob(-1);
    PARTITION(clusterGraph) = 0;

    delete [] RPMCTopSort;
    RPMCTopSort = new int[graphSize];

    delete [] APGANTopSort;
    APGANTopSort = new int[graphSize];

    delete [] topInvSort;
    topInvSort = new int[graphSize];

    // Number the blocks in galaxy at flags[0].  NOTE: This will only
    // number the cluster wrappers, not the stars inside.
    numberBlocks(*clusterGraph,NODE_NUM_FLAG_LOC);

    // this function will also propgate the cluster numbering to the
    // star inside.
    if (!createNodelist(*clusterGraph)) return FALSE;

    // createIncidenceMatrix requires the above node numbering
    createIncidenceMatrix(*clusterGraph);

    if (isWellOrdered(clusterGraph, wellOrderedList)) {
	// Means the graph is well ordered; hence, just fill up the
	// topSort array and call DPPO on it.
	i = 0;
	ListIter nextStar(wellOrderedList);
	while ( (d=(Block*)nextStar++) != NULL) {
	    RPMCTopSort[i] = NODE_NUM(d);
	    i++;
	}
	topSort = RPMCTopSort;
	rpmcDppo = DPPO();
	if (logstrm) {
	    *logstrm << "\nGraph is well-ordered; only DPPO used.\n"
		 << "DPPO cost is: " << rpmcDppo << ".\n";
	}
	if (rpmcDppo < 0) {
	    message << "AcyLoopScheduler::DPPO failed on well-ordered graph.\n";
	    Error::abortRun(message);
	    return FALSE;
	}
    } else {

	SimpleIntMatrix RPMCGcdMatrix;
	SimpleIntMatrix RPMCSplitMatrix;

	// The complicated control structure below is designed to be
	// error tolerant.  In particular, even if one of RPMC or APGAN
	// fails, the scheduler won't give up; it will only give up
	// if both fail.

	// First of the two heuristics
	rpmc = callRPMC(clusterGraph);
	if (rpmc >= 0) {
            if (logstrm)
	    	*logstrm << "RPMC cost (w/out DPPO) is " << rpmc << ".\n";

	    rpmcDppo = DPPO();
	    if (rpmcDppo >= 0) {
		schedSuccess = 1;
	    	if (logstrm)
		    *logstrm << "RPMC+DPPO schedule:\n" << displaySchedule();

	    	// save results for rpmc
	    	RPMCGcdMatrix = gcdMatrix;
	    	RPMCSplitMatrix = splitMatrix;
		schedSuccess = 0;
	    } else {
		if (logstrm) *logstrm << "DPPO failed on RPMC schedule.\n";
	    }
	} else {
	    if (logstrm) *logstrm << "RPMC failed to produce schedule.\n";
	}

	// Second of the two heuristics
	apgan = callAPGAN(gal); 
	if (apgan >= 0) {
            if (logstrm)
		*logstrm << "APGAN cost (w/out DPPO) is " << apgan << ".\n";

	    apganDppo = DPPO();
	    if (apganDppo >= 0) {

		schedSuccess = 1;
		if (logstrm)
		    *logstrm << "APGAN+DPPO schedule:\n" << displaySchedule();

		schedSuccess = 0;
	    } else {
		if (logstrm) *logstrm << "DPPO failed on APGAN schedule.\n";
	    }
	} else {
	    if (logstrm) *logstrm << "APGAN failed to produce schedule.\n";
	}

	// check if either heuristic ran to completion.
	// Here, RPMC ran to completition if DPPO also ran on it;
	// same for APGAN.
	if (apganDppo < 0 && rpmcDppo < 0) {
	    message << "Both APGAN and RPMC failed to produce valid "
		    << "schedules due to previous errors.  Aborting...\n";
	    Error::abortRun(message);
	    return FALSE;
	}
	// 3 cases left now:
	// Only RPMC failed ==> do nothing; all data-structures have 
	// 			APGAN results already.
	// Only APGAN failed ==> All data structures should hold previously
	//			 computed RPMC results
	// neither failed ==> if rpmcDppo is lower, then above, else nothing.
	if ( (apganDppo < 0) || ((rpmcDppo >= 0) && (rpmcDppo < apganDppo))) {
	    // use RPMC results
	    gcdMatrix = RPMCGcdMatrix;
	    splitMatrix = RPMCSplitMatrix;
	    topSort = RPMCTopSort;
	}
	if (logstrm) {
	    *logstrm << "\nSummary:\n";
	    *logstrm << "\n" << "RPMC + DPPO cost is " << rpmcDppo << ".\n";
	    *logstrm << "APGAN + DPPO cost is " << apganDppo << ".\n";
	    *logstrm << "The BMLB for the graph is " << computeBMLB() << ".\n";
	}
    }

    // build inverse topSort array, topInvSort.  This array gives
    // the position in topSort for a node i.  So 
    // topSort[topInvSort[i]] == topInvSort[topSort[i]] == i

    for (i=0; i<graphSize; i++) topInvSort[topSort[i]] = i;

    // finally, set all the buffer sizes in accordance with the schedule.
    if (!fixBufferSizes(0,graphSize-1)) {
	message << "Failed to set buffer sizes after "
		<< "schedule computation.  Aborting...\n";
	Error::abortRun(message);
	return FALSE;
    }

    if (logstrm) logstrm->flush();

    delete logstrm;
    // Temporary hack to let other methods know that schedule was
    // successfully computed.  This info. is needed under the present
    // architecture by the displaySchedule routine since that can be
    // called even though a schedule wasn't computed.
    schedSuccess = 1;

    return TRUE;
}

// This function just sets up executes everything needed to run RPMC.
int AcyLoopScheduler :: callRPMC(AcyCluster* clusterGraph)
{
    int rpmc;

    // The following function is a hack needed by RPMC; see its
    // comments for more information.
    if (!clusterSplicedStars(clusterGraph)) return -1;


    // topSort should point to RPMCTopSort
    topSort = RPMCTopSort;

    // call RPMC
    rpmc = RPMC(clusterGraph);
    if (rpmc < 0) return -1;

    // now build up the topSort array
    if (!buildTopsort(clusterGraph,0)) return -1;
    //check if topSort is valid
    if (!checkTopsort()) return -1;

    return rpmc;
}

// This function just sets up and executes everything needed to run APGAN 
int AcyLoopScheduler :: callAPGAN(Galaxy& gal)
{
    int apgan;
    AcyCluster *superOmega, *clusterGraph;

    // Flatten everything
    AcyCluster dummy;
    dummy.initializeForClustering(gal);

    // Even though APGAN is not recursive, and does not require a
    // cluster as an argument, we add a top level cluster for 2 reasons:
    // 1) It is safe because we cannot mistakenly cross wormhole boundaries,
    // 2) It makes this code symmetric to callRPMC.

    if (!addTopLevelCluster(&gal)) return -1;

    clusterGraph = (AcyCluster*)(gal.head());
    clusterGraph->settnob(-1);
    PARTITION(clusterGraph) = 0;
    
    // APGAN requires the node numbers in the clusters as well; hence,
    // copy those numbers to the constituent clusters.  RPMC did not
    // want this so wasn't done before.
    copyFlagsToClusters(clusterGraph,0);

    // Reachability matrix is needed by APGAN.  Note that copyFlagsToClusters
    // should be invoked before this since the following function depends
    // on the clusters having the node numbers.
    if (!createReachabilityMatrix(*clusterGraph)) return -1;

    topSort = APGANTopSort;
    apgan = APGAN(clusterGraph);
    if (apgan < 0) return -1;

    // Since apgan >= 0, clusterGraph has just one cluster when APGAN finishes.
    superOmega = (AcyCluster*)(clusterGraph->head());
    if (!buildTopsort(superOmega, 0)) return -1;

    if (!checkTopsort()) return -1;
    return apgan;
}

/****
This function checks whether a topological sort is valid or not.

@Description
Does two things: verifies whether the elements of topSort are a permutation
of the integers in [0,graphSize-1], and verifies whether the elements respect
the precedence constraints (ignoring arcs that have sufficient delays)

@SideEffects fills up topInvSort

@Returns Int: TRUE or FALSE

****/
int AcyLoopScheduler :: checkTopsort()
{
    // First check that every element of topSort is unique and is
    // less than graphSize.
    DataFlowStar *nodei, *pn;
    DFPortHole *p, *q;
    int flag=0;
    StringList message = "AcyLoopScheduler::checkTopsort:\n";
    for (int i=0; i<graphSize; i++) topInvSort[i] = -1;
    for (i = 0; i < graphSize; i++) {
	if ( (topSort[i] >= 0) && (topSort[i] < graphSize) &&
	    (topInvSort[topSort[i]] == -1) ) {
	    topInvSort[topSort[i]] = i;
	} else {
	    message << "AcyLoopScheduler::topSort is not valid.\n"
		    << "There could be several reasons for this: "
		    << "the node numbering might be wrong, RPMC or "
		    << "APGAN constructed an invalid topSort, or other "
		    << "reasons.\n";
	    Error :: warn(message);
	    return FALSE;
	}
    }
    for (i = 0; i < graphSize; i++) {
	// for each topSort[i], verify that all of i's predecessors appear
	// before i in topSort
	flag = 0;
	nodei = nodelist[topSort[i]];
	DFStarPortIter nextPort(*nodei);
	while ((p = nextPort++) != NULL) {
	    if (p->isItOutput()) continue;
	    if (!p->far() || !p->far()->parent()) continue;
	    pn = (DataFlowStar*)(p->far()->parent());
	    if (topInvSort[NODE_NUM(pn)] > i) {
	   	// verify that every arc between pn and nodei has
		// enough delays.
		DFStarPortIter nextO(*pn);
		while ((q = nextO++) != NULL) {
		    if (q->far() && q->far()->parent() == nodei) {
			if (q->numInitDelays() < q->far()->numXfer() * 
				nodei->reps()) {flag = 1; break;}
		    }
		}
	    }
	}
        if (flag) break;
    }
    if (flag) {
	message << "AcyLoopScheduler::topSort array is invalid; precedence "
		<< "constraints violated.\n";
	Error::warn(message);
	return FALSE;
    }
    return TRUE;
}


/****
Add a top level cluster under the galaxy.  So gal will have just one cluster.

@Description
We do this by absorbing all of the clusters into one.
This function is essentially a hack to get around
a vexing problem with teh clustering process that leaves the
topmost level structure as a Galaxy instead of a Cluster.

@Returns Int: 0 if unsuccessful , 1 if successful.

@SideEffects gal will only have one block, a cluster containing everything
else inside it.

****/
int AcyLoopScheduler::addTopLevelCluster(Galaxy* gal)
{
    AcyCluster* c, *b, *topC;
    StringList message = "AcyLoopScheduler::addTopLevelCluster:\n";
    if ((gal->head())->isItCluster()) {
	AcyClusterIter nextClust(*gal);
	c = nextClust++;
	// first check whether galaxy has only one star.
	if (gal->numberBlocks() == 1) {
	    // need to create a new top level cluster and absorb c into that.
	    topC = new AcyCluster;
	    topC->setName(gal->name());
	    // add topC to gal's list
	    gal->addBlock(*topC, topC->name());
	    // absorb c into topC and remove c from gal (done by absorb)
	    if (!topC->absorb(*c)) {
		gal->removeBlock(*topC);
		message << "Failed to absorb c into newly created empty cluster";
		Error::warn(message);
		return FALSE;
	    }
	    return TRUE;
	} else {
	    while ((b=nextClust++) != NULL) {
	        if (b==c) continue;
	        if (!c->absorb(*b,0)) return FALSE;
	    }
	    cleanupAfterCluster(*gal);
	    if (gal->numberBlocks() != 1) {
	        message << "Galaxy has more than one block after "
		        << "clustering process.\n";
	        Error::warn(message);
	        return FALSE;
	    }
	    // Change c's name to gal's name.
	    c->setName(gal->name());
	    return TRUE;
	}
    } else {
	message << "Galaxy has not been converted to cluster hierarchy "
		<< "by calling Cluster::initializeForClustering first.\n"
		<< "OR There's a bug in addTopLevelCluster since it thinks "
		<< "that the members of the argument "
		<< "galaxy are not Clusters.\n";
	Error::warn(message);
	return FALSE;
   }
}


/****

Cluster all spliced-stars together with their source or sink.

@Description
The following function an unfortunate hack needed because in the CG
domains, type-conversion and LinToCirc/CircToLin stars get spliced in
BEFORE the schedule is computed.  We perform a clustering step here to
ensure that these stars stay together with their source/sink so that
they end up in the innermost loops of the looped schedule.  It turns out
that clustering two actors A(a)--->(b)B(b) where a=b does not harm the
objective of buffer minimization.  Since type conversion and LinToCirc/
CircToLin stars are like B, this preprocessing step will work for any
spliced star currently used.  Hence, the function below clusters together
actors like B (that produce and consume the same amount as the source
or sink that B is connected to) with their source/sink.
<p>
However, some care needs to be exercised since there may be non-spliced
in stars that are "like B" in that there is no rate change across the
arc and node.  Clustering all these stars together could lead to cycles
in the graph.  Hence, the following rule is used.  
<p>
Let (A,B) be an arc and suppose that A produces the same amount as B consumes
on this arc.  We cluster A and B together iff either of these conditions
hold:
a) B has only one input arc and on all its output arcs it produces the same
	number as it consumes.
b) A has only output arc and on all of its input arcs, it produces the same
	number as it consumes.
<p>
As we traverse the graph one node at a time and examine that nodes neighbors,
if the node under examination is either A or B, and B is a spliced-in star,
then condition a) is satisfied.  If A is the spliced-in star, then b) is
satisfied.

@Returns Int: TRUE or FALSE depending on whether the clustering was successful

@SideEffects gr gets modified because that is the point of the clustering.

****/
int AcyLoopScheduler::clusterSplicedStars(AcyCluster* gr)
{
    int sdfParam, flag;
    AcyCluster *c, *b;
    SynDFClusterPort *p, *bp, *cp;
    AcyClusterIter nextClust(*gr);
    while ((c=nextClust++) != NULL) {
	SynDFClusterOutputIter nextPort(*c);
	while ((p=nextPort++) != NULL) {
	    flag = 1;
	    if (!p->far()) continue;
	    b = (AcyCluster*)p->far()->parent();
	    if (!b) continue;
	    if ((sdfParam=p->numXfer()) == p->far()->numXfer()) {

		// b is a candidate for being clustered into the
		// source of this edge; hence, it should either have just
		// one input or c should have just one output.
		if (b->numInputs() > 1 && c->numOutputs() > 1) continue;

		SynDFClusterPortIter nextPortb(*b);
		while ((bp = nextPortb++) != NULL) {
		    if (sdfParam != bp->numXfer()) {flag = 0; break;}
		}
		// if flag = 0, then check whether the above condition
		// holds for all of c's ports.
		if (!flag) {
		    flag = 1;
		    SynDFClusterPortIter nextPortc(*c);
		    while ((cp = nextPortc++) != NULL) {
			if (sdfParam != cp->numXfer()) {flag = 0; break;}
		    }
		}
		if (flag) {
		    // c gets absorbed into something else only when
		    // it is the node being examined.  So the main while
		    // loop can never visit c again after it has been absorbed.
		    if (!b->absorb(*c,0)) {
			StringList message = 
				"AcyLoopScheduler::clusterSplicedStars:\n";
			message << "Failed to complete absorb operation.\n";
			Error::warn(message);
			return FALSE;
		    }
		    break;
		}
	    }
	}
    }
    cleanupAfterCluster(*gr);
    return TRUE;
}

/****

Recursive Partitioning by Minimum Cuts heuristic

@Description This function constructs a topological sort by recursively
creating cuts of the graph so that the amount of data crossing the cut
is minimized.
<p>
<a href="#ClassAcyLoopScheduler">See the  book refered to in
the comments for the class for more details.</a>
<p>
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
    int cost = 0, leftCost = 0, rightCost = 0, totalCost = 0;
    int N, lflag = 0, rflag = 0;
    AcyCluster *c, *leftSubGraph = 0, *rightSubGraph = 0, *newL = 0, *newR = 0;
    SequentialList leftGroup, rightGroup;
    StringList message = "AcyLoopScheduler::RPMC:\n";

    // Call graph splitter if total number of nodes in cluster hierarchy 
    // is more than 1.
    if ((N = gr->totalNumberOfBlocks()) > 1) {
	// But first ensure that gr does not have just one cluster inside.
	while (gr->numberBlocks() == 1) {
	    // Set gr to be the cluster inside it.
	    if (gr->head()->isItAtomic()) {

		// Since totalNumberOfBlocks() returned more than 1,
		// we cannot ever end up with an atomic block in this
		// loop; hence, abort.
		message << "There appears to be an error in the  "
			<< "number returned by "
			<< "SynDFCluster::totalNumberOfBlocks.\n";
		Error::warn(message);
		return -1;
	    }
	    gr = (AcyCluster*)gr->head();
	}
	if ( (cost=gr->legalCutIntoBddSets((3*N)/4)) == -1) {
	    // Means that a bounded cut was not found; call again
	    // relaxing the bound.  Note that the splitter returns
	    // -1 iff a cut respecting the bound was not found.
	    // However, with a bound of N-1, a cut should always be found.
	    cost = gr->legalCutIntoBddSets(N-1);
	}

	if (cost == -1) {
	    // Something is wrong.  We have this check for debugging
	    // since normally the graph splitter should never return -1
	    // if it can find a bounded cut, and the code above takes
	    // care of that.
	    message << "AcyCluster::legalCutIntoBddSets failed to "
		    << "produce a cut. Either there is a bug there "
		    << "or earlier clustering steps have introduced "
		    << "cycles into the graph.\n";
	    Error::warn(message);
	    return -1;
	}

	// Now build up the right and left subgraphs.
	AcyClusterIter nextClust(*gr);
	while ((c = nextClust++) != NULL && lflag+rflag < 2) {
	    // first start with some cluster in the left partition
	    if (PARTITION(c) == 0 && lflag == 0) {
		leftSubGraph = c;
		lflag = 1;
		continue;
	    }
	    if (PARTITION(c) == 1 && rflag == 0) {
		rightSubGraph = c;
		rflag = 1;
	    }
	}
	// Ensure that leftSubGraph and rightSubGraph are non-Null
	// If either is NULL, then this means that either there is
	// only one cluster in gr, or that all clusters in gr are
	// on the same side of the cut.  Both these conditions are
	// errors and mean that there is a bug somewhere.
	if (!leftSubGraph || !rightSubGraph) {
	    message << "Cut produced in RPMC has an error: Every node "
		    << "appears to be on the same side of the cut.\n";
	    Error::warn(message);
	    return -1;
	}
	// Now we start clustering using leftSubGraph and rightSubGraph
	// clusters as starting points.
	nextClust.reset();
	while ((c = nextClust++) != NULL) {
	    if (c == leftSubGraph || c == rightSubGraph) continue;
	    if (PARTITION(c) == 0) {
		leftGroup.append(c);
	    } else {
		rightGroup.append(c);
	    }
	}

	if (leftGroup.size() > 0) {
	    ListIter nextLeft(leftGroup);
	    c = (AcyCluster*)nextLeft++;
	    newL = (AcyCluster*)(leftSubGraph->group(*c));
	    if (!newL) {
		message << "Failed to complete group operation.\n";
		Error::warn(message);
		return -1;
	    }
	    PARTITION(newL) = 0;
	    while ((c = (AcyCluster*)nextLeft++) != NULL) {
		if (!newL->absorb(*c)) {
		    message << "Failed to complete absorb operation.\n";
		    Error::warn(message);
		    return -1;
		}
	    }
	} else newL = leftSubGraph;

	if (rightGroup.size() > 0) {
	    ListIter nextRight(rightGroup);
	    c = (AcyCluster*)nextRight++;
	    newR = (AcyCluster*)(rightSubGraph->group(*c));
	    if (!newR) {
		message << "Failed to complete group operation.\n";
		Error::warn(message);
		return -1;
	    }
	    PARTITION(newR) = 1;
	    while ((c = (AcyCluster*)nextRight++) != NULL) {
		if (!newR->absorb(*c)) {
		    message << "Failed to complete absorb operation.\n";
		    Error::warn(message);
		    return -1;
		}
	    }
	} else newR = rightSubGraph;

	// Now call RPMC again on the left and right subgraphs.

	leftCost = RPMC(newL);
	if (leftCost < 0) return -1;

	rightCost = RPMC(newR);
	if (rightCost < 0) return -1;

	totalCost = leftCost + cost + rightCost;
    }
    return totalCost;
}

/****

Acyclic Pairwise Grouping of Adjacent nodes heuristic

@Description This algorithm builds up a schedule by repeatedly clustering
pairs of actors according to a particular cost function.
<p>
<a href="#ClassAcyLoopScheduler">See the  book refered to in
the comments for the class for more details.</a>
<p>
<a href="http://ptolemy.eecs.berkeley.edu/papers/PganRpmcDppo/">
Also see the second paper at this site.</a>

@Returns Int: either postive or -1; -1 means that something went wrong and
schedule was not computed.  Positive integer should equal graphSize+1 if
schedule was computed successfully.

@SideEffects gr gets modified due to clustering operations.  reachMatrix,
edgelist are also modified.

****/
int AcyLoopScheduler::APGAN(Galaxy* gal)
{
    // APGAN heuristic.  In this heuristic, we build up a cluster hierarchy
    // by clustering pairs of actors together at each step.  The pair chosen
    // for clustering should a) not introduce any cycles in the graph,
    // and b) maximize the gcd of the repetitions of the two actors
    // over all pairs that can be clustered together without introducing
    // cycles.  See chapter 7 of the book
    // "Software Synthesis from Dataflow graphs" for more details.
    // Also note that for APGAN, we do not need a pre-pass to
    // cluster the spliced-in stars with their sources/sinks since
    // the APGAN algorithm, by its very nature, will keep spliced actors
    // together with their source/sink in the same innermost loop always.
    int N, rho, flag, tmp, i, cost = 0;
    SynDFClusterPort *p, *clusterEdge = 0;
    AcyCluster *src, *snk, *clusterSrc = 0, *clusterSnk = 0, *omega;
    StringList message = "AcyLoopScheduler::APGAN:\n";
    // create the edge list; this list will contain all the output ports
    // in gal.
    createEdgelist(gal);
    resetFlags(*gal, PARTITION_FLAG_LOC, 0);
    while ((N=gal->numberBlocks()) > 1) {
	// explore each edge in the graph.
	rho = 0;
	ListIter nextEdge(edgelist);
	while ((p=(SynDFClusterPort*)nextEdge++) != NULL) {
	    if (p->DELETE || !p->far() || !p->far()->parent()) {
		// If output port is unconnected for some reason (could
		// be that it is a galaxy port going out of this
		// galaxy), just delete it from the list here.
		nextEdge.remove();
		continue;
	    }
	    // p is an output port since edgelist only has output ports
	    src = (AcyCluster*)p->parent();
	    snk = (AcyCluster*)p->far()->parent();
	    // The following should never be true since any such edge
	    // between src and snk that have been clustered already
	    // would be marked delete already.
	    if (reachMatrix.m[NODE_NUM(src)][NODE_NUM(snk)] == -1) {
		nextEdge.remove();
		continue;
	    }
	    tmp = gcd(src->loopFactor(), snk->loopFactor());
	    if (rho < tmp) {
		// Now we should check whether 
	        // clustering these will create a cycle.
	        // It will create a cycle if some node that is reachable from
	        // src can reach snk.
	        flag = 1;
	        for (i = 0; i<graphSize; i++) {
		    if (reachMatrix.m[NODE_NUM(src)][i] == 1 &&
		        reachMatrix.m[i][NODE_NUM(snk)] == 1) {flag=0; break;}
	        }
	        if (flag) {
		    rho = tmp;
		    clusterSrc = src;
		    clusterSnk = snk;
		    clusterEdge = p;
		}
	    }
	}
	// Now check whether the edgelist is empty.  If it is empty,
	// then there are no new candidates to be clustered since
	// the part that removes edges above also continues the loop.
	// Also, only edges marked for deletion from previous pass are
	// deleted. Hence, this means that the graph has several connected
	// components.
	if (edgelist.size() == 0) break;

	// cluster clusterSrc and clusterSnk.  Also update the reachability
	// matrix and the edge list.  The edgelist is updated next time around;
	// now it is simply marked to be deleted.
	omega = (AcyCluster*)(clusterSrc->group(*clusterSnk));
	NODE_NUM(omega) = NODE_NUM(clusterSrc);

	PARTITION(clusterSrc) = 0;
	PARTITION(clusterSnk) = 1;

	// mark all the output ports from clusterSrc and clusterSnk
	// for deletion as well (this will include clusterEdge).
	// Finally, add all of the output ports of omega to the list.
	SynDFClusterOutputIter nextO1(*clusterSrc);
	while ((p=nextO1++) != NULL) {
	    p->DELETE = 1;
	    if (!p->far() || !p->far()->parent()) continue;
	    // update the cost by looking at all arcs between clusterSrc
	    // and clusterSnk.
	    if (p->far()->parent() == clusterSnk) {
		cost += (clusterSrc->loopFactor() * p->numXfer()) /
			omega->loopFactor();
		cost += p->numInitDelays();
	    }
	}
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
	    reachMatrix.m[NODE_NUM(clusterSrc)][i] = 
	       (reachMatrix.m[NODE_NUM(clusterSrc)][i]==1) |
	       (reachMatrix.m[NODE_NUM(clusterSnk)][i]==1);
	    reachMatrix.m[i][NODE_NUM(clusterSrc)] =
	       (reachMatrix.m[i][NODE_NUM(clusterSrc)]==1) |
	       (reachMatrix.m[i][NODE_NUM(clusterSnk)]==1);
	       // The above update can cause self-loops to occur
	       // in the reachability matrix since if i = NODE_NUM(clusterSrc)
	       // then that entry will be 1 by the above test; hence, we
	       // set it to 0 below.
	    reachMatrix.m[i][i] = 0;
	}
	for (i = 0; i < graphSize; i++) {
	    reachMatrix.m[NODE_NUM(clusterSnk)][i] = -1;
	    reachMatrix.m[i][NODE_NUM(clusterSnk)] = -1;
	}
    }

    // graph has been completely clustered now.  Build up the schedule
    // tree by traversing the clustering hierarchy backwards.
    // First check that there is only one node in gal.
    if (gal->numberBlocks() != 1) {
	// this means that the graph has several connected components
	// So we just create a linear tree-shaped cluster hierarchy
	// out of these in order to maintain a binary-tree hierarchy
	// in the cluster nesting.
	AcyClusterIter nextClust(*gal);
	AcyCluster* c1, *c2;
	// Since we use group operations, and these add clusters to
	// the parent galaxy, we do not iterate over members of the parent.
	// instead, we copy the clusters to this list and iterate over
	// this list.
	SequentialList connComps;

	// first check that all clusters are maximal connected components
	while ((c1=nextClust++) != NULL) {
	    if (c1->numberPorts() != 0) {
		// Check for unconnected ports here since one non-bug
		// situation where it can happen is with wormholes:
		// a galaxy port can be connected to a wormhole; it
		// looks unconnected from inside a cluster.
		SynDFClusterPortIter nextPort(*c1);
		flag=0;
		while((p=nextPort++) != NULL) {
		    if (p->far() && p->far()->parent()) {flag=1;break;}
		}
		if (flag) {
		    message << "Did not cluster properly; "
			    << "there is a bug in the routine.\n";
		    Error::warn(message);
		    return -1;
		}
	    }
	    connComps.append(c1);
	}
	// Now build up the linear binary-tree.
	ListIter nextComp(connComps);
	c1 = (AcyCluster*)nextComp++;
	while ((c2=(AcyCluster*)nextComp++) != NULL) {
	    c1 = (AcyCluster*)(c1->group(*c2));
	    if (!c1) break;
	}
	// Below should hopefully never occur.
        if (gal->numberBlocks() != 1) {
	    message << "Failed to build tree-hierarchy out of "
		    << "connected components after main clustering.\n";
	    Error::warn(message);
	    return -1;
    	}
    }
    return cost;
}

/****

Builds up the topological sort by traversing the cluster hierarchy

@Description Traverse the cluster hierarchy to determine the order in which
nodes should be scheduled.

@Returns Int: -1 if error, graphSize+1 if success.

@SideEffects APGANTopSort gets filled with node numbers.

****/
int AcyLoopScheduler::buildTopsort(AcyCluster* gr, int ti)
{
    AcyCluster *tmp,*leftOmega, *rightOmega;
    int lt, rt;
    if (gr->numberBlocks() > 2) {
	StringList message = "AcyLoopScheduler::buildTopsort:\n";
	message << "Error detected in RPMC or APGAN in AcyLoopScheduler. "
		<< "Clustering process did not cluster properly.\n";
	Error::warn(message);
	return 0;
    }
    else if (gr->numberBlocks() == 1) {
	if (!gr->head()->isItAtomic()) {
	    // A cluster again; call this function on it again.
	    return buildTopsort((AcyCluster*)(gr->head()), ti);
	} else {
	    // Atomic block; hence, a leaf node in the schedule tree.
	    topSort[ti++] = NODE_NUM(gr->head());
	    return ti;
	}
    }
    else {
	AcyClusterIter nextClust(*gr);
	tmp = nextClust++;
	if (PARTITION(tmp)) {
	    rightOmega = tmp;
	    leftOmega = nextClust++;
	}
	else {
	    leftOmega = tmp;
	    rightOmega = nextClust++;
	}
	lt = buildTopsort(leftOmega, ti);
	if (!lt) return 0;
	rt = buildTopsort(rightOmega, lt);
	if (!rt) return 0;
	return rt;
    }
}

/****

Dynamic Programming Post Optimization

@Description Take a topological sort and create an optimal looping hierarchy.
Uses 3 matrices to store data.
<p>
See
<a href="http://ptolemy.eecs.berkeley.edu/papers/chainmemman.ps.Z">the Icassp 94 paper
</a> for details.
<p>
<a href="#ClassAcyLoopScheduler">Also see the  book refered to in
the comments for the class.</a>
<p>
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
    for (int j = 0; j< N3; j++) costs[j] = 0;
    int* reps = new int[graphSize];

    costMatrix.resize(graphSize,graphSize);
    splitMatrix.resize(graphSize,graphSize);
    gcdMatrix.resize(graphSize,graphSize);
    for (j=0; j<graphSize; j++) {
	gcdMatrix.m[j][j] = nodelist[topSort[j]]->reps();
	reps[j] = gcdMatrix.m[j][j];
    }
    for (j = 0; j < graphSize-1; j++) {
	int maxi = graphSize - 1 - j;
	for (i = 0; i < maxi; i++) {
	    ke = i + j + 1;
	    costMatrix.m[i][ke] = INT_MAX;
	    costTmp = INT_MAX;
	    g = gcd(gcdMatrix.m[i][ke-1], reps[ke]);
	    gcdMatrix.m[i][ke] = g;
	    // Define these indices here to avoid computing in the
	    // inner loop
	    cur = N2*i + graphSize*ke;
	    left = cur - graphSize;
	    bottom = cur + N2;
	    btmlft = bottom - graphSize;
	    costij = 0;
	    tmpInc1 = incMatrix.m[topSort[i]][topSort[ke]];
	    tmpInc2 = incMatrix.m[topSort[ke]][topSort[i]];
	    tmpDel1 = delMatrix.m[topSort[i]][topSort[ke]];
	    tmpDel2 = delMatrix.m[topSort[ke]][topSort[i]];
	    if ( tmpInc1 > 0) {
		costij = reps[i]*tmpInc1/g + tmpDel1;
	    }
	    if (tmpInc2 > 0) {
		// First check if the number of delays are sufficiently high
		// since this is a feedback arc
		if (tmpDel2 < tmpInc2*reps[ke]) {
		    StringList message = "AcyLoopScheduler::DPPO:\n";
		    message << "DPPO thinks that the topological sort "
			    << "constructed by RPMC or APGAN "
		            << "has reverse arcs with insufficient delay.\n"
			    << "This could happen if the topSort array "
			    << "is filled incorrectly.  Either that or "
			    << "there is a bug in the DPPO function.\n";
		    Error::warn(message);
		    return -1;
		} else {
		    costij = tmpDel2;
		}
	    }
	    for (int k = i; k<ke; k++) {
		cst = costs[left+ k] * gcdMatrix.m[i][ke-1];
		cst += costs[bottom + k] * gcdMatrix.m[i+1][ke];
		cst -= costs[btmlft + k] * gcdMatrix.m[i+1][ke-1];
		cst = cst/g;
		cst += costij;
		costs[cur+k] = cst;
		costMatrix.m[i][ke] = min(costMatrix.m[i][ke],
			costMatrix.m[i][k]+ costMatrix.m[k+1][ke]+cst);
		if (costMatrix.m[i][ke] != costTmp) splitMatrix.m[i][ke] = k;
		costTmp = costMatrix.m[i][ke];
	    }
	}
    }
    delete [] reps;
    delete [] costs;
    return costMatrix.m[0][graphSize-1];
}

/****

Recursive function to establish buffer sizes.

@Description Establish the buffer size for all geodesics according to the
schedule computed by <code>computeSchedule</code>.

@SideEffects Geodesics bufferSize is changed.

@Returns Int: TRUE or FALSE

****/
int AcyLoopScheduler::fixBufferSizes(int i, int j) {
 
    // function that establishes the maximum buffer size for
    // each arc based on the schedule computed by RPMC+DPPO
    // or APGAN+DPPO
    int N = j-i+1;
    int g_ij = gcdMatrix.m[i][j];
    int split, repsij, delays;
    DataFlowStar *s;
    Block *t;
    DFPortHole *p;
    Geodesic* geod;
    if (N > 1) {
	split = splitMatrix.m[i][j];
	// fix buffer sizes for all arcs crossing this split
	for (int k=i; k<=split; k++) {
	    s = nodelist[topSort[k]];
	    repsij = (s->reps())/g_ij;
	    DFStarPortIter nextPort(*s);
	    while ((p=nextPort++) != NULL) {
		if (p->far() && (t=p->far()->parent()) && 
			topInvSort[NODE_NUM(t)] > split &&
			topInvSort[NODE_NUM(t)] <= j ) {
		    // t is on the other side of the split.
		    geod = p->geo();
		    if (!geod) {
			Error::warn(*p, "has no geodesic.\n");
			return FALSE;
		    }
		    delays = p->numInitDelays();
		    if (p->isItInput()) {
			// A reverse arc
			geod->setMaxArcCount(delays);
		    } else {
		        geod->setMaxArcCount(repsij*(p->numXfer())+delays);
		    }
		}
	    }
	}
	if (!fixBufferSizes(i,split)) return FALSE;
	if (!fixBufferSizes(split+1,j)) return FALSE;
    }
    return TRUE;
}

/****

Determine whether the graph is well-ordered or not.

@Description
Returns a list containing ALL the nodes in the graph in topological
ordering if the graph is well ordered.  This means that the
following are equivalent for this graph
<p>
<li>a) there is only 1 topological sort
<li>b) there is a hamiltonian path
<li>c) there is a total precedence ordering rather than a partial one,
<li>d) For any two nodes u,v, it is either the case that u->v is a path
	or v->u is a path
<p>
The algorithm is to repeatedly find sources, and return the incomplete
list if there is more than 1 source node at any stage.  If there is
only one source at any stage, we mark that node (meaning it is
"removed" from the graph) and find sources on the resulting
graph.  Worst case running time: O(N+E) where N=graphSize,
E is the number of edges.

@SideEffects
Fills the <code>topsort</code> list with the topological ordering of the nodes

@Returns Int TRUE or FALSE.

****/
int AcyLoopScheduler::isWellOrdered(Galaxy* g, SequentialList& topsort)
{
    int flagLoc = TMP_PARTITION_FLAG_LOC, numPasses=0;
    resetFlags(*g,flagLoc); // reset flags[flagLoc] for all blocks
    SequentialList sources;
    findSources(g,flagLoc, sources, 0);
    while (numPasses < graphSize) {
	if (sources.size() == 1) {
	    Block* src = (Block*)sources.getAndRemove();
	    topsort.append(src);
	    src->flags[flagLoc] = 1;
	    numPasses++;
	    findSources(g, flagLoc, sources, src);
	} else {
	    return FALSE;
	}
    }
    return TRUE;
}
/************* End of methods called from computeSchedule() *************/

/************************** Run methods *********************************/


// AcyLoopScheduler does not need to redefine run(); SDFScheduler::run
// is used.

// this is called from SDFScheduler::run
void AcyLoopScheduler::runOnce()
{
    (void)runOnce(0,graphSize-1,1);
}

/****

This <code>runOnce(int,int,int)</code> actually does the work.

@Description Recursively traverse the splitMatrix and run the stars according
to the schedule.

****/
int AcyLoopScheduler::runOnce(int i, int j, int g)
{
    int N = j-i+1;
    int g_ij = gcdMatrix.m[i][j];
    int loopFac = g_ij/g;
    int split;
    DataFlowStar* s;
    if (N == 1) {
	s = nodelist[topSort[i]];
	for (int k = 0; k < loopFac; k++) {
	    invalid = !s->run();
	    if (invalid) return FALSE;
	}
	return TRUE;
    } else {
	split = splitMatrix.m[i][j];
	for (int k = 0; k < loopFac; k++) {
	    if(!runOnce(i,split,g_ij)) return FALSE;
	    if(!runOnce(split+1,j,g_ij)) return FALSE;
	}
	return TRUE;
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
    int g_ij = gcdMatrix.m[i][j];
    int loopFac = g_ij/g;
    int split;
    DataFlowStar *s;
    if (N==1) {
	s = nodelist[topSort[i]];
	if (loopFac > 1) t.beginIteration(loopFac,depth);
	t.writeFiring(*s, depth+1);
	if (loopFac > 1) t.endIteration(loopFac, depth);
    } else {
	split = splitMatrix.m[i][j];
	if (loopFac > 1) t.beginIteration(loopFac,depth);
	genCode(t, depth+1, i, split, g_ij);
	genCode(t, depth+1, split+1, j, g_ij);
	if (loopFac > 1) t.endIteration(loopFac,depth);
    }
}

/************************* End of methods called from run() **********/

/************************* Display Schedule routines. ****************/
//
///////  These methods are called when display-schedule  ///////
///////  is called in pigi or the "schedule" command is  ///////
///////  used in ptcl.

// Display schedule in standard format;called by "schedule" command (pigi,ptcl)
StringList AcyLoopScheduler::displaySchedule()
{
    StringList out;
    out << "{\n";
    out << "{ scheduler \"Murthy and Bhattacharyya's SDF Loop scheduler\" }\n";
    if (schedSuccess) {
	out << dispNestedSchedules(0,0,graphSize-1,1);
    } else {
	out << "Error: schedule hasn't been computed.\n";
    }
    out << "}\n";
    return out;
}

// called by displaySchedule; this one actually does the work.
StringList AcyLoopScheduler::dispNestedSchedules(int depth,int i,int j,int g)
{
    StringList sch;
    int split;
    int N = j-i+1;
    int g_ij = gcdMatrix.m[i][j];
    int loopFac = g_ij/g;
    if (N == 1) {
	sch << tab(depth);
	if (loopFac > 1) {
	    sch << "{ repeat " << loopFac << " {\n" << tab(depth+1);
	}
	sch << "{ fire " << nodelist[topSort[i]]->name() << " }\n";
	if (loopFac > 1) {
	    sch << tab(depth) << "}\n";
	}
    } else {
	split = splitMatrix.m[i][j];
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

/************************ Methods used for debugging only *******************/

// These methods are for debugging; they print out the various matrices
// used by this scheduler.
// This one prints the topology-related matrices
void AcyLoopScheduler::printTopMatrices()
{
    cout << "The incidence matrix is:\n\n" << incMatrix;
    cout << "The delay matrix is:\n\n" << delMatrix;
    cout << "The reachability matrix is:\n\n" << reachMatrix;
}

// This one prints out the matrices used by DPPO
void AcyLoopScheduler::printDPPOMatrices()
{
    cout << "The GCD matrix is:\n\n" << gcdMatrix;
    cout << "The costs matrix is:\n\n" << costMatrix;
    cout << "The splits matrix is:\n\n" << splitMatrix;
}

// This one prints out the RPMC and APGAN topSort arrays
void AcyLoopScheduler::printTopSorts()
{
    int i = 0;
    cout << "The RPMC top sort array is:\n\n[";
    for (i = 0; i < graphSize; i++) {
	cout << RPMCTopSort[i] << "  ";
    }
    cout << "]\n\n" << "The APGAN top sort array is:\n\n[";
    for (i = 0; i < graphSize; i++) {
	cout << APGANTopSort[i] << "  ";
    }
    cout << "]\n";
}

// This one prints out the node number corresponding to each star
void AcyLoopScheduler::printStarNumbers()
{
    for (int i = 0; i < graphSize; i++) {
	cout << nodelist[i]->name() << " : " << i << "\n";
    }
    cout << "\n";
}

// define << operator for SimpleIntMatrices
ostream& operator << (ostream& o, const SimpleIntMatrix& a)
{
    StringList sl;
    for (int i = 0; i < a.nrows; i++) {
	for (int j = 0; j < a.ncols; j++) {
	    sl << a.m[i][j] << "  ";
	}
	sl << "\n";
    }
    sl << "\n";
    o << sl;
    return o;
}


/****
// old one with BFS

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
void AcyLoopScheduler :: createReachabilityMatrix(Galaxy& gal)
{
    DataFlowStar *s, *t, *u;
    GalTopBlockIter nextStar(gal);
    SequentialList nodesTobeVisited;
    graphSize = gal.numberBlocks();
    reachMatrix.resize(graphSize, graphSize);
    SimpleIntMatrix nodeColors(1,graphSize);

    while ((s=(DataFlowStar*)nextStar++) != NULL) {
	nodesTobeVisited.initialize();
	nodesTobeVisited.append(s);
	for (int i=0; i<graphSize; i++) nodeColors.m[0][i] = 0;
	while ((t = (DataFlowStar*)nodesTobeVisited.getAndRemove()) != NULL) {
	    nodeColors.m[0][NODE_NUM(t)] = 2;
	    SuccessorIter nextSucc(*t);
	    while ((u=(DataFlowStar*)nextSucc++) != NULL) {
		if (reachMatrix.m[NODE_NUM(u)][NODE_NUM(s)]) {
		    StringList message;
		    message << "Failed to create reachability matrix. ";
			    << "Graph appears to be non-acyclic.\n";
		    Error::warn(message);
		    return FALSE;
		}
		reachMatrix.m[NODE_NUM(s)][NODE_NUM(u)] = 1;
		if (nodeColors.m[0][NODE_NUM(u)] == 0) {
		    nodesTobeVisited.append(u);
		    nodeColors.m[0][NODE_NUM(u)] = 1;
		}	
	    }
	}
    }
}
*/
