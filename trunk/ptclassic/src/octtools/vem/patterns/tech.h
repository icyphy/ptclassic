/* Version Identification:
 * $Id$
 */
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
/*
 * Technology Maintainence Package Header File
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * This file contains definitions for using the technology
 * maintainence package.  This package is built on top
 * of TAP (technology access package) written by T. Laidig.
 */

#ifndef TECHHEADER
#define TECHHEADER

#include "general.h"		/* General VEM defns */
#include "oct.h"		/* Database          */
#include "attributes.h"		/* Attribute handler */

/* Exported type for handing a technology */

/* Can be typedef'ed to void if supported */
typedef struct tk_dummy_def {
    int dummy;
} tk_dummy;

typedef tk_dummy *tkHandle;

/* Exported type for a layer in a technology */
typedef struct tk_lyr_defn {
    int priority;		/* Layer priority */
} tk_lyr;

typedef tk_lyr *tkLayer;


/*
 * Exported functions and procedures
 */

extern tkHandle tkGet ARGS((octObject *theFacet));
  /* Returns a technology given an Oct facet */
extern void tkFree ARGS((tkHandle tech));
  /* Frees resources associated with a technology handle */
extern void tkWipe ARGS(());
  /* Remove cached information on all known technologies */

extern vemStatus tkIsTapFacet ARGS((octObject *fct));
  /* Returns VEM_OK if the supplied facet is a tap facet */

extern tkLayer tkLyr ARGS((tkHandle tech, octObject *lyrObj));
  /* Retrieves a layer from a technology given an Oct layer */

extern atrHandle tkAttr ARGS((tkLayer lyr, int alt));
  /* Gets an attribute from a technology layer */

extern vemStatus tkMinWidth ARGS((tkHandle tech, STR lyrName, octCoord *minwidth));
  /* Minimum width of a named layer */

#endif
