static const char file_id[] = "SRRecursiveScheduler.cc";

/* Version $Id$

@Copyright (c) 1996-%Q% The Regents of the University of California.
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

    Author:     S. A. Edwards
    Created:    23 October 1996

 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRStar.h"
#include "SRRecursiveScheduler.h"
#include "SRRecSchedule.h"
#include "SRDependencyGraph.h"
#include "SRParter.h"
#include "Set.h"
#include "StringList.h"
#include "Error.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "GraphUtils.h"
#include <stream.h>
#include <assert.h>
#include <limits.h>

extern const char SRdomainName[];

// Return a printed representation of the schedule
//
// @Description Identify this as the static SR scheduler

StringList SRRecursiveScheduler::displaySchedule()
{
  StringList out;
  out << "{\n  { scheduler \"Recursive SR Scheduler\" }\n";
  if ( schedule ) {
    out << "  { numberOfStarOrClusterFirings " << schedule->cost() << " }\n"
	<< schedule->printVerbose();
  }
  out << "}";
  return out;
}

SRRecursiveScheduler::SRRecursiveScheduler()
{
  numInstantsSoFar = 0;
  numInstants = 1;
  schedulePeriod = 10000.0;
  dgraph = NULL;
  schedule = NULL;
}

SRRecursiveScheduler::~SRRecursiveScheduler()
{
  if (dgraph != NULL) {
    delete dgraph;
  }
  if ( schedule != NULL ) {
    delete schedule;
  }
}

// Return the name of the SR domain
const char* SRRecursiveScheduler::domain() const
{
    return SRdomainName;
}

// Initialize the galaxy and compute the schedule
void SRRecursiveScheduler::setup()
{
  numInstants = 1;
  numInstantsSoFar = 0;

  if (!galaxy()) {
    Error::abortRun(domain(), " scheduler has no galaxy.");
    return;
  }    

  galaxy()->initialize();

  if ( !computeSchedule( *galaxy() ) ) {
    return;
  }
}

// Run (or continue) the simulation
//
// @Description Call runOneInstant until numInstants equals
// numInstantsSoFar, checking SimControl::haltRequested() before each
// instant.
int SRRecursiveScheduler::run()
{
    if (SimControl::haltRequested() || !galaxy()) {
	Error::abortRun(domain(), " Scheduler has no galaxy to run");
	return FALSE;
    }

    while ( numInstantsSoFar < numInstants && !SimControl::haltRequested() ) {
      runOneInstant();
      currentTime += schedulePeriod;
      numInstantsSoFar++;
    }

    return !SimControl::haltRequested();
}

// Execute the galaxy for an instant
//
// @Description Call initializeInstant() on each star, and then for
// each cluster, call run() on each star in the cluster, repeating the
// cluster as many times as necessary.  Call tick() on each star.

void SRRecursiveScheduler::runOneInstant()
{
  GalStarIter nextStar( *galaxy() );
  Star *s;

  // Begin the instant by initializing all the stars

  while ( ( s = nextStar++ ) != 0 ) {
    ((SRStar *) s)->initializeInstant();
  }

  // Run the schedule

  if ( schedule ) {
    schedule->runSchedule();
  }

  // Finish the instant by calling each star's tick() method

  nextStar.reset();

  while ( ( s = nextStar++ ) != 0 ) {
    ((SRStar *) s)->tick();
  }

  // cout << "Completed instant\n";

}

// Set the stopping time, for compatibility with the DE scheduler
//
// @Description Roundoff errors makes this non-trivial

void SRRecursiveScheduler::setStopTime(double limit)
{
  numInstants = int( floor(limit + 0.001) );
}

// Set the stoppping time for a wormhole
//
// @Description A wormhole invocation is always one instant--the time
// given is ignored.

void SRRecursiveScheduler::resetStopTime(double)
{
  numInstants = 1;
  numInstantsSoFar = 0;
}

// Generate the schedule
//
// @Description

int SRRecursiveScheduler::computeSchedule( Galaxy & g )
{

  if (dgraph != NULL ) {
    delete dgraph;
  }

  dgraph = new SRDependencyGraph( g );

  if ( dgraph->vertices() == 0 ) {
    delete dgraph;
    dgraph = 0;
    return 0;
  }

  // cout << dgraph->displayGraph();

  Set wholeGraph(dgraph->vertices(), 1);

  if ( schedule != NULL ) {
    delete schedule;
  }

  schedule = new SRRecursiveSchedule( *dgraph );

  //  cout << "There are " << Set::setcount() << " sets to begin with\n";

  int mc = mincost( wholeGraph, INT_MAX, *schedule, 0, 0);

  if ( mc == INT_MAX ) {
    // cout << "No schedule met the bound\n";
  } else {
    // cout << "mincost is " << mc << "\n";
    // cout << "best schedule was " << schedule->print() << '\n';

    int cost = schedule->cost();
    assert( cost == mc );

    // Merge all the vertices into the best parallel blocks
    for ( int v = 0 ; v < dgraph->vertices() ; v++ ) {      
      schedule->mergeVertex(v);
    }

    // Split any erroneous parallel blocks
    schedule->splitParallelBlocks();

    // cout << "After splitting " << schedule->print() << '\n';

  }

  // cout << "There are " << Set::setcount() << " sets finally\n";

  return 0;
}

// Compute the SCCs of the given subgraph
//
// @Description This returns a SequentialList of pointers to vertex
// sets---the SCCs in a topological order.
//
// <P> This uses the SCC algorithm from Cormen, Leiserson, and Rivest,
// which computes two DFSs, one on the forward graph, the other on the back.
// A new SequentialList is created, which the caller must ultimately free.
SequentialList *
SRRecursiveScheduler::SCCsOf(Set & subgraph /* Vertices in the subgraph of the
					       dependency graph */ )
{
  SequentialList * SCCs = new SequentialList;

  // cout << "SCCsOf called on " << subgraph.print() << "\n";
  int * finishingTimes = new int[ subgraph.cardinality() ];
  int finishIndex = 0;
  Set unvisitedSet(1);

  unvisitedSet.setequal(subgraph);

  // Perform a DFS on the forward graph, recording finishing times

  SetIter next(subgraph);
  int v;
  while ( (v = next++) >= 0 ) {
    //    cout << "Trying to visit " << v << " in " << unvisitedSet.print() << "\n";
    fDFSVisit( v, unvisitedSet, finishIndex, finishingTimes );
  }

  // We should have visited everything
  assert( unvisitedSet.cardinality() == 0 );

  // We should have recorded the finishing times of every vertex
  // in the subgraph

  //  cout << "finishIndex " << finishIndex << " subgraph.cardinality() "
  //       << subgraph.cardinality() << "\n";

  assert( finishIndex == subgraph.cardinality() );

  //  cout << "Completed DFS of the forward graph\n";

  // Perform a DFS on the backward graph, spitting out SCCs for each group

  unvisitedSet.setequal(subgraph);

  while ( --finishIndex >= 0 ) {
    v = finishingTimes[finishIndex];
    if ( unvisitedSet[v] ) {

      // Create a new SCC and add it to the list

      Set * newSCC = new Set( dgraph->vertices() );
      bDFSVisit( v, unvisitedSet, *newSCC );
      SCCs->append((Pointer) newSCC);

      // cout << "SCC: " << newSCC->print() << "\n";
    }
  }
      
  // We should have visited everything
  assert( unvisitedSet.cardinality() == 0 );

  delete [] finishingTimes;

  return SCCs;

}

