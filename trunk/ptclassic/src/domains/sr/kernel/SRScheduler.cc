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

#include "SRScheduler.h"
#include "SRStar.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "Error.h"
#include <stream.h>

extern const char SRdomainName[];

// Constructor.
SRScheduler::SRScheduler()
{
    setStopTime(0.0);
    setCurrentTime(0.0);

    /* Insert your own initialization code. */
}

// Domain identification.
const char* SRScheduler::domain() const
{
    return SRdomainName;
}

// Initialization.
void SRScheduler::setup()
{
    if (!galaxy()) {
	Error::abortRun(domain(), " scheduler has no galaxy.");
	return;
    }

    galaxy()->initialize();
}

// Run (or continue) the simulation.
int SRScheduler::run()
{
    if (SimControl::haltRequested() || !galaxy()) {
	Error::abortRun(domain(), " Scheduler has no galaxy to run");
	return FALSE;
    }

    GalStarIter nextStar( *galaxy() );
    Star *s;

    while ( currentTime < stopTime ) {

      // Begin the instant by initializing all the stars

      nextStar.reset();
      while ( ( s = nextStar++ ) != 0 ) {
	((SRStar *) s)->initializeInstant();
      }

      int numKnown = 0;
      int lastNumKnown;

      // Simulate the instant by calling each star's go() method until
      // no additional outputs become known

      do {
	lastNumKnown = numKnown;
	numKnown = 0;

	nextStar.reset();

	while ( ( s = nextStar++ ) != 0 ) {
	  ((SRStar *) s)->run();
	  numKnown += ((SRStar *) s)->knownOutputs();
	}

      } while ( numKnown != lastNumKnown );     

      // Finish the instant by calling each star's tick() method

      nextStar.reset();

      while ( ( s = nextStar++ ) != 0 ) {
	((SRStar *) s)->tick();
      }

      // Advance to the next tick

      currentTime += 1.0;
    }

    return !SimControl::haltRequested();
}

// Get the stopping time.
double SRScheduler::getStopTime()
{
    return stopTime;
}

// Set the stopping time.
void SRScheduler::setStopTime(double limit)
{
  stopTime = limit;
}
