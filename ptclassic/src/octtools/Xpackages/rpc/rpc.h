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
#ifndef RPC_H
#define RPC_H

/*
 * Remote Procedure Call Package for VEM (RPC)
 *
 * Copyright (c) 1986, 1987, 1988, 1989, Rick L Spickelmier.  All rights reserved.
 *
 * Use and copying of this software and preparation of derivative works
 * based upon this software are permitted.  However, any distribution of
 * this software or derivative works must include the above copyright
 * notice.
 *
 * This software is made available AS IS, and neither the Electronics
 * Research Laboratory or the University of California make any
 * warranty about the software, its performance or its conformity to
 * any specification.
 *
 * Suggestions, comments, or improvements are welcome and should be
 * addressed to:
 *
 *   Rick L Spickelmier
 *   558 Cory Hall
 *   University of California
 *   Berkeley, CA   94720
 *
 *   rpc@eros.Berkeley.EDU  (ARPANET)
 *   ..!ucbvax!eros!rpc        (UUCP)
 *
 */

#ifndef PORT_H
#include "port.h"
#endif

#ifndef OCT_H
#include "oct.h"
#endif

#include <X11/Xlib.h>
#ifdef PTSVR4
#include "utility.h"
#endif

extern long UserMain();

typedef char *STR;
typedef char *Pointer;		/* Pointer to anything   */
typedef int vemStatus;		/* Status codes          */

/* VE status codes.  Negative codes are fatal.  Positive are non-fatal. */

#define VEM_OK		0

#define VEM_FALSE	1	/* Predicate failure value       */
#define VEM_NOMORE	2	/* Generator is exhausted 	 */
#define VEM_DUPLICATE	3	/* Same name used more than once */
#define VEM_ZEROGRID	4	/* Indicates zero grid line      */
#define VEM_ARGMOD	5	/* Command modified argument list */
#define VEM_ARGRESP	6	/* Like 5 but redisplay arg list  */
#define VEM_ARGBREAK	7	/* Breaks into a string argument */
#define VEM_NOSELECT	8	/* No selection made             */
#define VEM_SELPLANE	9	/* Selection on its own plane    */
#define VEM_NEWITEM	10	/* New item created              */
#define VEM_TOOSMALL	11	/* Value is too small            */

#define VEM_BADSTATE	-1	/* State of routine is inappropriate */
#define VEM_CORRUPT	-2	/* Data structure corruption         */
#define VEM_NOROOM	-3	/* Allocation of new space failed    */
#define VEM_NOFILE	-4	/* File or directory not found       */
#define VEM_CANTFIND	-5	/* Requested item can't be found     */
#define VEM_RESOURCE	-6	/* Can't allocate a new resource     */
#define VEM_NOIMPL	-7	/* Feature is not implemented        */
#define VEM_NOTSUFF	-8	/* Not enough data provided          */
#define VEM_TYPECONF	-9	/* Type conflict                     */
#define VEM_NOTUNIQ	-10	/* Requested item is not unique      */
#define VEM_ACCESS	-11	/* Access rights are not sufficient  */
#define VEM_NOTINIT	-12	/* No initialization has been done   */
#define VEM_INUSE	-13	/* Item requested is in use          */
#define VEM_BADARG	-14	/* Bad argument to routine           */

#define VEM_ABORT	-99	/* Terminate program                 */


#define MSG_NOLOG	0x01	/* Don't log message to file */
#define MSG_DISP	0x02	/* Display message to user   */
#define MSG_MOREMD	0x04	/* Use more mode             */

extern void vemMessage();  	/* Writes out a message      */


extern vemStatus bufLock();

/*
 * technology functions
 */
extern void vemFlushTechInfo();
extern void vemSetDisplayType();
extern void vemGetDisplayType();

/*
 * Argument types:
 */

#define	VEM_POINT_ARG	0x0	/* One or more points   */
#define VEM_BOX_ARG	0x1	/* One or more boxes    */
#define VEM_LINE_ARG	0x2	/* One or more lines    */
#define VEM_TEXT_ARG	0x4	/* A single string      */
#define VEM_OBJ_ARG	0x8	/* A bag of OCT objects */


typedef struct selset_dummy_defn {
    int dummy;
} selset_dummy;

typedef selset_dummy *vemSelSet;

extern vemSelSet vemNewSelSet();
   /* Creates a new selected set with specified attributes     */
extern int vemFreeSelSet();
   /* Frees the selected set AND any predefined attributes     */

extern int vemClearSelSet();
   /* Deselects all items in a specified selected set           */
extern int vemAddSelSet();
   /* Adds a specified OCT object to a selected set             */
extern int vemDelSelSet();
   /* Removes a specified OCT object from a selected set        */
extern int vemZoomSelSet();
   /* Zooms a specified window to contain only selected items    */

#include "list.h"
#include "region.h"

extern vemStatus vuFindLayer();
   /* Returns the name of a layer given a spot */
extern vemStatus vuFindSpot();
   /* Return exactly one object at a particular point */


#ifndef DM_H

typedef struct dm_which_item {
    STR itemName;		/* Displayed name               */
    Pointer userData;		/* Associated user data         */
    int flag;			/* Non-zero if user selected it */
} dmWhichItem;


typedef struct dm_which_text {
    STR itemPrompt;		/* Type-in prompt                */
    int rows, cols;		/* Size of type-in               */
    STR value;		/* Optional Default value        */
    Pointer userData;		/* Optional Associated user data */
} dmTextItem;

extern vemStatus dmMultiWhich();
   /* Select zero or more from list of items */

extern vemStatus dmWhichOne();
   /* Select exactly one from a list of items */

extern vemStatus dmMultiText();
   /* Multiple text type-in fields */