// Delete a SequentialList of SCCs
void SRRecursiveScheduler::destroySCCs( SequentialList * SCCs )
{
  Set * SCC;
  ListIter next(*SCCs);

  while ( (SCC = (Set *)(next++)) != NULL ) {
    delete SCC;
  }

  delete SCCs;
}

// Try to visit a node in the subgraph of forward edges
void SRRecursiveScheduler::fDFSVisit( int vertex,
				      Set & unvisitedSet,
				      int & finishIndex,
				      int * finishingTimes )
{
  if ( unvisitedSet[vertex] ) {

    //    cout << "fDFSVisit " << vertex << "\n";

    // Visit the vertex by removing it from the unvisited set
    unvisitedSet -= vertex;

    // Visit each of its destinations
    for (int e = dgraph->forwardEdges(vertex) ; --e >= 0 ; ) {
      fDFSVisit( dgraph->destination(vertex,e),
		 unvisitedSet, finishIndex, finishingTimes );
    }

    // Record the finishing time of this vertex
    finishingTimes[finishIndex++] = vertex;

  }
}

// Try to visit a node in the subgraph of backward edges
void SRRecursiveScheduler::bDFSVisit( int vertex,
				      Set & unvisitedSet,
				      Set & SCC )
{
  if ( unvisitedSet[vertex] ) {

    //    cout << "bDFSVisit " << vertex << "\n";

    // Visit the vertex by removing it from the unvisited set
    unvisitedSet -= vertex;

    // Add it to the SCC
    SCC |= vertex;

    // Visit each of its sources
    for (int e = dgraph->backwardEdges(vertex) ; --e >= 0 ; ) {
      bDFSVisit( dgraph->source(vertex,e), unvisitedSet, SCC );
    }

  }
}

// Compute the minimum cost schedule
//
// @Description Returns the cost of the minimum schedule that is
// less than or equal to m, or INT_MAX if the bound cannot be met.
// If SRPart::ignoreSimple is true and the subgraph is a single SCC and
// depth > 1, then INT_MAX is returned immediately.

