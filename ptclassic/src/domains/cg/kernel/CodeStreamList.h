/****************************************************************
SCCS version identification
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

Programmer: J. Pino
Date: 10/2/92

*******************************************************************/

#ifndef _CodeStreamList_h
#define _CodeStreamList_h 1

#include "NamedList.h"
#include "StringList.h"

// For temporary backward compatibility.
typedef StringList CodeStream;

class StringListList : private NamedList
{
public:
	// For temporary backward compatibility.
	int add(const char* name, CodeStream* stream)
	{
	    return append(stream, name);
	}

	// adds a new StringList called name to the list.  If a StringList
	// called name already exists in the list with a different
	// StringList* return FALSE else return TRUE
	int append(CodeStream* stream, const char* name)
	{
        	return NamedList::append(stream, name);
	}

	// return the pointer for the object of the specified name.
	CodeStream* get(const char* name) const
	{
        	return (CodeStream*)NamedList::get(name);
	}
	
	// Remove a CodeStream from the list. "name" is not optional.
	int remove(const char* name)
	{
	    return NamedList::remove(name);
	}
};

#endif
