/* $Id$
   Copyright 1992 The Regents of the University of California
   All Rights Reserved.

   Author:	T. M. Parks
   Date:	8 October 1992

*/

#ifndef _CodeStream_h
#define _CodeStream_h

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "UniqueStringList.h"

class CodeStream : public StringList
{
public:
    // Add code to the stream.  If "name" is not NULL code is added
    // only if "name" has not appeared before.
    // Returns TRUE if code was added to the stream, FALSE otherwise.
    int put(const char* code, const char* name = NULL);

    // initialize myself and sharedNameds if exists.
    void initialize();

protected:
    UniqueStringList sharedNames;
};

#endif
