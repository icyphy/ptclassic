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
    ptkTemplate.c  aok
    Version: $Id$
*/


/*
   The following code is take from appTemplate.c in the octtools set.
   The version of appTemplate.c was dated Feb 22, 1990.  If appTemplate.c
   is updated, this program must be changed as well.  To aid in this,
   unneed code from appTemplate has been left here, but is enclosed in
   #ifndef PTKCODE ... #endif, and new code is also bracketed by ifdefs
   as in the .h file below.  
   note also that the original appTemplate.c error reporting to stderr
   has also been preserved.  It may be desirable to change this in the
   future.  - Alan Kamas
*/
#define PTKCODE

#ifdef PTKCODE
#include "ptkTkSetup.h"
#endif

#ifndef VAXLISP

#include "copyright.h"
#include "port.h"

/*
 * the VAXLISP version defines it own main
 */

/*
 * application main
 *
 */

#include "rpcApp.h"

extern long UserMain();

#ifndef PTKCODE
RPCMain(argc, argv)
#endif
#ifdef PTKCODE
ptkRPCInit(argc, argv)
#endif
int argc;
char **argv;
{
    RPCFunction *CommandArray;          /* menu/function array */
    RPCSpot spot;
    lsList cmdList;
    long userOptionWord;
    char *display;
    long size;
    static int been_here_before = 0;            /* for 'wbaker' */

#ifndef aiws
    /*
     * ignore stop signals, it's real bad to stop remote applications...
     * so I won't let you (note that AIX does not support these)
     */
    (void) signal(SIGSTOP, SIG_IGN);
    (void) signal(SIGTSTP, SIG_IGN);
#endif

    if (been_here_before == 1) {
        (void) fprintf(stderr, "You've called a NULL function pointer!!!!!\n");
        (void) fprintf(stderr, "This has been a public service announcement from\n");
        (void) fprintf(stderr, "your friendly paranoid rpc library\n");
        exit(RPC_BAD_EXIT);
    }

    been_here_before = 1;

    errProgramName(argv[0]);

    if (argc == 10) {

        /* determine the application is byte swapped relative to the server */

        RPCByteSwappedApplication(atoi(argv[5]),
                                         atoi(argv[6]),
                                         atoi(argv[7]),
                                         atoi(argv[8]),
                                         atoi(argv[9]));
            
        /* host port protocol */
        if (RPCConnectToServer(argv[2], atoi(argv[3]), argv[4],
                                        &RPCSendStream,
                                        &RPCReceiveStream) == RPC_ERROR) {
            (void) fprintf(stderr, "RPC Error: can not connect to the server\n");
            exit(RPC_BAD_EXIT);
        }

    } else {
        (void) fprintf(stderr, "RPC Error: incorrect number of arguments to remote application\n");
        exit(RPC_BAD_EXIT);
    }

    if (vemInitializeApplication(&display, &spot, &cmdList, &userOptionWord)
        != OCT_OK) {
        (void) fprintf(stderr, "RPC Error: application: error in initialization\n");
        exit(RPC_BAD_EXIT);
    }

    if ((size = UserMain(display, &spot, cmdList, userOptionWord, &CommandArray)) < 0) {
        (void) fprintf(stderr, "RPC Error: UserMain returned a value less than zero\n");
        exit(RPC_BAD_EXIT);
    }
    
    if (RPCApplicationFunctionComplete() != RPC_OK) {
        exit(RPC_BAD_EXIT);
    }

    /*
     * tell VEM about the remote user functions
     */

    if (vemSendMenu(CommandArray, size) != OCT_OK) {
        exit(RPC_BAD_EXIT);
    }
    
    /* process VEM remote user function requests */
#ifndef PTKCODE
    if (RPCApplicationProcessEvents(CommandArray, size) != RPC_OK) {
        exit(RPC_BAD_EXIT);
    }
#endif
#ifdef PTKCODE
    if (ptkTkSetup(CommandArray, size) != RPC_OK) {
	exit(RPC_BAD_EXIT);
    }
#endif

    return RPC_GOOD_EXIT;
}

#endif /* VAXLISP */

#ifndef lint
/*
 * XXX this is to make sure appOct.c (and all of the Oct routines)
 * are brought in before any routines for 'liboct.a'
 */
static void (*dummy)() = octBegin;
#endif
