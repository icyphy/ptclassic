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
 * OCT Region Generation Package Include File
 *
 * David Harrison
 * University of California, 1985
 *
 * This file contains definitions used by the region package.
 * There are two levels to this package:  the low level
 * routines and the high level interface.  The low level
 * routines are used to generate items attached to an object
 * that are in a given region.  The high level routines
 * provide a more convienent (and policy specific) interface.
 */

#ifndef REGION
#define REGION

#include "oct.h"

extern char *region_pkg_name;

/*
 * Error codes
 */

typedef int regStatus;

#define REG_OK		1
#define REG_FAIL	0

#define REG_NOMORE	-1

/* Possible fatal faults */
#define REG_MEM		1	/* No memory         */
#define REG_OCT		2	/* Generic oct error */
#define REG_UCR		3	/* Bad change record */
#define REG_KDE		4	/* Generic k-d error */
#define REG_BA		5	/* Bad argument      */
#define REG_NIF		6	/* No interface facet*/
#define REG_CON		7	/* Inconsistent facets */
#define REG_BDB		8	/* Bad change DB     */

#define REG_UNKNOWN	99	/* Unknown error     */

/*
 * Below are the definitions for the low-level interface
 */

typedef char *reg_generic;

typedef struct reg_gen {
    reg_generic state;		/* Generator state            */
    regStatus (*gen_func)();	/* Generator                  */
    void (*end_func)();		/* End function               */
} regGenerator;

extern void regGetPoints();
  /* Gets the points of a path or polygon */

extern void regInit();
  /* Initializes generation sequence */

#define regNext(generator, object, num_points, points, size) \
  (((generator)->gen_func)((generator), (object), (num_points), (points), (size)))
  /* Generates the next object in a sequence */

#define regEnd(generator) \
  ((generator)->end_func)(generator)
  /* Ends a generation sequence */

extern char *regErrorString();
  /* Produces an error message if things go wrong */

/*
 * Below are the definitions for the high level interface
 */

/* Can be typedef'ed to void if supported */
typedef struct obj_gen_dummy_defn {
    int dummy;
} obj_gen_dummy;

typedef obj_gen_dummy *regObjGen;

extern regStatus regObjStart();
   /* Initializes generation of geometries */
extern regStatus regObjNext();
   /* Returns next item in sequence */
extern void regObjFinish();
   /* Ends a generation sequence */

typedef struct reg_term_info {
    octObject term;		/* Terminal     */
    struct octBox bb;		/* Bounding box */
} regTermInfo;

extern regStatus regMultiActual();
   /* Finds all actual terminals which intersect a box */
extern regStatus regFindActual();
   /* Finds an actual terminal at a given point */
extern regStatus regSegActuals();
   /* Finds actual terminals that intersect a segment */
extern regStatus regFindImpl();
   /* Finds a terminal's implementation bounding box */
extern regFindLyrImpl();
   /* Finds layers of terminal's implementation */
extern regStatus regFindNet();
   /* Finds a major net associated with an object */


#endif
