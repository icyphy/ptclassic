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
 * X Constants - Header definitions
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * Definitions for accessing X constants exported by xvals.c.
 */

#ifndef _XV_H_
#define _XV_H_

#include "ansi.h"
#include "port.h"
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

extern char *xv_pkg_name;

/* Fatal errors */
#define XV_MULTI_SET	-1	/* Value set twice */
#define XV_NOT_SET	-2	/* Value not set   */

/*
 * Getting values
 */

Display *xv_disp();
   /* Returns display connection */
int xv_scrn();
   /* Returns display screen */
Visual *xv_vis();
   /* Returns appropriate visual */
Colormap xv_cmap();
   /* Returns appropriate colormap */
Drawable xv_drw();
   /* Returns appropriate drawable */
int xv_depth();
   /* Returns appropriate depth */
Widget xv_topw();
   /* Returns top level shell widget */

/*
 * Setting values
 *
 * These values are set once by VEM initialization routines.
 */

void xv_set_disp
  ARGS((Display *disp));
void xv_set_scrn
  ARGS((int scrn));
void xv_set_vis
  ARGS((Visual *vis));
void xv_set_cmap
  ARGS((Colormap cmap));
void xv_set_drw
  ARGS((Drawable drw));
void xv_set_depth
  ARGS((int depth));
void xv_set_topw
  ARGS((Widget widget));

#endif /* _XV_H_ */
