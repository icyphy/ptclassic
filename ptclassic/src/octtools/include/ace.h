/* Version Identification:
 * @(#)ace.h	1.1	12/15/93
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
 * ace.h
 *
 * User header file for the ACE simulated-annealing-based
 * array folder/compactor package.
 *
 * ACE: an Array Compaction Engine
 */

#ifndef ACE_H /* avoid multiple includes				*/
#define ACE_H

/* needed for definition of ARGS macro					*/
#include "ansi.h"

#define ACE_PKG_NAME "ace"

/*
 * UCF event types
 */
#define ACE_UCFE_NET_MOVE	(1<<0)
#define ACE_UCFE_FLIP_TERM	(1<<1)
#define ACE_UCFE_NET_DLEN	(1<<2)
#define ACE_UCFE_NET_CELL_MOVE	(1<<3)

#define ACE_UCFE_ALL	( ACE_UCFE_NET_MOVE	|			\
			ACE_UCFE_FLIP_TERM	|			\
			ACE_UCFE_NET_DLEN	|			\
			ACE_UCFE_NET_CELL_MOVE )

/* Ids used to identify various elements.				*/
typedef unsigned aceId;

#define ace_null_id ((aceId)0)

/* Objects accessible in user constraint functions			*/
typedef enum aceObjType {
    ACE_OBJ_MATRIX,
    ACE_OBJ_PLANE,
    ACE_OBJ_ROWCOL,
    ACE_OBJ_NET,
    ACE_OBJ_MASTER,
    ACE_OBJ_CELL,
    ACE_OBJ_UNKNOWN
} aceObjType;

typedef struct aceObject{
    struct aceObject *next;			/* link to next object	*/
    aceObjType type;				/* type of this object	*/
    aceId id;					/* id of this object	*/
} aceObject;

typedef struct aceCoord {
    int row, col;                               /* loc in matrix        */
} aceCoord;

typedef struct aceApNet {
    struct aceObject *next;			/* link to next object	*/
    aceObjType type;				/* type of this object	*/
    aceId id;					/* net's id		*/
    unsigned termMask;				/* ACE_TERM bits	*/
    short isRow;				/* bool, set if row net	*/
    int rowcol;					/* row or col of full matrix */
    int lower, upper;				/* range of net		*/
} aceApNet;

typedef struct aceApRowCol {
    struct aceObject *next;			/* link to next object	*/
    aceObjType type;				/* type of this object	*/
    aceId id;					/* rowcol's id		*/
    int rowcol;					/* rowcol index		*/
    short isRow;				/* bool, orientation	*/
    int netcount;				/* count of nets in rowcol */
} aceApRowCol;

typedef struct aceApPlane {
    struct aceObject *next;			/* link to next object	*/
    aceObjType type;				/* type of this object	*/
    aceId id;					/* plane's id		*/
    int lower, upper;				/* range of plane	*/
    short isRow;				/* bool, orientation	*/
} aceApPlane;

typedef struct aceApCell {
    struct aceObject *next;			/* link to next object	*/
    aceObjType type;				/* type of this object	*/
    aceId id;					/* cell's id		*/
    aceCoord loc;				/* location of cell	*/
    aceId masterId;				/* id of cell's master	*/
} aceApCell;

typedef struct aceApMaster {
    struct aceObject *next;			/* link to next object	*/
    aceObjType type;				/* type of this object	*/
    aceId id;					/* cell's id		*/
    char *name;					/* single char		*/
    char *path;					/* full path (if appl.)	*/
    aceCoord cellCap;				/* capacity used by inst */
} aceApMaster;

/* generator structure							*/
typedef struct aceGenerator {
    int ( *gen_fn) ARGS((struct aceGenerator *gen, struct aceObject **obj));
    char *state;
    void ( *free_fn) ARGS((struct aceGenerator *gen));
} aceGenerator;

#define aceGenerate( gen, obj)						\
	( *(gen)->gen_fn)((gen), (obj))

/* ace move set								*/
#define ACE_MOVE_SWAP		0		/* swap rowcols of nets	*/
#define ACE_MOVE_DISPLACE	1		/* move one net		*/
#define ACE_MOVE_FLIP_TERM	2		/* flip term to other side */
#define ACE_MOVE_RM_OVERLAP	3		/* get rid of all overlap */

