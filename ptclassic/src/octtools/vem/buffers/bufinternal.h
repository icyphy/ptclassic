/* Version Identification:
 * $Id$
 */
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/*
 * VEM Buffer Management
 * Shared definitions
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * This file contains definitions shared across all of the files
 * used to implement the buffer module.
 */

#ifndef BUF_I_H
#define BUF_I_H

#include "ansi.h"		/* Ansi declarations    */
#include "buffer.h"		/* Self declaration     */
#include "st.h"			/* Hash table           */

#ifndef OLD_DISPLAY
#include "gu.h"			/* Geometric updates    */
#endif

/*
 * Autosave and crash save defines
 */

#define AUTO_SAVE_VERSION	"autosave"
#define CRASH_SAVE_VERSION	"crash"

/*
 * Layer specific information
 *
 * This structure is stored in the `lyr_tbl' hash table in
 * the main buffer structure.
 */

typedef struct buf_lyr_defn {
    octId lyr_id;		/* Oct Id of corresponding layer */
    octCoord path_width;	/* Default path width            */
#ifndef OLD_DISPLAY
    gu_marker lyr_changes;	/* Changes on this layer         */
#endif
} buf_lyr;

/* Internal option for bufOpen */
#define TRY_READ_ONLY	0x10	/* Try opening for read only */

/* Buffer status bits (low-order 3 bits are buffer type) */
/* The actual types are defined in buffer.h              */

#define TYPE_BITS	2
#define BEYOND_TYPE	(1<<TYPE_BITS)
#define TYPE_MASK	(BEYOND_TYPE-1)

/* Status bits themselves */

#define	READ_ONLY	BEYOND_TYPE	/* Buffer is physically unwritable  */
#define WRITE_PROTECT	(READ_ONLY<<1)	/* Buffer is marked unwritable      */
#define FACET_CORRUPT	(WRITE_PROTECT<<1)/* Something terrible has happened  */
#define LYR_UPDATE	(FACET_CORRUPT<<1)/* Layer table requires updating  */

/* Facet ids are used to hash into a table of these structures: */

typedef struct intern_buffer_defn {
    bdTable mainTable;		/* Primary command bindings          */
    tkHandle tech;		/* Technology associated with buffer */
    st_table *lyr_tbl;		/* Table indexed by tkLayer          */
    octId hi_lyr;		/* Id of highest priority layer      */
    int bufBits;		/* Buffer status bits                */
    st_table *inst_table;	/* Hash table of instances           */
#ifndef OLD_DISPLAY
    octId layer_changes;	/* Creation and deletion of layers   */
    gu_marker inst_changes;	/* Changes to instances              */
#else
    octId buf_changes;		/* Changes to this buffer            */
#endif
    long change_count;		/* Total number of changes to buffer */
} intern_buffer;

/* Global hash table of all buffers */
extern st_table *_bufTable;
extern st_table *_activeBufTable;

/*
 * Local routines
 */

void bufMarkActive
    ARGS((octId id));

extern intern_buffer *_bufFindBuffer
  ARGS((octId id));
   /* Returns an internal buffer given a facet id (bufinternal.c) */

extern vemStatus _bufSetValues
  ARGS((octObject *newFacet, int lockFlag, intern_buffer *newIntern));
   /* Sets (resets) values in an internal buffer (open.c) */

extern void _bufMrkLayers
  ARGS((octId fctId, intern_buffer *buf));
  /* Erases layer table information for a buffer and reconstructs (layers.c) */

extern void _bufMakeLayers
  ARGS((octId fctId, intern_buffer *buf));
   /* Fills in the layer list for a buffer (layers.c) */

extern void _bufFreeLayers
  ARGS((st_table *lyr_tbl));
   /* Reclaims layer table resources (layers.c) */

/*
 * Change monitoring subsystem
 */

#ifndef OLD_DISPLAY

extern void _bufChInit
  ARGS((intern_buffer *buf));
  /* Initializes change list fields in `buf' */

extern void _bufRCFree
  ARGS((intern_buffer *buf));
  /* Frees change list fields in `buf' */

extern void _bufLCFree
  ARGS((buf_lyr *lyr_info));
  /* Frees change list resources in layer table */

extern void _bufExChange
  ARGS((octObject *fct, intern_buffer *buf));
  /* Examines changes for the specified buffer */

#else

extern octId _bufNewChanges
  ARGS((octObject *fct));
   /* Returns a new change list (changes.c) */

extern void _bufDelChanges
  ARGS((octId changeId));
   /* Deletes a change list  (changes.c)    */

#endif

#endif
