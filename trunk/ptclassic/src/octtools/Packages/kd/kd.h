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
 * Headers for k-d tree geometric data structure
 */

#ifndef KD_HEADER
#define KD_HEADER

#include "ansi.h"
#include "copyright.h"

extern char *kd_pkg_name;	/* For error handling */

typedef struct kd_dummy_defn {
    int dummy;
} kd_dummy;

typedef kd_dummy *kd_tree;
typedef kd_dummy *kd_gen;

#define KD_LEFT		0
#define KD_BOTTOM	1
#define KD_RIGHT	2
#define KD_TOP		3
#define KD_BOX_MAX	4

typedef int kd_box[4];
typedef int *kd_box_r;

typedef int kd_status;
typedef char *kd_generic;

/* Return values */

#define KD_OK		1
#define KD_NOMORE	2

#define KD_NOTIMPL	-3
#define KD_NOTFOUND	-4

/* Fatal Faults */
#define KDF_M		0	/* Memory fault    */
#define KDF_ZEROID	1	/* Insert zero     */
#define KDF_MD		2	/* Bad median      */
#define KDF_F		3	/* Father fault    */
#define KDF_DUPL	4	/* Duplicate entry */
#define KDF_UNKNOWN	99	/* Unknown error   */

extern char *kd_err_string();
  /* Returns a textual description of a k-d error */

extern kd_tree kd_create();
  /* Creates a new empty kd-tree */

extern kd_tree kd_build
  ARGS((int (*itemfunc)(), kd_generic arg));
  /* Makes a new kd-tree from a given set of items */

extern void kd_destroy
  ARGS((kd_tree this_one, void (*delfunc)()));
  /* Destroys an existing k-d tree */

extern kd_status kd_is_member
  ARGS((kd_tree theTree, kd_generic data, kd_box size));
  /* Tries to find a specific item in a tree */

extern void kd_insert
  ARGS((kd_tree theTree, kd_generic data, kd_box size));
  /* Inserts a new node into a k-d tree */

extern kd_status kd_delete
  ARGS((kd_tree theTree, kd_generic data, kd_box old_size));
  /* Deletes a node from a k-d tree */

extern kd_gen kd_start
  ARGS((kd_tree theTree, kd_box area));
  /* Initializes a generation of items in a region */

extern kd_status kd_next
  ARGS((kd_gen theGen, kd_generic *data, kd_box size));
  /* Generates the next item in a region */

extern void kd_finish
  ARGS((kd_gen theGen));
  /* Ends generation of items in a region */

extern int kd_count
  ARGS((kd_tree tree));
  /* Returns the number of objects stored in tree */

#endif /* KD_HEADER */
