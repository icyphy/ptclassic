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

#ifndef RPCINTERNAL_H
#define RPCINTERNAL_H

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
 *   Electronics Research Laboratory
 *   Cory Hall
 *   University of California
 *   Berkeley, CA   94720
 *
 *   rpc@eros.Berkeley.EDU  (ARPANET)
 *   ..!ucbvax!eros!rpc        (UUCP)
 *
 */

/*
 * internal header file for RPC (shared by application library and server)
 *
 * not to be used by mere mortals (i.e. users)
 *
 * Rick L Spickelmier, 11/12/86, last modified 3/2/87
 *
 */
#include "ansi.h"
#include "region.h"
/*
 * important top level defines
 *
 * VAXLISP - for VAXLISP support
 * NOWAIT - run RPC in NOWAIT mode - do not wait for a return code
 *
 */
#define NOWAIT

/* can not use fread in NOWAIT mode */
#ifdef SERVER  
#ifdef NOWAIT
int RPCfread
	ARGS((register char *ptr, int size, int count, FILE *stream));

#define fread	RPCfread
#endif /* NOWAIT */
#endif /* SERVER */

#define RPCALLOC(type)	(type *) malloc(sizeof(type))
#define RPCARRAYALLOC(type, count) \
	(type *) malloc((unsigned)(sizeof(type) * (count)))
#define RPCNIL(type)  (type *) 0
#define RPCFREE(ptr)  (void) free((char *) ptr)

/* RPC Version number - if the server and lib are not the same, complain */
#define RPC_VERSION     (long) 7

/* is the application byte swapped */
extern int RPCByteSwapped;

/* floating point number format */
extern int RPCFloatingPointSame;
#define RPCDOUBLE 1.2345678901234567e-7

/*
 * buffer used for building error and debugging messages to be logged.
 *
 * external definition and size
 */
extern char RPCErrorBuffer[];
#define MESSAGE_SIZE	1024

/* state of remote application */
#define RPC_BUSY 1
#define RPC_IDLE 0

/*
 * FDMASK sets a mask that can be used for adding or removing a file
 * descriptor to/from a selection mask
 */
#define FDMASK(fd)	(1 << (fd))

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

typedef FILE *STREAM;

/*
 * internal status 
 *
 * 1 (TRUE) or 0 (FALSE)
 *
 * used by low level routines
 *
 */
/* typedef int rpcInternalStatus; */
/* define rather than typedef to please lint -h */
#define rpcInternalStatus int

#define TRUE	1
#define FALSE	0

#ifndef RPCLOCATION
#define RPCLOCATION "unknown"
#endif /* RPCLOCATION */

#define RPCASSERT(function_and_args, returnCode) \
    if (!function_and_args) { \
	char buffer[1024]; \
        (void) sprintf(buffer, "Vem RPC Error: %s: function_and_args", RPCLOCATION); \
	perror(buffer); \
	return returnCode; \
    }

#define RPCFLUSH(stream, returnCode) \
    if (fflush(stream) == EOF) { \
	char buffer[1024]; \
        (void) sprintf(buffer, "Vem RPC Error: %s: fflush", RPCLOCATION); \
	perror(buffer); \
	return returnCode; \
    }

/*
 * external status, RPC_ERROR must be less than zero
 *
 */

/* RPC Exit Codes */
#define RPC_BAD_EXIT	-1
#define RPC_GOOD_EXIT	 0

/* remote kill signal */
#define RPC_KILL_SIGNAL 9


#ifndef RPC_H

typedef int rpcStatus;

#define RPC_OK		 1
#define RPC_ERROR	-1

/*
 * argument list for remote user functions
 *   much simplified version of the tightly bound argList.
 */

struct RPCPointArg {
    Window theWin;
    octId facet;
    struct octPoint thePoint;
};


