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
 * Symbolic Policy Library
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * Below are definitions for using the exported functions of
 * the symbolic policy library.
 */

#ifndef _SYM_H_
#define _SYM_H_

#include "ansi.h"
#include "oct.h"
#include "tap.h"

/*
 * Connector support
 */

EXTERN int symContactP
  ARGS((octObject *obj));
  /* Returns a non-zero status if `obj' is a connector or connector terminal */

#define SYM_MAX_MSG	2048

typedef int (*SCF)
	ARGS((int nLayers, tapLayerListElement *layerList,
		    octObject *instance, char why[SYM_MAX_MSG],
		    char *data));

typedef struct sym_con_func_defn {
    SCF func; 			/* Function to call            */
    char *data;			/* User data passed to function */
} symConFunc;

extern symConFunc *symDefConFunc;
   /* Exported default connector function */

EXTERN void symConnector
  ARGS((octObject *parent, octCoord x, octCoord y,
	int ne, tapLayerListElement elems[], symConFunc *confun,
	octObject *connector));
  /* Makes a new connector */

EXTERN int symUpgrade
  ARGS((octObject *term, tapLayerListElement *elem,
	symConFunc *confun, octObject *new_conn));
  /* Examines connector terminal for possible upgrade */

/*
 * Copy a set of objects
 */

typedef void (*SCIF)
	ARGS((octObject *oldObj, octObject *newObj, char *data));

typedef struct sym_copy_item_defn {
    SCIF func;			/* Function to call */
    char *data;			/* User supplied data */
} symCopyItemFunc;

EXTERN void symCopyObjects
  ARGS((octObject *facet, octObject *set, struct octTransform *transform,
	octObject *result, symConFunc *confun, symCopyItemFunc *ci));

/*
 * Delete a set of objects
 */

typedef void (*SDIF)
      ARGS((octObject *item, char *data));

typedef struct sym_del_item_func_defn {
    SDIF func;			/* function to call */
    char *data;			/* User data        */
} symDelItemFunc;

EXTERN void symDeleteObjects
  ARGS((octObject *delSetBag, symDelItemFunc *del_func));


/*
 * Formal terminals
 */

typedef void (*STF)
      ARGS((octObject *fterm, octObject *obj, int detach, char *data));

typedef struct sym_term_func_defn {
    STF func;			/* Function to call */
    char *data;			/* User data        */
} symTermFunc;

EXTERN void symNewFormal
  ARGS((octObject *facet, octObject *fterm, octObject *implBag,
	char *termtype, char *termdir, symTermFunc *term_func));
  /* Creates a new symbolic formal terminal */

#define SYM_TERM_TYPE	"TERMTYPE"
#define SYM_TERM_DIR	"DIRECTION"

EXTERN int symGetTermProp
  ARGS((octObject *term, octObject *prop));
  /* Finds property annotations on formal terminals */

/*
 * Interface generation
 */

typedef void (*SGF)
      ARGS((octObject *inFacet, octObject *outFacet, char *data));

typedef struct sym_gen_func_defn {
    SGF gen_func;		/* Generation function */
    char *data;			/* User data           */
} symGenFunc;

EXTERN void symSetGenFunc
  ARGS((symGenFunc *newObj, symGenFunc *old));
  /* Sets the interface generation function */

EXTERN void symInterface
  ARGS((octObject *inst, octObject *interface));
  /* Opens or makes an interface */

/*
 * Symbolic instances
 */

typedef void (*SLAF)
      ARGS((octObject *t1, octObject *t2, int num_alt, octObject lyrs[], char *data));

typedef struct sym_lyr_alt_defn {
    SLAF lyr_alt_func;		/* Alternate implementation function */
    char *data;			/* User data */
} symLyrAltFunc;

EXTERN void symSetLyrAltFunc
  ARGS((symLyrAltFunc *newObj, symLyrAltFunc *old));

EXTERN int symInstanceP
  ARGS((octObject *obj));
  /* Returns a non-zero status if object is real instance or instance term */

EXTERN void symInstance
  ARGS((octObject *fct, octObject *inst));
  /* Creates a new symbolic instance or connector - no terminal maintenence */

EXTERN void symReconnect
  ARGS((octObject *inst, symConFunc *confun, symDelItemFunc *del_func));
  /* Reconnects and existing instance by examining abutting elements */

/*
 * Jump terminal options
 */

typedef void (*SJNF)
      ARGS((octObject *term, char jump_name[SYM_MAX_MSG], char *data));

typedef struct sym_jumpname_defn {
    SJNF func;			/* Function to call */
    char *data;			/* User data passed to function */
} symJumpNameFunc;

