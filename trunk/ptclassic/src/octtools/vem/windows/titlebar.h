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
 * Title Bar Definitions
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1989
 *
 * This file contains definitions for using the title bar maintenence
 * module.
 */

#ifndef TB_INCLUDE
#define TB_INCLUDE

#include "general.h"
#include <X11/Xlib.h>
#include "ansi.h"
#include "oct.h"

extern char *tb_pkg_name;

typedef struct tb_void_defn {
    int dummy;
} tb_void;

typedef tb_void *tbHandle;

extern void tbGrab
  ARGS((Display *disp, Window win, int lambda, tbHandle *rtn_handle,
	int *rtn_height));
  /* Takes over ownership of a previously created window */

extern void tbRelease
  ARGS((tbHandle tb));
  /* Releases control of a previously grabbed window */

extern void tbRedraw
  ARGS((tbHandle tb));
  /* Redraws the contents of a active title bar window */

extern void tbResize
  ARGS((tbHandle tb, int w, int h));
  /* Change the size of a active title bar window */

extern void tbDeactivate
  ARGS((tbHandle tb));
  /* Marks a window as inactive until it receives a tbRedraw() */

#define TB_EDIT_CELL	0x01	/* Cell being edited      */
#define TB_CONTEXT_CELL	0x02	/* Context cell name      */
#define TB_STAT_ICONS	0x04	/* Cell status icons      */
#define TB_DIFF_COORDS	0x08	/* Difference coordinates */
#define TB_ABS_COORDS	0x10	/* Absolute coordinates   */
#define TB_RPC_NAME     0x20    /* RPC application name   */

extern void tbControl
  ARGS((tbHandle tb, int on_bits, int off_bits, int tog_bits, int *rtn_bits));
  /* Sets the redisplay state of a title bar window */

extern void tbEditCell
  ARGS((tbHandle tb, octObject *cell));
  /* Changes the name of the currently edited cell */

extern void tbContextCell
  ARGS((tbHandle tb, octObject *cell));
  /* Changes the name of the context cell */

extern void tbRPCName
  ARGS((tbHandle tb, STR name));
  /* Chagnes the name of the RPC application */

extern STR tbGetRPCName
  ARGS((tbHandle tb));
  /* Returns the currently stored RPC name in title bar */

extern void tbPoint
  ARGS((tbHandle tb, struct octPoint *pnt, int a_flag));
  /* Given a coordinate in Oct space,  updates the position and difference */

extern void tbSpot
  ARGS((tbHandle tb, struct octPoint *pnt));
  /* Sets the location for the difference display */

extern void tbLambda
  ARGS((tbHandle tb, int lambda));
  /* Change the Oct units per lambda for titlebar */

#define TB_READ_ONLY	0x01	/* Cell cannot be modified  */
#define TB_RPC_APPL	0x02	/* RPC Application attached */
#define TB_MIS_LAYERS	0x04	/* Missing layers           */
#define TB_INTERRUPT	0x08	/* Window has been deactivated */

extern void tbBits
  ARGS((tbHandle tb, int on_bits, int off_bits, int tog_bits, int *rtn_bits));
  /* Sets the status icons of a title bar */

#endif
