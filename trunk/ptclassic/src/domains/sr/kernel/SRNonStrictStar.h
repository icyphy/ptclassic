/* -*- C++ -*- */

#ifndef _SRNonStrictStar_h
#define _SRNonStrictStar_h

/*  Version @(#)SRNonStrictStar.h	1.1 4/16/96

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
    Created:	15 April 1996

    This file is really a workaround for ptlang, which insists
    on a different header file for each class (see the "derivedfrom"
    rule).

*/

#ifdef __GNUG__
#pragma interface
#endif

#include "SRStar.h"

/**********************************************************************

  Non-strict "template" star for the SR domain

  @Description A non-strict star can always fire, not just when all its
  inputs are known.  Strict stars (i.e., those derived from the SRStar class)
  are easier to code and use, but may cause deadlock when placed in
  zero-delay feedback loops.

  @SeeAlso SRStrictStar

 **********************************************************************/
class SRNonStrictStar : public SRStar {
public:
  // Identify the class
  int isA(const char*) const;

  // Identify the domain
  const char* domain() const;

  int run();
};

#endif
