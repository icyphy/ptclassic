/* 
   Tycho's support for scheduling and interleaving C modules.

Authors: John Reekie.

Version: @(#)tytimer.h	1.3 04/29/98

Copyright (c) 1997-1998 The Regents of the University of California.
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
#include "tk.h"
#include "tcl.h"
#include "itk.h"
#include "itcl.h"


/*
 * Ty_TimerPeriod
 *
 * Set the period of the scheduler timer.
*/
void
Ty_TimerPeriod( int ms );

/*
 * Ty_TimerStart
 *
 * Start the scheduler timer running.
*/
void
Ty_TimerStart();


/*
 * Ty_TimerStop
 *
 * Stop the timer.
 */
void
Ty_TimerStop();

/*
 * Ty_TimerElapsed
 *
 * Test if the timer has elapsed. This call can be made multiple
 * times; the elapsed flag is not reset until the timer is started
 * again. Note that this call must be made in order to actually
 * allow the signal to get through.
 */
int
Ty_TimerElapsed();

/*
 * Ty_DoAllEvents
 *
 * Process all pending Tk events. This only does something if the timer
 * has gone off and there are pending Tk events. After processing
 * all events, reset the timer if the event loop is still active.
 */
void
Ty_DoAllEvents();
