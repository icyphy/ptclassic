/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Author:	T.M. Parks
    Date:	1 July 1992

*/

#ifndef _MTDFThreadIter_h
#define _MTDFThreadIter_h

#include "MTDFThreadList.h"
#include "DataStruct.h"

class MTDFThreadIter : private ListIter
{
public:
    // Constructor.
    MTDFThreadIter(MTDFThreadList& tList) : ListIter(tList) {}

    // Next Thread on list.
    MTDFThread* next() { return (MTDFThread*)ListIter::next(); }
    MTDFThread* operator ++() { return next(); }

    // Reset to head of list.
    ListIter::reset;
};

#endif
