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
 * VEM Buffer Management Definitions
 *
 * David Harrison
 * University of California, Berkeley
 * 1986, 1989
 *
 * This file contains interface definitions for the buffer management
 * package implemented in the buffers subdirectory.
 */

#ifndef BUFFER
#define BUFFER

#include "ansi.h"
#include "general.h"
#include "tech.h"
#include "bindings.h"
#include "oct.h"

/*
 * Buffers are associated to facets by Oct object identifiers.
 *
 * The following routines open new buffers (open.c)
 */

#define BUFMUSTEXIST	0x01	/* Buffer must exist                  */
#define BUFSILENT	0x02	/* Do not complain about bad open     */
#define BUFREADONLY	0x04	/* Open the buffer for read only      */

extern vemStatus bufOpen
  ARGS((octObject *newFacet, int options));
  /* Opens a new buffer */

extern vemStatus bufOpenMaster
  ARGS((octObject *inst, octObject *newFacet, int options));
  /* Opens the master of an instance */

/*
 * Closing, Saving, and Reverting Buffers
 *
 * (save.c)
 */

extern vemStatus bufSave
  ARGS((octId fctId));
   /* Saves a specified buffer */

extern vemStatus bufRevert
  ARGS((octId *fctId));
   /* Rereads contents of facet from disk */

extern vemStatus bufClose
  ARGS((octId fctId));
   /* Close a buffer freeing all resources */

extern vemStatus bufSaveAll();
   /* Saves all modified buffers */

extern void bufTechFlush();
   /* Frees technology and layer information about all buffers */

extern vemStatus bufAutoSave
  ARGS((octId fctId));
  /* Causes the specified facet to be autosaved */

extern vemStatus bufRecover
  ARGS((octId *fctId, char *version));
   /* Recovers cell from crash or autosave */

/*
 * Tracking changes to Buffers
 *
 * (change.c)
 */

extern vemStatus bufChanges();
   /* Examines all changes since last called and does appropriate updates */

/*
 * Layer Table Routines
 *
 * (layer.c)
 */

extern vemStatus bufLyrId
  ARGS((octId fct_id, tkLayer tkl, octId *lyr_id));
   /* Returns Oct layer id given tkLayer */

extern tkLayer bufHighLayer
  ARGS((octId fct_id));
   /* Returns highest priority layer in buffer */

extern octCoord bufPathWidth
  ARGS((octId fctId, octObject *lyr));
   /* Returns the current path width for a layer in this buffer */

extern vemStatus bufSetWidth
  ARGS((octId fctId, octObject *lyr, octCoord width));
   /* Sets the current path width for a layer in this buffer    */

/*
 * General Query/Modification Routines
 *
 * (general.c)
 */

extern vemStatus bufInstances
  ARGS((octId fctId, vemStatus (*func)(), char *arg));
   /* Call function for all instances of a buffer */

extern vemStatus bufBinding
  ARGS((octId fctId, bdTable *theBinding));
   /* Returns main binding table associated with buffer */

extern vemStatus bufLock
  ARGS((octId fctId));
   /* Makes a buffer unwritable or writable */

extern vemStatus bufWritable
  ARGS((octId fctId));
   /* Returns VEM_OK if the buffer is writable */

#define BUF_NO_TYPE	0
#define BUF_PHYSICAL	1
#define BUF_SYMBOLIC	2
#define BUF_SCHEMATIC	3

extern int bufStyle
  ARGS((octId fctId));
   /* Returns one of the editing styles defined above */

extern vemStatus bufTech
  ARGS((octId fctId, tkHandle *tech));
   /* Returns the technology associated with a buffer */

#define BUF_MAX_ALTERNATES	2

extern int bufCheckRecover
  ARGS((octId fct_id, int ver_len, char *versions[]));
  /* Returns more recent alternate versions of a cell */

extern vemStatus bufModified
  ARGS((octId fctId));
  /* Returns VEM_OK if the buffer has been modified */

/* Really this should be internal, but physical/createmod.c, rpc/vemRpc.c
 * and files in symbolic all use it.
 * (bufinternal.h)
 */
void bufMarkActive
    ARGS((octId id));


#endif