#define ACE_MOVE_COUNT		4		/* number of move types	*/

/* defaults structure							*/
typedef struct aceCostPair {
    double row, col;				/* value of various 	*/
} aceCostPair;					/* component costs	*/

typedef struct aceAnnealParams {
    aceCostPair netLength;
    aceCostPair usedRC;
    aceCostPair usedBias;
    aceCostPair netCapacity;
    aceCostPair cellCapacity;
} aceAnnealParams;

typedef struct aceDefaults {
    aceAnnealParams cost;			/* costFn parameters	*/
    double (* probFnVec[ ACE_MOVE_COUNT ])	/* move gen probFns	*/
	    ARGS((double temp));
    double (* initTemp)				/* initialize temperature */
	    ARGS((double cost, int fastFlag));
    double (* updateTemp)			/* gen next temperature	*/
	    ARGS((double temp, double cost, int fastFlag));
    void (* initStopInner)			/* init inner loop stop params*/
	    ARGS((int rows, int cols, int planes, int nets, int cells,
			double temp, double cost, int fastFlag));
    int (* stopInner)				/* stop inner loop	*/
	    ARGS((int gen, int accept, int fastFlag));
    int (* stopOuter)				/* stop outer loop	*/
	    ARGS((double temp, double cost, int fastFlag));
} aceDefaults;

/*
 * terminal net bits (for virtual range terminal nets: net.termMask).
 */
#define ACE_TERM_IS_TERM	(1<<0) 		/* this net is a term	*/
#define ACE_TERM_TWO_STATE	(1<<1) 		/* term can be on either edge */
#define ACE_TERM_LOWER_VE	(1<<2)		/* term is on lower edge */

/*
 * trace mode bits ( bits <0:15> ).  These specify what information
 * and matrix characteristics will be traced.
 */
#define ACE_TRACE_TEMP		(1<<0)
#define ACE_TRACE_COST		(1<<1)
#define ACE_TRACE_ACC_MOVE 	(1<<2)
#define ACE_TRACE_REJ_MOVE 	(1<<3)
#define ACE_TRACE_GEN_MOVE 	(ACE_TRACE_ACC_MOVE | ACE_TRACE_REJ_MOVE)
#define ACE_TRACE_MOVE_RESULTS	(1<<4)
#define ACE_TRACE_COM_COST	(1<<5)
#define ACE_TRACE_CPU_TIME	(1<<6)
#define ACE_TRACE_ALL_MASK	0x0000ffff

/*
 * trace frequency bits ( bits <16:31> ).  These specify how often
 * the tracing is performed.
 */
#define ACE_TRACEF_INITIAL	(1<<16)
#define ACE_TRACEF_FINAL	(1<<17)
#define ACE_TRACEF_STATE	(1<<18)
#define ACE_TRACEF_ACC_MOVE	(1<<19)
#define ACE_TRACEF_REJ_MOVE	(1<<20)
#define ACE_TRACEF_GEN_MOVE 	(ACE_TRACEF_ACC_MOVE | ACE_TRACEF_REJ_MOVE)
#define ACE_TRACEF_ALL_MASK	0xffff0000

/* external functions							*/
extern aceId		aceAddCell();
extern aceId		aceAddMaster();
extern void		aceAnnealMatrix();
extern aceId		aceCreateMatrix();
extern void		aceFreeGenerator();
extern void		aceFreeMatrix();
extern aceObject	*aceGetById();
extern void		aceGetCellNets();
extern aceDefaults	*aceGetDefaults();
extern void		aceGetNetsAtLoc();
extern void		aceGenFirstContainer();
extern void		aceInitGenContents();
extern void		aceOutputMatrix();
extern void		aceGetPlanesAtLoc();
extern void		aceRandomSeed();
extern void		aceRegisterTermNet();
extern void		aceRegisterUCF();
extern void		aceSetCellCapacity();
extern void		aceSetDefaults();
extern void		aceSetNetCapacity();
extern void		aceSetTrace();
extern void		aceSplitNet();
extern void		aceSplitPlane();
extern aceCoord		*aceTranslate();

/* constraint functions included in ACE					*/
extern double		aceUCFAdjNets();
extern double		aceUCFNetLength();
extern double		aceUCFNetOrder();

#endif /* ACE already included						*/
