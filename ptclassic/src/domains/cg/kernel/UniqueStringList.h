/* Version $Id$
   Copyright 1992 The Regents of the University of California
   All Rights Reserved.

   Author:	T. M. Parks
   Date:	4 October 1992

*/

#ifndef _UniqueStringList_h
#define _UniqueStringList_h

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"

class UniqueStringList : private StringList
{
public:
    StringList::initialize;

    int isUnique(const char*);
};

#endif