EXTERN void symSetJumpNameFunc
  ARGS((symJumpNameFunc *newObj, symJumpNameFunc *old));
  /* Sets the global/local net naming function */

/*
 * Moving sets of objects
 */

typedef void (*SMIF)
      ARGS((octObject *item, int after_p, char *data));

typedef struct sym_move_item_func_defn {
    SMIF func;			/* Function to call */
    char *data;			/* Extra data       */
} symMoveItemFunc;

EXTERN void symMoveObjects
  ARGS((octObject *set, struct octTransform *transform,
	symMoveItemFunc *move_func));
  /* Moves a set of items to a new location - doesn't change connectivity */

/*
 * Nets
 */

typedef int (*SMF)
      ARGS((octObject *net1, octObject *net2, char *data));

typedef struct sym_merge_func_defn {
    SMF func;			/* Function to call */
    char *data;			/* User data        */
} symMergeFunc;

EXTERN void symSetMergeFunc
  ARGS((symMergeFunc *newObj, symMergeFunc *old));
  /* Set the merging function */

EXTERN int symLocNet
  ARGS((octObject *obj, octObject *net));
  /* Finds net associated with object */

EXTERN int symHumanNamedNetP
  ARGS((octObject *net));
  /* Returns non-zero if net is human named */

/*
 * Replacing instances
 */

EXTERN void symReplaceInst
  ARGS((octObject *fct, octObject *old, octObject *newObj));

/*
 * Adding and modifying segments
 */

EXTERN void symSegments
  ARGS((octObject *facet, octObject *first, octObject *last,
	int np, struct octPoint *points,
	int nl, octObject layers[],
	int nw, octCoord widths[],
	symConFunc *confun));
  /* Creates a series of new symbolic segments */

EXTERN void symSegChange
  ARGS((octObject *fct, octObject *seg, octObject *new_lyr,
	octCoord new_width, symConFunc *confun));
  /* Changes the layer or width of an existing segment */

EXTERN void symSegDesc
  ARGS((octObject *seg, struct octPoint *ref, tapLayerListElement *elem));
  /* Returns description for segment */

EXTERN int symSegConnP
  ARGS((octObject *term, tapLayerListElement *elem));
  /* Returns non-zero if a segment described by `elem' can connect to `term' */

/*
 * Terminal information
 */

typedef struct term_dummy_defn {
    int dummy;
} term_dummy;

typedef term_dummy *symTermGen;

EXTERN void symInitTerm
  ARGS((octObject *a_term, symTermGen *t_gen));
  /* Initializes generation of terminal implementations */

EXTERN int symNextTerm
  ARGS((symTermGen gen, octObject *lyr, octObject *geo));
  /* Returns next implementation geometry in sequence */

EXTERN void symEndTerm
  ARGS((symTermGen gen));
  /* Ends sequence of terminal generation */

/*
 * Area searching
 */

typedef struct area_dummy_defn {
    int dummy;
} *symAreaGen;

EXTERN void symAreaInit
  ARGS((octObject *fct, octObjectMask mask, struct octBox *area,
	symAreaGen *gen));

EXTERN int symAreaNext
  ARGS((symAreaGen gen, octObject *obj));

EXTERN void symAreaEnd
  ARGS((symAreaGen gen));


/*
 * Error and message handling
 */

extern char *sym_pkg_name;

typedef enum sym_severity_defn { SYM_INFO, SYM_WARN, SYM_ERR } symSeverity;

typedef struct sym_warn_func_defn {
    void (*func)		/* Function to call */
      ARGS((symSeverity severity, char *message, char *data));
    char *data;			/* User data        */
} symWarnFunc;

EXTERN void symSetWarnFunc
  ARGS((symWarnFunc *newObj, symWarnFunc *old));
  /* Sets the warning/message function for symbolic */

/* Possible fatal faults */
#define SYM_OCTFAIL	0	/* Oct call failed              */
#define SYM_NONET	1	/* Object has no associated net */
#define SYM_MAPFAIL	2	/* Group map table failure      */
#define SYM_BADOBJ	3	/* Bad object type              */
#define SYM_BADTERM	4	/* Formal/actual mismatch       */
#define SYM_NOINTR	5	/* No interface facet           */
#define SYM_NOMASTER	6	/* No master for instance       */
#define SYM_PERM	7	/* Permission failure           */
#define SYM_BADPOL	8	/* Cell has bad symbolic policy */
#define SYM_MULNET	9	/* Too many nets                */
#define SYM_BADBAG	10	/* Bad policy bag maintenence   */
#define SYM_BADSEG	11	/* Bad symbolic segment         */
#define SYM_BADFUNC	12	/* Bad user function            */
#define SYM_BADIMPL     13      /* Bad terminal implementation  */
#endif /* _SYM_H_ */
