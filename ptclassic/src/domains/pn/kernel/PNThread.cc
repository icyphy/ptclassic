static const char file_id[] = "$RCSfile$";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	18 February 1992
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFThread.h"

#ifdef __GNUG__
extern "C"
{
    int lwp_setpri(thread_t, int);
}
#endif

// Change the Thread's priority.
int MTDFThread::setPriority(unsigned int priority)
{
    return lwp_setpri(*this, priority);
}
