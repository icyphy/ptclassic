/*  Remove the objects contained in the DESIGN_RULES bag the uses of which I 
don't understand.  */

#include <stdio.h>
#include "oct.h"
#include "oh.h"

cleantap(tapName)
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
