#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "nle.h"

/* open chip, get the bag "NLE_BAG", and merge the cells inside */

/*ARGSUSED*/
int
main(argc,argv)
int argc;
char **argv;
{
  octObject Bag, NewChip, Chip, NewInstance;
  char *input_cell, *input_view;

  /* parse command line options*/

  /* XXX ohUnpackFacetName */

  input_cell = util_strsav(argv[1]);
  input_view = strchr(input_cell, ':');
  if (input_view != (char *) 0) {
    *input_view++ = '\0';
  } else {
    input_view = "symbolic";
  }

  Chip.type = OCT_FACET;
  Chip.contents.facet.cell = input_cell;
  Chip.contents.facet.view = input_view;
  Chip.contents.facet.facet = "contents";
  Chip.contents.facet.version = OCT_CURRENT_VERSION;
  Chip.contents.facet.mode = "a";

  OH_ASSERT(octOpenFacet(&Chip));
  
  OH_ASSERT(ohGetByBagName(&Chip, &Bag, "NLE_BAG"));

  NewChip.type = OCT_FACET;
  NewChip.contents.facet.cell = "new";
  NewChip.contents.facet.view = "symbolic";
  NewChip.contents.facet.facet = "contents";
  NewChip.contents.facet.version = OCT_CURRENT_VERSION;
  NewChip.contents.facet.mode = "a";

  OH_ASSERT(octOpenFacet(&NewChip));
  OH_ASSERT(octFlushFacet(&NewChip));	/* commit */

  (void) nleMakeMacro(&NewChip, &Bag, &NewInstance, NLE_HARD);

  /* (void) nleFlatInstance(&NewInstance, &Bag, NLE_HARD); */

  OH_ASSERT(octCloseFacet(&Chip)); 
  OH_ASSERT(octCloseFacet(&NewChip)); 

  exit(0);
}

