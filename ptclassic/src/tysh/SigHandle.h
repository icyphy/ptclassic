#ifndef _SigHandle_h
#define _SigHandle_h 1
/****************************************************************************
Version identification:
@(#)SigHandle.h

Author: Joel R. King

Defines the signal handlers for Tycho in both release and debug modes.

Copyright (c) 1990-1995 The Regents of the University of California.
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

**************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

/****************************************************************************/

extern char **environ; /* An array of pointers to strings containing the    */
                       /* environmental variables. This is needed by the    */
                       /* execle() function, as an argument to setup the    */
                       /* environment for the process that its launching.   */

/****************************************************************************/

int setReleaseHandlers(void);
int setDebugHandlers(void);
static void signalHandlerRelease(int);
static void signalHandlerDebug(int);
static void abortHandling(void);

/****************************************************************************/

#endif
