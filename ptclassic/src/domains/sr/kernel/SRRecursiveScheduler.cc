static const char file_id[] = "SRRecursiveScheduler.cc";

/* Version $Id$

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

    Author:     S. A. Edwards
    Created:    23 October 1996

 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRStar.h"
#include "SRRecursiveScheduler.h"
#include "SRDependencyGraph.h"
#include "StringList.h"
#include "Error.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "GraphUtils.h"
#include <stream.h>

extern const char SRdomainName[];

// Return a printed representation of the schedule
//
// @Description Identify this as the static SR scheduler

StringList SRRecursiveScheduler::displaySchedule()
{
  StringList out;
  out << "{\n  { scheduler \"Recursive SR Scheduler\" }\n"
      << "}\n";
  return out;
}

SRRecursiveScheduler::SRRecursiveScheduler()
{
  numInstantsSoFar = 0;
  numInstants = 1;
  schedulePeriod = 10000.0;
  dgraph = NULL;
}

SRRecursiveScheduler::~SRRecursiveScheduler()
{
  if (dgraph != NULL) {
    delete dgraph;
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

  // TODO: Write this!

  // Finish the instant by calling each star's tick() method

  nextStar.reset();

  while ( ( s = nextStar++ ) != 0 ) {
    ((SRStar *) s)->tick();
  }

  cout << "Completed instant\n";

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

  cout << dgraph->displayGraph();

  return 0;
}