extern vemStatus dmConfirm();
   /* Present a confirmation dialog */

extern void dmMessage();

#endif

/* RPC Stuff */

/* for user convenience */
#define VEMMSG(str) vemMessage(str, MSG_DISP|MSG_NOLOG)
  
/*
 * status returned by external RPC routines
 */
typedef int rpcStatus;

/* RPC Error Codes, RPC_ERROR must be less than zero */
#define RPC_OK		 1
#define RPC_ERROR	-1


/*
 * application function registering
 */

struct RPCFunction {
    int (*function)();			/* user function to call      */
    char *paneString;			/* name of the menu pane      */
    char *menuString;			/* name of the menu item      */
    char *keyBinding;			/* keybinding for the command */
    long userOptionWord;		/* user option word	      */
};

typedef struct RPCFunction RPCFunction;


/*
 * arguments for remote user functions
 */

struct RPCPointArg {
    long theWin;
    octId facet;
    struct octPoint thePoint;
};


struct RPCObjArg {
    octId facet;                    /* oct id of the facet the contains the object */
    octId theBag;                   /* oct id of the container                     */
    struct octTransform tran;       /* transform for the objects                   */
};


struct RPCArg {
    long argType;			/* type of the argument */
    long length;			/* length of the argument */
    union {
	struct RPCPointArg *points;     /* array of points (point, box, line) */
	char *string;			/* text string */
	struct RPCObjArg objArg;        /* bag arg */
    } argData;
};


typedef struct RPCPointArg RPCPointArg;
typedef struct RPCArg RPCArg;
typedef struct RPCObjArg RPCObjArg;


struct RPCSpot {
    Window theWin;              /* window where the command was invoked   */
    octId facet;		/* facet where command was invoked        */
    struct octPoint thePoint;	/* oct location where command was invoked */
};


typedef struct RPCSpot RPCSpot;

/* Hack for pigilib/PVem.cc */
#ifdef __cplusplus
extern vemStatus vemCommand(char *vemCmd, RPCSpot *spot,
			     lsList cmdList, long userOptionWord);
#else /* __cplusplus */
extern vemStatus vemCommand();	/* perform a vem command     */

#endif /* __cplusplus */

/* RPC Functions */

EXTERN void RPCExit();
EXTERN void RPCUserIO();
EXTERN char *RPCXGetDefault();
EXTERN void RPCExitVem();

/* Hack for pigilib/PVem.cc */
#ifdef __cplusplus
extern Window vemOpenWindow(octObject *facet, char *geo);
#else /* __cplusplus */
EXTERN Window vemOpenWindow();
#endif /* __cplusplus*/

EXTERN Window vemOpenRPCWindow();
EXTERN void vemPrompt();
EXTERN char *RPCVersionString();
EXTERN void RPCRegisterDemon
  ARGS(( octObject* changeList, void (*func)(octObject* changeList) ) );

EXTERN FILE *RPCfopen
  ARGS(( char* name, char* mode ));

/*
 * CRITICAL macro for SIGIO stuff
 */
#define rpcsigmask(m)      (1 << ((m) - 1))

#define CRITICAL(body) \
    { \
	int rpc_old_signal_mask = sigblock(rpcsigmask(SIGIO));\
	body; \
	sigsetmask(rpc_old_signal_mask); \
    }


/*
 * window options
 */

EXTERN vemStatus vemWnGetOptions();
EXTERN vemStatus vemWnSetOptions();
EXTERN vemStatus vemWnTurnOffLayer();
EXTERN vemStatus vemWnTurnOnLayer();


typedef struct win_options {
    int disp_options;		/* Display option bits (window.h)    */
    int lambda;			/* Oct units per lambda              */
    int snap;			/* Input coordinates snapped to this */
    /* Grid related options */
    int grid_base;		/* Logarithmic base of grid lines    */
    int grid_minbase;		/* Minimum base of logarithmic grid  */
    int grid_majorunits;	/* Major grid line frequency         */
    int grid_minorunits;	/* Minor grid line frequency         */
    int grid_diff;		/* Difference before grid not displayed */
    /* Display related options */
    double bb_thres;		/* Bounding box name threshold       */
    int bb_min;			/* Abstraction level of bounding boxes */
    int con_min;		/* Abstraction level of contacts     */
    int solid_thres;		/* Objects drawn solid at this level */
    char *interface;		/* Interface facet to display        */
    int inst_prio;		/* Instance priority level           */
} wnOpts;

/* field names for 'disp_options' */

#define VEM_GRIDLINES	0x001	/* Grid lines drawn (if visible)          */
#define VEM_TITLEDISP	0x002	/* Display window title                   */
#define VEM_EXPAND	0x004	/* Expand instances in this window        */
/* read-only field  */
#define VEM_REMOTERUN	0x008	/* Remote application running in this window*/
#define VEM_SHOWDIFF	0x010	/* Show difference coordinates            */
#define VEM_SHOWABS	0x020	/* Show absolute coordinates              */
#define VEM_SHOWBITS	0x040	/* Show status bits                       */
#define VEM_SHOWEDIT	0x080	/* Show edit cell name                    */
#define VEM_SHOWCON	0x100	/* Show context cell name                 */
#define VEM_SHOWBB	0x200	/* Show instance bounding boxes           */
#define VEM_SHOWRPC     0x400   /* Show RPC application name              */
/* read-only field */
#define VEM_NODRAW	0x800	/* Make window inactive                   */
#define VEM_MANLINE     0x1000  /* Manhattan line arguments               */
#define VEM_GRAVITY     0x2000  /* Argument gravity for symbolic/schematic */
#define VEM_SHOWAT	0x4000	/* Show actual terminals		  */
#define VEM_DOTGRID	0x8000	/* Display dotted grid			  */

#endif /* RPC_H */


