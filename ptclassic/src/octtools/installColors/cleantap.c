/*
Copyright (c) 1990-%Q% The Regents of the University of California.
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
*/
/*  Remove the objects contained in the DESIGN_RULES bag the uses of which I 
don't understand.  */

#include <stdio.h>
#include "oct.h"
#include "oh.h"
#include "cleantap.h"

void cleantap(tapName)
char *tapName;
{
    octObject tapFacet, designBag, minBag;
    octGenerator gen;

    octBegin();
    /* Assume schematic view! */
    ohOpenFacet(&tapFacet, tapName, "schematic", "contents", "a");
    ohGetByBagName(&tapFacet, &designBag, "DESIGN-RULES");
    octInitGenContents(&designBag, OCT_BAG_MASK, &gen);
    while (octGenerate(&gen, &minBag) == OCT_OK) {
        ohDeleteContents(&minBag, OCT_EDGE_MASK);
        octDelete(&minBag);
    }
    octFreeGenerator(&gen);
    octCloseFacet(&tapFacet);
    octEnd();
}
