static const char file_id[] = "$RCSfile$";

/*  Version $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

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

    Author:	S. A. Edwards
    Created:	9 April 1996

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRStaticScheduler.h"
#include "SRStar.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "Error.h"
#include "StringList.h"
#include "GraphUtils.h"
#include <stream.h>

extern const char SRdomainName[];

StringList SRStaticScheduler::displaySchedule()
{
  StringList out;
  out << "{\n  { scheduler \"Static SR Scheduler\" }\n"
      << mySchedule.printVerbose()
      << "}\n";
  return out;
}

StringList SRSchedule::printVerbose() const
{
  StringList out;
  SRSchedIter clusters(*this);
  SRScheduleCluster * c;

  for ( int i = size(); --i >= 0 ; ) {
    c = clusters++;
    out << "  { repeat " << c->numIterations << " {\n";
    {
      SRSchedClusterIter stars(*c);
      SRStar * s;
      for ( int j = c->size() ; --j >= 0 ; ) {
	s = stars++;
	  out << "    { fire " << s->name() << " }\n";
      }
    }
    out << "  }}\n";
  }
  return out;
}

// Constructor
SRStaticScheduler::SRStaticScheduler()
{
  numInstantsSoFar = 0;
  numInstants = 1;
  schedulePeriod = 10000.0;
}

// Domain identification.
const char* SRStaticScheduler::domain() const
{
    return SRdomainName;
}

// Initialization.
void SRStaticScheduler::setup()
{
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
int SRStaticScheduler::run()
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
// This is a very simple scheduler--in each instant, it initializes
// all the stars, runs each, and checks to see if any more outputs
// have become defined.  If any have, it runs them all again.
// Finally, it calls tick() to advance the stars' states.
//
void SRStaticScheduler::runOneInstant()
{
  GalStarIter nextStar( *galaxy() );
  Star *s;

  // Begin the instant by initializing all the stars

  while ( ( s = nextStar++ ) != 0 ) {
    ((SRStar *) s)->initializeInstant();
  }

  // Run the schedule

  SRSchedIter nextCluster( mySchedule );
  SRScheduleCluster * c;
  SRStar * st;
  while ( (c = nextCluster++) != NULL ) {
    SRSchedClusterIter nextStar( *c );
    for ( int i = c->numIterations ; --i >= 0 ; ) {
      nextStar.reset();
      while ( (st = nextStar++) != NULL ) {
	cout << "Firing " << st->name() << "\n";
	st->run();
      }
    }
  }

  // Finish the instant by calling each star's tick() method

  nextStar.reset();

  while ( ( s = nextStar++ ) != 0 ) {
    ((SRStar *) s)->tick();
  }

  cout << "Completed instant\n";

}

// Set the stopping time, for compatibility with the DE scheduler
//
// Roundoff errors makes this non-trivial
//
void SRStaticScheduler::setStopTime(double limit)
{
  numInstants = int( floor(limit + 0.001) );
}

// Set the stoppping time for a wormhole
//
// A wormhole invocation is always one instant--the time given is ignored.
//
void SRStaticScheduler::resetStopTime(double)
{
  numInstants = 1;
  numInstantsSoFar = 0;
}

// Do the actual scheduling

int SRStaticScheduler::computeSchedule( Galaxy & g )
{

  SCCDecompose( g );

  cout << displaySchedule();

  cutSCCs( g );

  cout << displaySchedule();

  return 0;
}

void SRStaticScheduler::SCCDecompose( Galaxy & g )
{
  // clear the depth flags
  resetFlags( g, sccDepth );

  // Call visit on all unvisited stars

  GalStarIter nextStar(g);
  SRStar * s;
  Stack stack;
  int index = 1;
  while ( (s = (SRStar *)(nextStar++)) != NULL ) {
    if ( s->flags[sccDepth] == 0 ) {
      SCCVisit( s, stack, index );
    }
  }  
}

// The strongly-connected component algorithm from Sedgewick's Algorithms
int SRStaticScheduler::SCCVisit( SRStar * s, Stack & stack, int & index )
{
  //  cout << "Visiting " << s->name() << " with index " << index << "\n";

  int min = index;
  s->flags[sccDepth] = index++;
  stack.pushTop( (Pointer) s );
  
  // Iterate over all the stars driven by the outputs of this one
  // TODO: This doesn't work because ports in the SR domain may be connected
  // to multiple inputs!

  BlockOutputIter nextPort( *((Block *)s) );
  PortHole * near;
  PortHole * far;
  SRStar * farStar;
  while ( (near = nextPort++) != NULL ) {
    far = near->far();
    if ( far != NULL ) {
      farStar = (SRStar *)(far->parent());

      int m;
      if ( (m = farStar->flags[sccDepth]) == 0 ) {
	m = SCCVisit( farStar, stack, index );
      }

      if (m < min) {
	min = m;
      }

    }
  }

  if ( min == s->flags[sccDepth] ) {

    //    cout << "Forming a new strongly-connected component with\n";

    // Form a new strongly-connected component by removing all the stars
    // on the stack up to and including this one

    SRScheduleCluster * newCluster = new SRScheduleCluster;

    SRStar * theStar;

    do {
      theStar = (SRStar *) stack.popTop();
      newCluster->append( theStar );
      //      cout << " " << theStar->name();
      theStar->flags[sccDepth] = 1000000;	// Larger than the number
					        // of stars           
    } while (theStar != s);

    //    cout << "\n";

    mySchedule.prepend( newCluster );

  }

  return min;

}

void SRStaticScheduler::cutSCCs( Galaxy & g )
{

  PortHole * nearPort;
  PortHole * farPort;
  SRStar * s;
  SRStar * farStar;
  SRStar * chosenStar;

  resetFlags( g, cutIndex );

  for ( int i = mySchedule.size() ; --i >= 0 ; ) {

    // Remove the last cluster and put a new one at the front of the schedule
    SRScheduleCluster * oldCluster = mySchedule.removeTail();
    SRScheduleCluster * newCluster = new SRScheduleCluster;
    mySchedule.prepend( newCluster );

    SRSchedClusterIter nextStar( *oldCluster );

    // Mark everything as being part of the SCC

    cout << "Processing SCC including ";
    while ( (s = nextStar++) != NULL ) {
      s->flags[cutIndex] = 1;
      cout << s->name() << " ";
    }
    cout << "\n";

    do {

      // Find the best "quality" star in the SCC--the one with maximum
      // fanouts - fanins

      chosenStar = NULL;
      int bestQuality = -100000;	// smallest number possible

      nextStar.reset();

      while ( (s = nextStar++) != NULL ) {
	if ( s->flags[cutIndex] != 0 ) {

	  int quality = 0;

	  // Iterate over all the ports
	  // TODO: This doesn't work for outputs with more than one fanout!

	  BlockPortIter nextPort( *((Block *)s) );
	  while ( (nearPort = nextPort++) != NULL ) {
	    farPort = nearPort->far();
	    if ( farPort != NULL ) {
	      farStar = (SRStar *)(farPort->parent());
	      if ( farStar->flags[cutIndex] != 0 ) {
		if ( nearPort->isItInput() ) {
		  quality--;
		} else {
		  quality++;
		}
	      }
	    }
	  }

	  // Remember the best quality star we find
	  
	  if ( quality > bestQuality ) {
	    bestQuality = quality;
	    chosenStar = s;
	  }
	  
	}
      }

      // If we found a star, add it to the schedule and remove it from
      // the SCC

      if ( chosenStar != NULL ) {
	newCluster->prepend(chosenStar);
	chosenStar->flags[cutIndex] = 0;
	cout << "prepending " << chosenStar->name() << "\n";
      }

    } while ( chosenStar != NULL );

    // Calculate the number of feedback arcs by ordering the stars in the
    // new cluster and counting the number of back arcs

    SRSchedClusterIter nextNewStar( *newCluster );

    int index = 1;

    // Label the stars by the order we found

    while ( (s = nextNewStar++) != NULL ) {
      s->flags[cutIndex] = index++;
    }

    // Go through the list of stars, counting the number of outputs
    // that go to earlier stars

    nextNewStar.reset();

    while ( (s = nextNewStar++) != NULL ) {

      cout << "Checking " << s->name() << "\n";

      BlockOutputIter nextPort( *((Block *)s) );
      while ( (nearPort = nextPort++) != NULL ) {
	farPort = nearPort->far();
	if ( farPort != NULL ) {
	  farStar = (SRStar *)(farPort->parent());
	  if ( farStar->flags[cutIndex] != 0 &&
	       farStar->flags[cutIndex] < s->flags[cutIndex] ) {
	    newCluster->numIterations++;
	    cout << "Found a feedback arc to " << farStar->name() << "\n";
	  }
	}
      }
      
    }

    // Clear the labels on the stars in this SCC

    nextNewStar.reset();
    while ( (s = nextNewStar++) != NULL ) {
      s->flags[cutIndex] = 0;
    }
    
    delete oldCluster;
  }

}
