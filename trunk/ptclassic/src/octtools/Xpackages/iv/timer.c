#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1994 The Regents of the University of California.
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
#include "copyright.h"
#include "port.h"
#include <sys/time.h>
#include <signal.h>

#ifdef linux
#define sigvec		sigaction
#define sv_mask		sa_mask
#define sv_flags	sa_flags
#define sv_handler	sa_handler
#endif

#if defined(__sparc) && !defined(__svr4__)  && defined (__GNUC__)
/* SunOS4.1.3 under gcc. */
extern int sigvec(int, struct sigvec *, struct sigvec *);
extern int setitimer(int, struct itimerval *, struct itimerval *);
extern int sigpause(int);
#endif

#ifdef sequent

int  ignoreAlarm( sig, code, scp)
    int sig, code;
    struct sigcontext *scp;
{
    /* printf( "Alarm went off\n"); */
    return ;
}

/* This is a fairly accurate timer routine */
void Timer( val )
    int  val;			/* microseconds */
{
    struct timeval init, current;
    struct itimerval value, ovalue;
    struct sigvec sig, osig;
    extern int ignoreAlarm();

    sig.sv_handler = ignoreAlarm;
    sig.sv_mask = 0;
    sig.sv_onstack = 0;
    (void) sigvec( SIGALRM, &sig, &osig );

    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 0;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = val;
    (void) setitimer( ITIMER_REAL, &value, &ovalue);
    (void) sigpause( 0 );		/* Wait for alarm to come */
    
    (void) sigvec( SIGALRM, &osig, 0 ); /* restore previous status */

    return;
}

#else	/* VAXes etc. */

void  ignoreAlarm()
{
    /* printf( "Alarm went off\n"); */
    return ;
}

#ifdef SYSV
/* This is a fairly accurate timer routine */
void Timer( val )
    int  val;			/* microseconds */
{
    struct itimerval value, ovalue;
    struct sigaction sig, osig;

    sig.sa_handler = ignoreAlarm;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;

    sigaction(SIGALRM, &sig, &osig);

    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 0;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = val;
    setitimer(ITIMER_REAL, &value, &ovalue);
    sigpause(0);		/* Wait for alarm to come */
    
    sigaction(SIGALRM, &osig, 0); /* restore previous status */

    return;
}
#else

/* This is a fairly accurate timer routine */
void Timer( val )
    int  val;			/* microseconds */
{
    struct itimerval value, ovalue;
    struct sigvec sig, osig;
    extern void ignoreAlarm();

    sig.sv_handler = ignoreAlarm;
    sig.sv_mask = 0;
    sig.sv_flags = 0;
/* The HP-UX documentation claims to support sigvec in signal.h,
	but this is not so (for version 8.07). */
#ifdef hpux
    (void) sigvector( SIGALRM, &sig, &osig );
#else
    (void) sigvec( SIGALRM, &sig, &osig );
#endif

    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 0;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = val;
    (void) setitimer( ITIMER_REAL, &value, &ovalue);
    (void) sigpause( 0 );		/* Wait for alarm to come */
    
#ifdef hpux
    (void) sigvector( SIGALRM, &osig, 0 ); /* restore previos status */
#else
    (void) sigvec( SIGALRM, &osig, 0 ); /* restore previous status */
#endif
    return;
}

#endif /*SYSV*/
#endif /*sewuent*/
