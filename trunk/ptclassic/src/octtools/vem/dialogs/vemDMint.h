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
 * VEM Dialog Manager Interface Routines
 * 
 * David Harrison
 * University of California,  Berkeley
 * 1986, 1989, 1990
 *
 * This file contains easy to use programming interfaces for common
 * dialog operations.  These are usually short moded dialogs which
 * ask questions that require an answer immediately.  The prefix of
 * these routines is "dm".
 *
 * This package is now implemented on top of the more powerful
 * Xt toolkit based package "dds".  It is provided for
 * compatibility.  The library must be initialized using
 * dmCompatInit().
 *
 * The routines defined here are internal for use by the rest of
 * the package.
 */

#ifndef _VEMDMINT_H_
#define _VEMDMINT_H_

#include "ansi.h"
#include "port.h"
#include "dds.h"
#include "errtrap.h"
#include "vemDM.h"

extern char *vemdm_pkg_name;
extern int _vemdm_initialized;
extern ddsEvtHandler _dmCompatHandler;

#define SCROLL_THRESHOLD 10
#define MAX_HELP_COLS	 60
#define MAX_EDIT_TEXT	 16
#define MAX_ENUM_ITEMS	 12
#define MAX_BUF		1024

typedef struct con_callback_defn {
    vemStatus status;		/* Returned status */
    ddsHandle help;		/* Help (if any)   */
} con_callback;

extern void _vemdm_general_ok
  ARGS((ddsHandle item));
  /* For "ok" buttons */

extern void _vemdm_general_cancel
  ARGS((ddsHandle item));
  /* For "cancel" buttons */

extern void _vemdm_general_no
  ARGS((ddsHandle item));
  /* For "No" button in confirm */

extern void _vemdm_preserve_ok
  ARGS((ddsHandle item));
  /* For "ok" buttons that don't immediately destroy */

extern void _vemdm_preserve_cancel
  ARGS((ddsHandle item));
  /* For "cancel" buttons that don't immediately destroy */

#endif /* _VEMDMINT_H_ */
