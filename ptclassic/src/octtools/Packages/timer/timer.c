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
/*LINTLIBRARY*/
/*
 * timer package
 *
 *   Rick Spickelmier
 */
#include "copyright.h"
#include "port.h"
#ifndef sgi
#define HAS_TIMEB
#include <sys/timeb.h>
#endif
#include <sys/time.h>
#include <sys/resource.h>
#ifdef PTSOL2
#include <fcntl.h>
/* sys/rusage.h is in Solaris2.3, but not Solaris2.4 */
/*#include <sys/rusage.h>*/
#include <sys/procfs.h>
#endif

#if defined(PTSUN4) && defined(__GNUC__)
/* SunOS4.1.3 under gcc */
extern int getrusage(int who, struct rusage *rusage);
#endif

struct timestruct {
#ifdef HAS_TIMEB
   struct timeb startElapsedTime;
   struct timeb currentElapsedTime;
#else
   struct timeval startElapsedTime;
   struct timeval currentElapsedTime;
#endif
   struct timeval startUserTime;
   struct timeval currentUserTime;
   struct timeval startSystemTime;
   struct timeval currentSystemTime;
};

void timerContinue();

void
timerReset(timer)
struct timestruct *timer;
{
#ifdef HAS_TIMEB
   timer->currentElapsedTime.time = 0;
   timer->currentElapsedTime.millitm = 0;
#else
   timer->currentElapsedTime.tv_sec = 0;
   timer->currentElapsedTime.tv_usec = 0;
#endif
   timer->currentUserTime.tv_sec = 0;
   timer->currentUserTime.tv_usec = 0;
   timer->currentSystemTime.tv_sec = 0;
   timer->currentSystemTime.tv_usec = 0;
}

struct timestruct *
timerCreate()
{
    struct timestruct *timer =
	    (struct timestruct *) malloc(sizeof(struct timestruct));

    timerReset(timer);
    return(timer);
}

void
timerContinue(timer)
struct timestruct *timer;
{
#ifdef HAS_TIMEB
    struct timeb tb;
#else
    struct timeval tb;
#endif
#ifdef PTSOL2
    int             fd;
    char            proc[BUFSIZ];
    prusage_t       rst;
#else
    struct rusage rst;
#endif
    extern int ftime();

#ifdef HAS_TIMEB
    ftime(&tb);
    timer->startElapsedTime.time = tb.time;
    timer->startElapsedTime.millitm = tb.millitm;
#else
#if defined(_BSD_TIME) || defined(_BSD_COMPAT)
    /* For irix5 */
    BSDgettimeofday(&tb,(struct timezone*)NULL);
#else
    gettimeofday(&tb);
#endif /* _BSD_TIME || _BSD_COMPAT*/

    timer->startElapsedTime.tv_sec = tb.tv_sec;
    timer->startElapsedTime.tv_usec = tb.tv_usec;
#endif

#ifdef PTSOL2
    /* For Solaris2, taken from solaris2_porting.faq */
    sprintf(proc,"/proc/%d", (int)getpid());
    if ((fd = open(proc, O_RDONLY)) == -1)
      perror("timerContinue(): open");
    if (ioctl(fd, PIOCUSAGE, &rst) == -1)
      perror("timerContinue(): ioctl");
#else /* PTSOL2 */
    (void) getrusage(0, &rst);
#endif /* PTSOL2 */

#ifdef PTSOL2
    timer->startUserTime.tv_sec = rst.pr_utime.tv_sec;
    timer->startUserTime.tv_usec = rst.pr_utime.tv_nsec/1000;
    timer->startSystemTime.tv_sec = rst.pr_stime.tv_sec;
    timer->startSystemTime.tv_usec = rst.pr_stime.tv_nsec/1000;
/*
    timer->startUserTime.tv_sec = rst.ru_utime.tv_sec;
    timer->startUserTime.tv_usec = rst.ru_utime.tv_usec;
    timer->startSystemTime.tv_sec = rst.ru_stime.tv_sec;
    timer->startSystemTime.tv_usec = rst.ru_stime.tv_usec;
*/
#else
    timer->startUserTime = rst.ru_utime;
    timer->startSystemTime = rst.ru_stime;
#endif

    return;
}

void
timerStart(timer)
struct timestruct *timer;
{
    timerContinue(timer);
    return;
}

