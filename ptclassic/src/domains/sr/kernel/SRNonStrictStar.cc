static const char file_id[] = "SRNonStrictStar.cc";

/*  Version @(#)SRNonStrictStar.cc	1.1 4/16/96

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

#include "SRNonStrictStar.h"
#include "Block.h"

extern const char SRdomainName[];

// Class identification.
ISA_FUNC(SRNonStrictStar, Star);

// Domain identification.
const char * SRNonStrictStar::domain() const
{
  return SRdomainName;
}

// Compute the output of the star
//
// @Description For this non-strict star, it always calls go().

int SRNonStrictStar::run()
{
  return Star::run();
}
