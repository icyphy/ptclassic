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

#include "NameList.h"
#include "StringList.h"

class StringListList : private NameList {
public:
	// adds a new StringList called name to the list.  If a StringList
	// called name already exists in the list with a different
	// StringList* return FALSE else return TRUE
	int add(const char* name, StringList* slist) {
        	return NameList::add(name,slist);
	}

	// return the pointer for the object of the specified name.  if the
	// StringList does not exist, return NULL
	StringList* get(const char* name) {
        	return (StringList*)NameList::get(name);
	}
	
	// remove a StringList from the list.  if the StringList
	// does not exist, return FALSE.
	int NameList::remove(const char* name);
};

#endif
