#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
/* LINTLIBRARY */
#include "copyright.h"
#include "port.h"
#include "utility.h"

#ifdef IBM_WATC		/* IBM Waterloo-C compiler (same as bsd 4.2) */
#ifndef BSD
#define BSD
#endif
#ifndef void
#define void int
#endif
#endif

#ifdef ultrix
#ifndef BSD
#define BSD
#endif
#endif

#ifdef aiws
#ifndef UNIX10
#define UNIX10
#endif
#endif

/* default */
#if !defined(BSD) && !defined(UNIX10) && !defined(UNIX60) && !defined(UNIX100) && !defined(UNIX50) && !defined(vms)
#define BSD
#endif

#ifdef BSD
#include <sys/time.h>
#include <sys/resource.h>
#endif

#ifdef hpux
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#endif

#ifdef UNIX10
#include <sys/times.h>
#endif

#ifdef UNIX60
#include <sys/times.h>
#endif

#ifdef UNIX100
#include <sys/times.h>
#endif

#ifdef vms
#include <time.h>
#endif


/*
 *   util_cpu_time -- return a long which represents the elapsed processor
 *   time in milliseconds since some constant reference
 */
long 
util_cpu_time()
{
    long t = 0;

#ifndef hpux
#ifdef BSD
    struct rusage rusage;
    (void) getrusage(RUSAGE_SELF, &rusage);
    t = (long) rusage.ru_utime.tv_sec*1000 + rusage.ru_utime.tv_usec/1000;
#endif
#endif

#ifdef hpux
    struct tms buffer;
    long tickspersec;
    times(&buffer);
    tickspersec = sysconf(CLK_TCK);
    t = buffer.tms_utime * (1000/tickspersec);
#endif

#ifdef IBMPC
    long ltime;
    (void) time(&ltime);
    t = ltime * 1000;
#endif

#ifdef UNIX10			/* times() with 10 Hz resolution */
    struct tms buffer;
    (void) times(&buffer);
    t = buffer.tms_utime * 100;
#endif

#ifdef UNIX60			/* times() with 60 Hz resolution */
    struct tms buffer;
    times(&buffer);
    t = buffer.tms_utime * 16.6667;
#endif

#ifdef UNIX100
    struct tms buffer;		/* times() with 100 Hz resolution */
    times(&buffer);
    t = buffer.tms_utime * 10;
#endif

#ifdef vms
    struct tbuffer buffer;	/* times() with 100 Hz resolution */
				/* but a different buffer struct */
    times(&buffer);
    t = buffer.proc_user_time * 10;
#endif

    return t;
}
