/* Version Identification:
 * $Id$
 */
/*
Copyright (c) 1990-%Q% The Regents of the University of California.
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
*/
/*
 * timer package
 *
 *  Rick Spickelmier
 */

#ifndef TIMER_H
#define TIMER_H

#include "ansi.h"

struct dummy_timer {
    int dummy;
};

typedef struct dummy_timer *timer;

EXTERN timer timerCreate
	NULLARGS;
EXTERN void timerReset
	ARGS((timer tm));
EXTERN void timerStart
	ARGS((timer tm));
EXTERN void timerContinue
	ARGS((timer tm));
EXTERN void timerStop
	ARGS((timer tm));
EXTERN void timerPrint
	ARGS((timer tm, FILE *fp));
EXTERN void timerFree
	ARGS((timer tm));

EXTERN double timerElapsedTime
	ARGS((timer tm));
EXTERN double timerUserTime
	ARGS((timer tm));
EXTERN double timerSystemTime
	ARGS((timer tm));

#endif /* TIMER_H */