struct RPCObjArg {
    octId facet;                /* oct id of the facet the contains the object */
    octId theBag;               /* oct id of the container                     */
    struct octTransform tran;   /* transform for the objects                   */
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

/*
 * spot
 *   an argument to every remote function
 *   much simplified version of the tightly bound spot argument, this
 *   one only contains that oct id of the facet that is displayed
 *   in the window that the command was invoked in and the oct location
 *   where the command was invoked.
 *
 */
struct RPCSpot {
    Window theWin;				/* window of invocation	*/
    octId facet;				/* facet in the window	*/
    struct octPoint thePoint;			/* oct point in the window */
};

typedef struct RPCSpot RPCSpot;

#endif /* RPC_H */


/* for brain damaged octGetPoints */
#define RPC_SEND_POINTS		1
#define RPC_DO_NOT_SEND_POINTS	0

/* for octError */
#define RPC_OCT_ERROR_END	(long) -10000

/*
 * VEM RPC function codes
 */
#define VEM_INITIALIZE_APPLICATION_FUNCTION	(long) 1
#define VEM_SEND_MENU_FUNCTION			(long) 2

#define VEM_COMPLETE_FUNCTION			(long) 3

#define VEM_MESSAGE_FUNCTION			(long) 10
#define VEM_PROMPT_FUNCTION			(long) 11
#define VEM_COMMAND_FUNCTION			(long) 12
#define VEM_OPEN_WINDOW_FUNCTION		(long) 13

#define VEM_FLUSH_TECH_INFO_FUNCTION		(long) 14
#define VEM_SET_DISPLAY_TYPE_FUNCTION		(long) 15
#define VEM_GET_DISPLAY_TYPE_FUNCTION		(long) 16
#define VEM_OPEN_RPC_WINDOW_FUNCTION            (long) 17

/*
 * window operations
 */
#define WN_FLUSH_FUNCTION			(long) 20
#define WN_QREGION_FUNCTION			(long) 21
#define WN_GET_OPTIONS_FUNCTION			(long) 22
#define WN_SET_OPTIONS_FUNCTION			(long) 23
#define WN_TURN_OFF_LAYER_FUNCTION		(long) 24
#define WN_TURN_ON_LAYER_FUNCTION		(long) 25

/*
 * select set stuff
 */
#define VEM_NEW_SEL_SET_FUNCTION		(long) 30
#define VEM_FREE_SEL_SET_FUNCTION		(long) 31
#define VEM_CLEAR_SEL_SET_FUNCTION		(long) 32
#define VEM_ADD_SEL_SET_FUNCTION		(long) 33
#define VEM_DEL_SEL_SET_FUNCTION		(long) 34
#define VEM_ZOOM_SEL_SET_FUNCTION		(long) 35

#define VEM_BUF_LOCK_FUNCTION			(long) 38

/*
 * region package
 */
#define REG_INIT_FUNCTION			(long) 40
#define REG_NEXT_FUNCTION			(long) 41
#define REG_END_FUNCTION			(long) 42
#define REG_OBJ_START_FUNCTION			(long) 43
#define REG_OBJ_NEXT_FUNCTION			(long) 44
#define REG_OBJ_FINISH_FUNCTION			(long) 45
#define REG_FIND_ACTUAL_FUNCTION		(long) 46
#define REG_FIND_IMPL_FUNCTION			(long) 47
#define REG_FIND_LYR_IMPL_FUNCTION		(long) 48
#define REG_FIND_NET_FUNCTION			(long) 49
#define REG_ERROR_STRING_FUNCTION		(long) 50

/*
 * dialog manager package
 */
#define DM_MULTI_WHICH_FUNCTION			(long) 60
#define DM_WHICH_ONE_FUNCTION			(long) 61
#define DM_MULTI_TEXT_FUNCTION			(long) 62
#define DM_CONFIRM_FUNCTION			(long) 63
#define DM_MESSAGE_FUNCTION			(long) 64

/*
 * vem utility package
 */
#define VU_FIND_SPOT_FUNCTION			(long) 70
#define VU_FIND_LAYER_FUNCTION			(long) 71

/*
 * rpc package
 */
#define RPC_CLEAN_EXIT_FUNCTION			(long) 80
#define RPC_FILE_OPEN_FUNCTION			(long) 81
#define RPC_X_GET_DEFAULT_FUNCTION		(long) 82
#define RPC_EXIT_VEM_FUNCTION			(long) 83
#define RPC_REGISTER_DEMON_FUNCTION		(long) 84

#define RPC_DEMON_FUNCTION			(long) -1
  
#define VEM_MAX_FUNCTION			(long) 200

/*
 * OCT RPC function codes
 */
#define OCT_ATTACH_FUNCTION			(long) 1 + VEM_MAX_FUNCTION
#define OCT_DETACH_FUNCTION			(long) 2 + VEM_MAX_FUNCTION
#define OCT_COPY_FACET_FUNCTION			(long) 3 + VEM_MAX_FUNCTION
#define OCT_CREATE_FUNCTION			(long) 5 + VEM_MAX_FUNCTION
#define OCT_DELETE_FUNCTION			(long) 6 + VEM_MAX_FUNCTION
#define OCT_ERROR_FN_FUNCTION			(long) 7 + VEM_MAX_FUNCTION
#define OCT_INIT_GEN_CONTENTS_FUNCTION		(long) 8 + VEM_MAX_FUNCTION
#define OCT_INIT_GEN_CONTAINERS_FUNCTION	(long) 9 + VEM_MAX_FUNCTION
#define OCT_GENERATE_FUNCTION			(long) 10 + VEM_MAX_FUNCTION
#define OCT_GET_BY_NAME_FUNCTION		(long) 11 + VEM_MAX_FUNCTION
#define OCT_GET_BY_ID_FUNCTION			(long) 12 + VEM_MAX_FUNCTION
#define OCT_GET_OR_CREATE_FUNCTION		(long) 13 + VEM_MAX_FUNCTION
#define OCT_CREATE_OR_MODIFY_FUNCTION		(long) 14 + VEM_MAX_FUNCTION
#define OCT_MODIFY_FUNCTION			(long) 15 + VEM_MAX_FUNCTION
#define OCT_OPEN_FACET_FUNCTION			(long) 16 + VEM_MAX_FUNCTION
#define OCT_FLUSH_FACET_FUNCTION		(long) 17 + VEM_MAX_FUNCTION
#define OCT_CLOSE_FACET_FUNCTION		(long) 18 + VEM_MAX_FUNCTION
#define OCT_FREE_FACET_FUNCTION			(long) 19 + VEM_MAX_FUNCTION
#define OCT_GET_POINTS_FUNCTION			(long) 20 + VEM_MAX_FUNCTION
#define OCT_PUT_POINTS_FUNCTION			(long) 21 + VEM_MAX_FUNCTION
#define OCT_BB_FUNCTION				(long) 22 + VEM_MAX_FUNCTION

#define OCT_GENERATE_SPECIAL_FUNCTION		(long) 25 + VEM_MAX_FUNCTION
  
#define OCT_MODIFY_SPECIAL_FUNCTION		(long) 29 + VEM_MAX_FUNCTION

#define OCT_GET_FACET_FUNCTION			(long) 30 + VEM_MAX_FUNCTION
#define OCT_ERROR_STRING_FUNCTION		(long) 31 + VEM_MAX_FUNCTION
  
#define OCT_ATTACH_ONCE_FUNCTION		(long) 32 + VEM_MAX_FUNCTION
#define OCT_IS_ATTACHED_FUNCTION		(long) 33 + VEM_MAX_FUNCTION
#define OCT_OPEN_MASTER_FUNCTION		(long) 34 + VEM_MAX_FUNCTION

#define OCT_GET_CONTAINER_BY_NAME_FUNCTION	(long) 35 + VEM_MAX_FUNCTION
#define OCT_GEN_FIRST_CONTENT_FUNCTION		(long) 36 + VEM_MAX_FUNCTION
#define OCT_GEN_FIRST_CONTAINER_FUNCTION	(long) 37 + VEM_MAX_FUNCTION
#define OCT_FREE_GENERATOR_FUNCTION		(long) 38 + VEM_MAX_FUNCTION

/* transform and scale */
#define OCT_TRANSFORM_GEO_FUNCTION		(long) 39 + VEM_MAX_FUNCTION
#define OCT_TRANSFORM_MODIFY_GEO_FUNCTION	(long) 40 + VEM_MAX_FUNCTION
#define OCT_TRANSFORM_POINTS_FUNCTION		(long) 41 + VEM_MAX_FUNCTION	
#define OCT_SCALE_GEO_FUNCTION			(long) 42 + VEM_MAX_FUNCTION
#define OCT_SCALE_MODIFY_GEO_FUNCTION		(long) 43 + VEM_MAX_FUNCTION

#define OCT_MARK_TEMPORARY_FUNCTION		(long) 44 + VEM_MAX_FUNCTION
#define OCT_UNMARK_TEMPORARY_FUNCTION		(long) 45 + VEM_MAX_FUNCTION
#define OCT_IS_TEMPORARY_FUNCTION		(long) 46 + VEM_MAX_FUNCTION

#define OCT_XID_FUNCTION			(long) 47 + VEM_MAX_FUNCTION
#define OCT_GET_BY_XID_FUNCTION			(long) 48 + VEM_MAX_FUNCTION

#define OCT_FULL_NAME_FUNCTION			(long) 49 + VEM_MAX_FUNCTION
#define OCT_GET_FACET_INFO_FUNCTION		(long) 50 + VEM_MAX_FUNCTION

#define OCT_OPEN_RELATIVE_FUNCTION              (long) 51 + VEM_MAX_FUNCTION
#define OCT_WRITE_FACET_FUNCTION		(long) 52 + VEM_MAX_FUNCTION

/*
 * integer and real oct arrays
 */

struct octIntegerArray {
    int length;
    int *array;
};

struct octRealArray {
    int length;
    double *array;
};


/*
 * function declarations
 */
extern char *RPCstrsave();

extern void RPCByteSwappedApplication();
extern void RPCProcessFileWriteRequest();

/* receive data across a socket */
extern rpcInternalStatus	RPCReceiveLong();
extern rpcInternalStatus	RPCReceiveByte();
extern rpcInternalStatus	RPCReceiveFloat();
extern rpcInternalStatus	RPCReceiveOctGenerator();
extern rpcInternalStatus	RPCReceiveOctObject();
extern rpcInternalStatus	RPCReceiveOctBox();
extern rpcInternalStatus	RPCReceiveOctPoint();
extern rpcInternalStatus	RPCReceiveOctProp();
extern rpcInternalStatus	RPCReceiveOctTransform();
extern rpcInternalStatus	RPCReceiveString();
extern rpcInternalStatus        RPCReceiveIntegerArray();
extern rpcInternalStatus        RPCReceiveRealArray();

extern rpcStatus                RPCReceiveVemArgs();


/* send data across a socket */
extern rpcInternalStatus	RPCSendLong();
extern rpcInternalStatus	RPCSendByte();
extern rpcInternalStatus	RPCSendFloat();
extern rpcInternalStatus	RPCSendOctGenerator();
extern rpcInternalStatus	RPCSendOctObject();
extern rpcInternalStatus	RPCSendOctBox();
extern rpcInternalStatus	RPCSendOctPoint();
extern rpcInternalStatus	RPCSendOctProp();
extern rpcInternalStatus	RPCSendOctTransform();
extern rpcInternalStatus	RPCSendString();
extern rpcInternalStatus        RPCSendIntegerArray();
extern rpcInternalStatus        RPCSendRealArray();

extern rpcStatus                RPCSendVemArgs();

extern rpcInternalStatus	RPCSendFacetInfo
	ARGS((struct octFacetInfo *info, STREAM stream));
extern rpcInternalStatus	RPCSendRegionGenerator
	ARGS((regGenerator *generator, STREAM stream));
extern rpcInternalStatus        RPCReceiveRegionGenerator
	ARGS((regGenerator *generator, STREAM stream));
extern rpcInternalStatus	RPCReceiveSpot
	ARGS((RPCSpot *spot, STREAM stream));

#endif /* RPCINTERNAL_H */
