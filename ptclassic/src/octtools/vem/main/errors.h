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
 * Error Package
 */

#ifndef _VEM_ERR_H_
#define _VEM_ERR_H_

#include "ansi.h"
#include "general.h"
#include "X11/Xlib.h"
#include "errtrap.h"

extern int vemFail
  ARGS((char *name, char *filename, int line));
   /* Prints out classic yes, no, abort message */

extern int vemOctFail
  ARGS((char *filename, int line));
   /* Prints out oct assertion failure message */

extern int vemMultiFail
  ARGS((char *title, int numopts, char *options[], char *filename, int line));
   /* Prints out message,  offers user several options */

extern int vemXError
  ARGS((Display *theDisp, XErrorEvent *theError));
   /* Prints out X error message */

extern void vemXtError
  ARGS((char *msg));
  /* Prints out an Xt error message */

/*
 * Errtrap support
 */

void vemPushErrContext
  ARGS((jmp_buf env));
   /* Pushes setjmp environment */
void vemPopErrContext();
   /* Pops setjmp environment */
void vemDefaultTrapHandler
  ARGS((STR name, int code, STR message));
   /* Displays dialog with fatal message */

#endif /* _VEM_ERR_H_ */
