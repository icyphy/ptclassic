/****************************************************************
SCCS version identification
$Id$

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

Programmer: J. Pino
Date: 10/2/92

*******************************************************************/

#ifndef _CodeStreamList_h
#define _CodeStreamList_h 1

#include "NamedList.h"
#include "CodeStream.h"

class CodeStreamList : private NamedList
{
public:
	// adds a new StringList called name to the list.  If a StringList
	// called name already exists in the list with a different
	// StringList* return FALSE else return TRUE
	int append(CodeStream* stream, const char* name)
	{
        	return NamedList::append(stream, name);
	}

	// For temporary backward compatibility.
	int add(const char* name, CodeStream* stream)
	{
	    return append(stream, name);
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
