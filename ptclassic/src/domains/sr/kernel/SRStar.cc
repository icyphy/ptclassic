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
    Created:	14 April 1996

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRStar.h"
#include "Block.h"

extern const char SRdomainName[];

// Class identification.
ISA_FUNC(SRStar, Star);

// Domain identification.
const char* SRStar::domain() const
{
  return SRdomainName;
}

// Initialize the star for the beginning of an instant:
//  Clear all the output ports
//  Reset the hasFired flag
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

// Inter-instant time advancement
// By default, do nothing
void SRStar::tick()
{
}

// Inter-instant time advacement
// For this strict star, it calls go() at most once an instant when
// all its inputs are known, and at least one is present if it's a
// reactive star
int SRStar::run()
{
  if ( !hasFired ) {

    int ableToFire = 1;
    int presentInputs = 0;

    BlockPortIter nextPort(*((Block *) this) );
    PortHole * p;

    while ( ( p = nextPort++) != 0 ) {
      if ( p->isItInput() ) {
	if ( !((InSRPort *) p)->known() ) {
	  ableToFire = 0;
	  break;
	} else {
	  if ( ((InSRPort *) p)->present() ) {
	    presentInputs = TRUE;
	  }
	}
      }
    }

    if ( ableToFire ) {
      if (!isReactive || presentInputs ) {

	// All of the inputs are known, and if the star is reactive,
	// there is at least one present input--call go()

	go();

      } else {

	// All of the inputs are known, but none are present and the
	// star is reactive--mark all outputs as absent and don't call go()
      
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

// Initialize all domain-specific flags
void SRStar::initialize()
{
  Star::initialize();

  isReactive = 0;
}
