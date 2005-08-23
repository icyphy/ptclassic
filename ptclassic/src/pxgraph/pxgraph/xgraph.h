/*
Copyright (c) 1990-1999 The Regents of the University of California.
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
 * Globally accessible information from xgraph
 */

#ifndef _XGRAPH_H_
#define _XGRAPH_H_

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xresource.h>	/* Pick up XrmUniqueQuark() for sol2 */

#include <stdio.h>              /* Pick up sprintf() */
#include <malloc.h>

/* If you change this, then update the version in configure.in */
#define VERSION_STRING	"11.3.4 (Build: @(#)xgraph.h	1.17 04/29/99)"

#define MAXKEYS		50
#define MAXATTR 	8
#define MAXSETS		64
#define MAXBUFSIZE 	120
#define MAXLS		50

#define STRDUP(xx)	(strcpy(malloc((unsigned) (strlen(xx)+1)), (xx)))

#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args)	args
#else
#define ARGS(args)	()
#endif


typedef unsigned long Pixel;

/* Globally accessible values */
extern Display *disp;			/* Open display            */
extern Visual *vis;			/* Standard visual         */
extern Colormap cmap;			/* Standard colormap       */
extern int screen;			/* Screen number           */
extern int depth;			/* Depth of screen         */

extern void do_hardcopy();	/* Carries out hardcopy    */
extern void ho_dialog();	/* Hardcopy dialog         */
extern void set_X();		/* Initializes X device    */

extern char *strcpy();
extern char *strcat();
extern char *strrchr();
extern char *strchr();
/* 
 * extern char *malloc();
 * extern char *realloc();
 * extern void exit();
 * extern void free();
 * extern void abort();
 */
/* Apparently in linux Slackware 2.1, "extern double atof()" fails
   because Linux stdlib.h makes atof a macro.  Easiest solution: Add
   #undef atof to the xgraph.h file anywhere before atof mention.
 */
#ifdef atof
#undef atof
#endif

extern double atof();


#endif /* _XGRAPH_H_ */
