/* 
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
                                                        COPYRIGHTENDKEY
*/

/*
    ptkNet.c  aok
    Version: $Id$
*/


/*
   The following code is take from appNet.c in the octtools set.
   The version of appNet.c was dated Feb 1, 1990.  If appNet.c
   is updated, this program must be changed as well.  To aid in this,
   unneed code from appNet.c has been left here, but is enclosed in
   ifndef PTKCODE comments. 
   note also that the original appNet.c error reporting to stderr
   has also been preserved.  It may be desirable to change this in the
   future.
*/
#define PTKCODE


#include "copyright.h"
#include "port.h"
#include "utility.h"

/*
 * network routines for the application
 *
 * Rick L Spickelmier, 11/11/86, last modified 3/2/87
 */

/* kludge - due to moving target, handled in -I on compile */
/* KERNEL define to stop recursive inclusion of time.h in BSD and SUN */
#if defined(sun) || defined(vax)
#define KERNEL
#include <time.h>
#undef KERNEL
#else
#include <time.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

extern int errno;

/* #include "list.h"    */
/* #include "message.h" */
#include "rpcApp.h"
#include "oh.h"

/* ptk code change
   huge block of functions removed 
*/

/*
 * handle remote events FROM VEM and User Selection
 *
 * Arguments:
 *   funcArray  array of function pointers and menu strings
 *   size       number of function pointer and menu string pairs
 *
 * Returns:
 *   RPC_ERROR on error condition
 *   otherwise, never returns
 */

rpcStatus
#ifndef PTKCODE
RPCApplicationProcessEvents(funcArray, size)
#endif
#ifdef PTKCODE
ptkRPCFileHandler(funcArray, size)
#endif
RPCFunction funcArray[];
long size;                      /* number of items in the array */
{
#ifndef PTKCODE
    int rmask;                  /* read selection mask */
    int wmask;                  /* write selection mask */
    int emask;                  /* exception selection mask */
    int nfound;                 /* number of items selected */
#endif /* PTKCODE */
    long functionNumber;        /* user function number */
    RPCSpot spot;
    lsList argList;             /* user function arguments */
    long userOptionWord;

#ifndef PTKCODE
    /* set up the masks */
    rmask = FDMASK(fileno(RPCReceiveStream)) | RPCUserSelection.ReadMask;
    wmask = RPCUserSelection.WriteMask;
    emask = RPCUserSelection.ExceptionMask;

    /*
     * loop waiting for VEM to request user functions
     */
loop:
    while ((nfound = select(32, &rmask, &wmask, &emask,
                            RPCNIL(struct timeval))) >= 0) {

        /* timeout or error */
        if (nfound <= 0) {
            perror("select fail");
            return RPC_ERROR;
        }

        if (FDMASK(fileno(RPCReceiveStream)) & rmask) {
            nfound--;
#endif /*PTKCODE */

            /* get the function number */
            if (!RPCReceiveFunctionToken(&functionNumber, RPCReceiveStream)) {
                return RPC_ERROR;
            }

            /* special case demon functions */
            if (functionNumber == RPC_DEMON_FUNCTION) {
                octObject changeList;
                long id;
                void (*ptr)();                /* was char *ptr */
                void (*func)();

                /* get the octId of the change list */
                if (RPCReceiveLong(&id, RPCReceiveStream) != RPC_OK) {
                    return RPC_ERROR;
                }
                /* see if a demon has really been registered on the change list */
/* fprintf(stderr, "client here: just received a demon request\n"); */
                if (st_lookup(RPCDemonTable, (char *) id, (char**)&ptr)) {
                    ohGetById(&changeList, (octId) id);
                    func = ptr;
/* fprintf(stderr, "client here: found the corresponding demon\n"); */
                    /* run the function */
                    (*func)(&changeList);
/* fprintf(stderr, "client here: finished the demon\n"); */

                    /* clean up */
                    st_delete(RPCDemonTable, (char **) &id, &ptr);
                } 
/*
                else {
fprintf(stderr, "client here: could not find the demon\n");
                }
*/
                if (RPCApplicationFunctionComplete() != RPC_OK) {
                    return RPC_ERROR;
                }
/* fprintf(stderr, "client here: sent application complete message\n"); */
            } else if (functionNumber <= size) {

                /* user rpc functions */
                if (RPCReceiveVemArgs(&spot, &argList, &userOptionWord,
                                      RPCReceiveStream) != RPC_OK) {
                    (void) fprintf(stderr,
                                   "RPC Error: getting args\n");
                    return RPC_ERROR;
                }

                /* call the user function */
                if ((*funcArray[functionNumber].function)(&spot, argList,
                           userOptionWord) != RPC_OK) {
                    (void) fprintf(stderr, "RPC Error: user function\n");
                    return RPC_ERROR;
                }

                /* clean up */
                if (RPCApplicationFunctionComplete() != RPC_OK) {
                    return RPC_ERROR;
                }
            } else {
                (void) fprintf(stderr, "RPC Error: bad function number\n");
                return RPC_ERROR;
            }
#ifndef PTKCODE
        }

        if ((nfound > 0) &&
            ((RPCUserSelection.ReadMask & rmask)
             || (RPCUserSelection.WriteMask & wmask)
             || (RPCUserSelection.ExceptionMask & emask))) {
            (*RPCUserSelection.Function)(nfound,
                                         (int *) &rmask,
                                         (int *) &wmask,
                                         (int *) &emask);
        }

        /* reset the masks */
        rmask = FDMASK(fileno(RPCReceiveStream)) | RPCUserSelection.ReadMask;
        wmask = RPCUserSelection.WriteMask;
        emask = RPCUserSelection.ExceptionMask;
    }

    if (errno == EINTR) {
        goto loop;
    }
    
    (void) fprintf(stderr, "RPC Error: interrupted select\n");

    return RPC_ERROR;
#endif /* PTKCODE */
    return RPC_OK;
}

/* ptk code change
rest of code in file appNet.c removed
*/