int
SRRecursiveScheduler::mincost(
  Set & subgraph	/* Vertices in the subgraph being scheduled */,
  int max		/* Maximum allowable cost */,
  SRRecursiveSchedule & bestSchedule	/* The best schedule found */,
  int scheduleIndex	/* Where to put the subgraph in the schedule */,
  int depth		/* Depth of recursion */ )
{

  // cout << "mincost called on " << subgraph.print() << " with bound " << max
  //     << "\n";

  // Decompose the subgraph into strongly-connected components

  SequentialList * SCCs = SCCsOf( subgraph );

  int numSCCs = SCCs->size();

  // cout << "Found " << numSCCs << " SCCs\n";

  // Check for a single SCC if necessary

  if ( depth > 1 && SRPart::ignoreSimple() && numSCCs == 1
       && subgraph.cardinality() > 1 ) {
    destroySCCs( SCCs );
    return INT_MAX;
  }

  // Compute a rough bound on the cost by summing a rough bound on each SCC
  // and save the size of each SCC for later use

  int * SCCsize = new int[ numSCCs ];

  int remaining = 0;
  int i = 0;
  Set * SCC;
  ListIter next(*SCCs);
  while( (SCC = (Set *)(next++)) != NULL ) {
    SCCsize[i] = SCC->cardinality();

    //cout << "Size " << SCCsize[i] << "\n";

    remaining += SCCsize[i]*SCCsize[i] - (SCCsize[i] - 1);
    i++;
  }

  if ( remaining < max ) {
    max = remaining;	    // Rough bound was better -- use it
  } else {
    remaining = max;	    // Given bound was better -- use it
  }

  // cout << "Initial remaining = " << remaining << "\n";

  // Find the minimum cost of each SCC

  i = 0;
  next.reset();
  while( (SCC = (Set *)next++ ) != NULL ) {

    // if ( depth < 2 ) {
    //   cout << "Working with SCC " << SCC->print() << "\n";
    // }

    // Compute a bound for this component by removing a best-case cost
    // of all the unscheduled SCCs from the remaining cost

    int bound = remaining;
    for ( int j = numSCCs ; --j > i ; ) {
      bound -= SCCsize[j];
    }

    if ( bound < SCCsize[i] ) {
      destroySCCs( SCCs );
      delete [] SCCsize;
      return INT_MAX;		// Bound cannot possibly be met
    }

    if ( SCCsize[i] == 1 ) {

      // A one-dimensional function -- its cost is always one
      remaining--;

      scheduleIndex = bestSchedule.addSingleVertex( scheduleIndex,
						    SCC->onemember() );

    } else {

      // The main challenge: Find the minimum cost of a strongly-connected
      // component

      int actual = INT_MAX;	// No satisfying schedule has been found

      // cout << "The bound on this component is " << bound << "\n";

      SRParter part( *SCC, *dgraph );

      SRRecursiveSchedule trialSchedule( *dgraph );

      int maxpartition;
      Set * partition;

      while (1) {

	maxpartition = (bound - SCCsize[i]) / ( SCCsize[i] - 1 );
	partition = part.next(maxpartition);

	if ( partition == NULL ) {
	  break;
	}

	// if ( depth < 2 ) {
	//   for ( int k = depth ; --k >= 0 ; ) { cout << " "; }
	//   cout << "Part: " << partition->print() << "\n";
	// }

	int psize = partition->cardinality();

	Set remaining( partition->size() );
	remaining.setequal(*SCC);
	remaining -= *partition;

	// cout << "Remaining function " << remaining.print() << "\n";
	  
	int trycost = mincost( remaining,
			       (bound - psize*psize) / (psize + 1),
			       trialSchedule,
			       scheduleIndex + psize,
			       depth + 1 );

	if ( trycost < INT_MAX ) {
	  int found = psize * psize + (psize + 1) * trycost;
	  if ( found < actual ) {
	    actual = found;
	  }
	  if ( actual <= bound ) {
	    //	    if ( depth < 2 ) {
	    //  cout << "Met the bound " << bound << " with a cost of " << actual
	    //   << "\n";
	    // }

	    // Get the optimal schedule for the subgraph and
	    // wrap the first partition around it
	    
	    bestSchedule.getSection( scheduleIndex + psize, SCCsize[i] - psize,
				     trialSchedule );
	    bestSchedule.addPartition( scheduleIndex, SCCsize[i], *partition );
	    
	    bound = actual - 1;
	  }
	}
	
	delete partition;

      }

      if ( actual == INT_MAX ) {
	destroySCCs( SCCs );
	delete [] SCCsize;
	return INT_MAX;		// no considered partition could meet this
      }

      remaining -= actual;

      scheduleIndex += SCCsize[i];

    }

    i++;
  }

  destroySCCs( SCCs );
  delete [] SCCsize;
  return max-remaining;

}
