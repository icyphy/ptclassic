/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
/*  Version $Id$
    Programmer:		T.M. Parks
    Date of creation:	18 February 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFThread.h"
#include "TimeVal.h"

#ifdef __GNUG__
extern "C"
{
    int lwp_setpri(thread_t, int);
    int lwp_sleep(timeval*);
    int pod_getmaxpri();
    int pod_setmaxpri(int);
}
#endif

// Constructor.
MTDFThread::MTDFThread(int priority, void (*thread)(MTDFStar*), MTDFStar* star)
    : Thread(priority, (void(*)(void*))thread, star)
{}

// Change the Thread's priority.
int MTDFThread::setPriority(int priority)
{
    return lwp_setpri(*this, priority);
}

// Set system-wide maximum priority.
int MTDFThread::setMaxPriority(int priority)
{
    return pod_setmaxpri(priority);
}

// System-wide maximum priority.
int MTDFThread::maxPriority()
{
    return pod_getmaxpri();
}

// System-wide minimum priority.
int MTDFThread::minPriority()
{
    return MINPRIO;
}

// Disable Thread for the specified time.
int MTDFThread::sleep(TimeVal delay)
{
    return lwp_sleep((timeval*)&delay);
}
