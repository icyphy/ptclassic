#ifndef _ACSStar_h
#define _ACSStar_h

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

    Author:	Eric Pauer (Sanders) and Christopher Hylands
    Created:	15 January 1998

*/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"

// The following include is not really required for this file,
// but any star for this domain will need it, so we include it here.
#include "ACSPortHole.h"

class ACSStar : public CGStar
{
public:
    // Class identification.
    /*virtual*/ int isA(const char*) const;

    // Domain identification.
    /*virtual*/ const char* domain() const;

    // For simulation we just call the base-class simulation star.
    // for code-generation we call something else.
    int run() { return(DataFlowStar::run()); } // FIXME
};

#endif
