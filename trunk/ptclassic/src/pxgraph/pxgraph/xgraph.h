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

#include <malloc.h>
#include "compat.h"		/* Pick up common decls (sprintf() etc) */

#define VERSION_STRING	"11.3.3 November 1994"

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
