static const char file_id[] = "SRScheduler.cc";

/*  Version @(#)SRScheduler.cc	1.5 4/22/96

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

    Author:	S. A. Edwards
    Created:	9 April 1996

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRScheduler.h"
#include "SRStar.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "Error.h"
#include <stream.h>

extern const char SRdomainName[];

SRScheduler::SRScheduler()
{
  numInstantsSoFar = 0;
  numInstants = 1;
  schedulePeriod = 10000.0;
}

// Return the name of the SR domain
const char* SRScheduler::domain() const
{
    return SRdomainName;
}

// Initialize the galaxy
void SRScheduler::setup()
{

  numInstants = 1;
  numInstantsSoFar = 0;

  if (!galaxy()) {
    Error::abortRun(domain(), " scheduler has no galaxy.");
    return;
  }

  galaxy()->initialize();
}

// Run (or continue) the simulation
//
// @Description Call runOneInstant until numInstants equals
// numInstantsSoFar, checking SimControl::haltRequested() before each
// instant.

int SRScheduler::run()
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
// @Description This is a very simple scheduler--in each instant, it
// initializes all the stars, runs each, and checks to see if any more outputs
// have become defined.  If any have, it runs them all again.
// Finally, it calls tick() to advance the stars' states.

void SRScheduler::runOneInstant()
{
  GalStarIter nextStar( *galaxy() );
  Star *s;

  // Begin the instant by initializing all the stars

  while ( ( s = nextStar++ ) != 0 ) {
    ((SRStar *) s)->initializeInstant();
  }

  // Count of the number of known outputs, for detecting when to stop

  int numKnown = 0;
  int lastNumKnown;

  // Simulate the instant by calling each star's go() method until
  // no additional outputs become known

  do {
    lastNumKnown = numKnown;
    numKnown = 0;

    nextStar.reset();

    while ( ( s = nextStar++ ) != 0 ) {
      // cout << "Firing " << ((SRStar *) s)->name() << "\n";
      ((SRStar *) s)->run();
      numKnown += ((SRStar *) s)->knownOutputs();
    }

    // cout << "Completed iteration, " << numKnown << " outputs known\n";

  } while ( numKnown != lastNumKnown );     

  // Finish the instant by calling each star's tick() method

  nextStar.reset();

  while ( ( s = nextStar++ ) != 0 ) {
    ((SRStar *) s)->tick();
  }

  // cout << "Completed instant\n";

}

// Set the stopping time, for compatibility with the DE scheduler
//
// @Description Roundoff errors makes this non-trivial.

void SRScheduler::setStopTime(double limit)
{
  numInstants = int( floor(limit + 0.001) );
}

// Set the stoppping time for a wormhole
//
// @Desciption A wormhole invocation is always one instant--the time
// given is ignored.

void SRScheduler::resetStopTime(double)
{
  numInstants = 1;
  numInstantsSoFar = 0;
}
