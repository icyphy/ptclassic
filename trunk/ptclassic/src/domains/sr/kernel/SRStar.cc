static const char file_id[] = "SRStar.cc";

/*  Version @(#)SRStar.cc	1.5 4/22/96

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
    Created:	14 April 1996

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRStar.h"
#include "Block.h"

// #define DEBUG_SRSTAR

#ifdef DEBUG_SRSTAR
#  include <stream.h> // for debugging
#endif

extern const char SRdomainName[];

// Class identification.
ISA_FUNC(SRStar, Star);

// Domain identification.
const char* SRStar::domain() const
{
  return SRdomainName;
}


// Prepare the star for a new instant
//
// @Description  Clear all the output ports to unknown and reset the
// hasFired flag.
//
// <P> Derived stars may override this to provide different behavior.
// Schedulers call this exactly once for each star at the beginning of
// each instant.

void SRStar::initializeInstant()
{

  PortHole * p;
  BlockPortIter portIter(*((Block *) this) );
  while ( (p = portIter++) != 0 ) {
    if ( p->isItOutput() ) {
      ((OutSRPort *) p)->clearPort();
    }
  }

  hasFired = 0;

}

// Advance a star's state at the end of an instant
//
// @Description Default does nothing.  Derived stars should override
// this with a method that advances a star's state based on its
// inputs and computed outputs.  Schedulers call this exactly once at
// the end of each instant for every star.  This may not be called
// for reactive stars in instants where they have no present inputs. <P>
//
// Zero-input reactive stars always fire.

void SRStar::tick()
{
}

// Compute the output of the star
//
// @Description For this strict star, it calls go() at most once an
// instant when all its non-independent inputs are known, and at least
// one is present if it is a reactive star.

int SRStar::run()
{
  if ( !hasFired ) {

#ifdef DEBUG_SRSTAR
    cout << "Trying to run " << name() << " isReactive:" << isReactive <<'\n';
#endif

    int ableToFire = 1;
    int presentInputs = 0;
    int numInputs = 0;

    BlockPortIter nextPort(*((Block *) this) );
    PortHole * p;

    while ( ( p = nextPort++) != 0 ) {
#ifdef DEBUG_SRSTAR
      cout << "Checking " << p->name() << '\n';
#endif
      if ( p->isItInput() ) {
	numInputs++;
	if ( !((InSRPort *) p)->isItIndependent() &&
	     !((InSRPort *) p)->known() ) {
#ifdef DEBUG_SRSTAR
	  cout << "unknown dependent input\n";
#endif
	  ableToFire = 0;
	  break;
	} else {
	  if ( ((InSRPort *) p)->present() ) {
	    presentInputs = TRUE;
#ifdef DEBUG_SRSTAR
	    cout << "present input\n";
#endif
	  }
	}
      }
    }

    if ( ableToFire ) {
      if (!isReactive || numInputs == 0 || presentInputs != 0) {

	// All of the inputs are known, and if the star is reactive,
	// there is at least one present input--call go()

#ifdef DEBUG_SRSTAR
	cout << "Reactive with at least one present input: calling go()\n";
#endif
	Star::run();

      } else {

	// All of the inputs are known, but none are present and the
	// star is reactive--mark all outputs as absent and don't call go()

#ifdef DEBUG_SRSTAR
	cout << "Reactive with no present inputs -- outputs made absent\n";
#endif      
	nextPort.reset();
	while ( ( p = nextPort++ ) != 0 ) {
	  if ( p->isItOutput() ) {
	    ((OutSRPort *)p)->makeAbsent();
	  }
	}	
      }

      hasFired = 1;
    }

  }

  return 0;
}

// Return the number of known outputs
int SRStar::knownOutputs()
{
  int number = 0;

  BlockPortIter nextPort(*((Block *) this) );
  PortHole * p;

  while ( ( p = nextPort++) != 0 ) {
    if ( p->isItOutput() ) {
      if ( ((InSRPort *) p)->known() ) {
	number++;
      }
    }
  }

  return number;

}

// Initialize the star in preparation for simulation
//
// @Description Resets the isReactive flag and calls Star::initialize
void SRStar::initialize()
{
  isReactive = 0;

  Star::initialize();
}
