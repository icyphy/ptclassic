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
/*LINTLIBRARY*/
/*
 * timer package
 *
 *   Rick Spickelmier
 */
#include "copyright.h"
#include "port.h"
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/resource.h>

struct timestruct {
   struct timeb startElapsedTime;
   struct timeb currentElapsedTime;
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
   timer->currentElapsedTime.time = 0;
   timer->currentElapsedTime.millitm = 0;
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
    struct timeb tb;
    struct rusage rst;
    extern int ftime();

    ftime(&tb);
    timer->startElapsedTime.time = tb.time;
    timer->startElapsedTime.millitm = tb.millitm;

    (void) getrusage(0, &rst);
    timer->startUserTime = rst.ru_utime;
    timer->startSystemTime = rst.ru_stime;

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
    struct timeb tb;
    struct rusage rst;
    extern int ftime();

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

    return;
}

double
timerElapsedTime(timer)
struct timestruct *timer;
{
    return((double) timer->currentElapsedTime.time
	   + (double) timer->currentElapsedTime.millitm / 1000.0);
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
    (void) fprintf(fp, "\nElapsed Time = %lf\n", timerElapsedTime(timer));
    (void) fprintf(fp, "User Time = %lf\n", timerUserTime(timer));
    (void) fprintf(fp, "System Time = %lf\n\n", timerSystemTime(timer));
    return;
}

void
timerFree(timer)
struct timestruct *timer;
{
    (void) free((char *) timer);
    return;
}
