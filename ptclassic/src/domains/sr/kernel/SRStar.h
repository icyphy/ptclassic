/* -*- C++ -*- */

#ifndef _SRStar_h
#define _SRStar_h

/*  Version $Id$

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
#pragma interface
#endif

#include "Star.h"

// The following include is not really required for this file,
// but any star for this domain will need it, so we include it here.
#include "SRPortHole.h"

/**********************************************************************

 The star for the SR domain

  @Description The inputs to this star are wires whose values may be
  unknown, absent, or present with some value.  The go() method should
  return a function of these wires (i.e., the star should not change
  its state), but the tick() method, called at the end of each
  instant, should update the state of the star based on inputs and
  outputs.  The go() method of stars derived from this class is only
  called when none of the inputs are unknown.  SRNonStrictStar
  overrides this, allowing some inputs to be unknown.

**********************************************************************/
class SRStar : public Star {

private:

  // Flag indicating whether the star has fired in the instant
  //
  // @Description Used by strict stars to ensure they fire at most
  // once in an instant

  int hasFired;

  // Flag indicating whether the star is reactive
  //
  // @Description A reactive star only produces present outputs and
  // updates its statewhen at least one input is present

  int isReactive;

protected:

  // Mark this star as reactive
  //
  // @Description A reactive star will not produce any present inputs
  // nor change its state if none of its inputs are present.
  // 
  // <P> Usually done within a star's setup() method

  void reactive() { isReactive = 1; }

public:
  // Identify the class
  int isA(const char*) const;

  // Identify the domain
  const char* domain() const;

  virtual void tick();

  virtual void initializeInstant();

  int run();  

  // Return the status of the reactive flag
  int isItReactive() const { return isReactive; }

  int knownOutputs();

  void initialize();

};

#endif
