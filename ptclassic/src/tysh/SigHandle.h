#ifndef _SigHandle_h
#define _SigHandle_h 1
/****************************************************************************
Version identification:
@(#)SigHandle.h	1.13	2/12/96

Author: Joel R. King

Defines the signal handlers for Tycho in both release and debug modes.

Copyright (c) 1990-1996 The Regents of the University of California.
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

// Define SIG_PT
#include "ptsignals.h"

#if !defined(PTSOL2) && !defined(PTSUN4) && !defined(PTIRIX5)
typedef void (*SIG_PT)(int);
#endif

// SIGEMT and SIGSYS are possible not defined in /usr/include/signal.h  
//   or /usr/include/sys/signal.h
#if defined(PTLINUX) || defined(PTLINUX_ELF)
#ifndef SIGEMT
#define SIGEMT 7	/* EMT instruction */
#endif
#ifndef SIGSYS
#define	SIGSYS	12	/* bad argument to system call */    
#endif
#endif

// Function prototypes
int setHandlers(SIG_PT sigHandler);
void signalHandler(int signo);
void abortHandling();
void setStrings(void) ;

#endif
