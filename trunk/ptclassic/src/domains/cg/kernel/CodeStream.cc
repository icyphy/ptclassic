/* $Id$
   Copyright 1992 The Regents of the University of California
   All rights reserved.

   Author:	T. M. Parks
   Date:	8 October 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CodeStream.h"

int CodeStream::put(const char* string, const char* name)
{
    if (name == NULL)
    {
	*this << string;
	return TRUE;
    }
    else
    {
	int unique = sharedNames.isUnique(name);
	if (unique) *this << string;
	return unique;
    }
    return FALSE;
}

void CodeStream::initialize() {
    sharedNames.initialize();
    StringList :: initialize();
}