void
timerStop(timer)
struct timestruct *timer;
{
#ifdef HAS_TIMEB
    struct timeb tb;
#else
    struct timeval tb;
#endif
#ifdef PTSOL2
    int             fd;
    char            proc[BUFSIZ];
    prusage_t       rst;
#else
    struct rusage rst;
#endif
    extern int ftime();

#ifdef HAS_TIMEB
    ftime(&tb);
    timer->currentElapsedTime.time
             += (tb.time - timer->startElapsedTime.time);
    if (tb.millitm < timer->startElapsedTime.millitm) {
	timer->currentElapsedTime.time--;
	timer->currentElapsedTime.millitm
		 += 1000 + (tb.millitm - timer->startElapsedTime.millitm);
    } else {
	timer->currentElapsedTime.millitm
		 += (tb.millitm - timer->startElapsedTime.millitm);
    }
#else /*HAS_TIMEB*/
#if defined(_BSD_TIME) || defined(_BSD_COMPAT)
    /* For irix5 */
    BSDgettimeofday(&tb,(struct timezone*)NULL);
#else
    gettimeofday(&tb);
#endif /* _BSD_TIME || _BSD_COMPAT*/

    timer->currentElapsedTime.tv_sec
             += (tb.tv_sec - timer->startElapsedTime.tv_sec);
    if (tb.tv_usec < timer->startElapsedTime.tv_usec) {
	timer->currentElapsedTime.tv_sec--;
	timer->currentElapsedTime.tv_usec
		 += 1000000 + (tb.tv_usec - timer->startElapsedTime.tv_usec);
    } else {
	timer->currentElapsedTime.tv_usec
		 += (tb.tv_usec - timer->startElapsedTime.tv_usec);
    }
#endif /*HAS_TIMEB*/

#if defined(PTSOL2)
    /* For Solaris2, taken from solaris2_porting.faq */
    sprintf(proc,"/proc/%d", (int)getpid());
    if ((fd = open(proc,O_RDONLY)) == -1)
      perror("util_cpu_time(): open");
    if (ioctl(fd, PIOCUSAGE, &rst) == -1)
      perror("util_cpu_time(): ioctl");

    timer->currentUserTime.tv_sec
             += (rst.pr_utime.tv_sec - timer->startUserTime.tv_sec);

    if (rst.pr_utime.tv_nsec*1000 < timer->currentUserTime.tv_usec) {
	timer->currentUserTime.tv_sec --;
	timer->currentUserTime.tv_usec
		 += 1000000 + 
		    (rst.pr_utime.tv_nsec*1000 - timer->startUserTime.tv_usec);
    } else {
	timer->currentUserTime.tv_usec
		 += (rst.pr_utime.tv_nsec*1000 - timer->startUserTime.tv_usec);
    }

#else /*PTSOL2*/
    (void) getrusage(0, &rst);

    timer->currentUserTime.tv_sec
             += (rst.ru_utime.tv_sec - timer->startUserTime.tv_sec);

    if (rst.ru_utime.tv_usec < timer->currentUserTime.tv_usec) {
	timer->currentUserTime.tv_sec --;
	timer->currentUserTime.tv_usec
		 += 1000000 + 
		    (rst.ru_utime.tv_usec - timer->startUserTime.tv_usec);
    } else {
	timer->currentUserTime.tv_usec
		 += (rst.ru_utime.tv_usec - timer->startUserTime.tv_usec);
    }

    timer->currentSystemTime.tv_sec
             += (rst.ru_stime.tv_sec - timer->startSystemTime.tv_sec);

    if (rst.ru_stime.tv_usec < timer->currentSystemTime.tv_usec) {
	timer->currentSystemTime.tv_sec--;
	timer->currentSystemTime.tv_usec
		 += 1000000 
		    + (rst.ru_stime.tv_usec - timer->startSystemTime.tv_usec);
    } else {
	timer->currentSystemTime.tv_usec
		 += (rst.ru_stime.tv_usec - timer->startSystemTime.tv_usec);
    }
#endif /*PTSOL2*/
    return;
}

double
timerElapsedTime(timer)
struct timestruct *timer;
{
#ifdef HAS_TIMEB
    return((double) timer->currentElapsedTime.time
	   + (double) timer->currentElapsedTime.millitm / 1000.0);
#else
    return((double) timer->currentElapsedTime.tv_sec
	   + (double) timer->currentElapsedTime.tv_usec / 1000000.0);
#endif
}
    
double
timerUserTime(timer)
struct timestruct *timer;
{
    return((double) timer->currentUserTime.tv_sec
	   + (double) timer->currentUserTime.tv_usec / 1000000.0);
}

double
timerSystemTime(timer)
struct timestruct *timer;
{
    return((double) timer->currentSystemTime.tv_sec
	   + (double) timer->currentSystemTime.tv_usec / 1000000.0);
}

void
timerPrint(timer, fp)
struct timestruct *timer;
FILE *fp;
{
    (void) fprintf(fp, "\nElapsed Time = %f\n", timerElapsedTime(timer));
    (void) fprintf(fp, "User Time = %f\n", timerUserTime(timer));
    (void) fprintf(fp, "System Time = %f\n\n", timerSystemTime(timer));
    return;
}

void
timerFree(timer)
struct timestruct *timer;
{
    (void) free((char *) timer);
    return;
}
