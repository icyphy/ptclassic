/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Author:	T.M. Parks
    Date:	1 July 1992

*/

#ifndef _MTDFThreadList_h
#define _MTDFThreadList_h

#include "MTDFThread.h"
#include "DataStruct.h"

class MTDFThreadList : public SequentialList
{
public:
    // Add Thread to list.
    void put(MTDFThread* t) { SequentialList::put(t); }

    // Remove Thread from list.
    int remove(MTDFThread* t) { return SequentialList::remove(t); }

    // First Thread on list.
    MTDFThread& head() const { return *(MTDFThread*)SequentialList::head(); }

    // Size of list.
    int size() const { return SequentialList::size(); }
};

#endif
