/* Version $Id$
   Copyright 1992 The Regents of the University of California
   All Rights Reserved.

   Author:      T. M. Parks
   Date:        4 October 1992

*/

static const char* file_id = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "UniqueStringList.h"

// Determine if a string is already present.
// If it is, remember it and return FALSE.
int UniqueStringList::isUnique(const char* s) 
{
    StringListIter string(*this);
    const char* u;

    // Search the list.
    while ((u = string++) != NULL)
    {
	if (strcmp(s,u) == 0)	// String is already in the list.
	    return FALSE;
    }

    // Add string to the list.
    *this += s;
    return TRUE;
}
