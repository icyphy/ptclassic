/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	14 February 1992

*/

#ifndef _MTDFThread_h
#define _MTDFThread_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Thread.h"

class MTDFThread : public Thread
{
public:
    // Change the Thread's priority.
    int setPriority(unsigned int p);
};

#endif