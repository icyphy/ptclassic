/* -*- C++ -*- */

#ifndef _SRStar_h
#define _SRStar_h

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
#pragma interface
#endif

#include "Star.h"

// The following include is not really required for this file,
// but any star for this domain will need it, so we include it here.
#include "SRPortHole.h"

class SRStar : public Star {

private:

  // flag indicating whether the star has fired in the instant
  // Used by strict stars to ensure they fire at most once in an instant
  int hasFired;

  // flag indicating whether the star is reactive--only producing
  // present outputs when at least one input is present
  int isReactive;

protected:

  // Mark this star as reactive--usually done within a star's setup() method
  void reactive() { isReactive = 1; }

public:
  // Class identification.
  /*virtual*/ int isA(const char*) const;

  // Domain identification.
  /*virtual*/ const char* domain() const;

  // Inter-instant time advancement
  // Default is to do nothing; derived non-strict stars should override this
  virtual int tick();

  // Beginning-of-instant initialization
  virtual void initializeInstant();

  // Intra-instant time advancement
  /*virtual*/ int run();  

  // Return the status of the reactive flag
  int isItReactive() const { return isReactive; }

  // Return the count of known outputs
  int knownOutputs();

  // Initialize all default values
  void initialize();

};

#endif
