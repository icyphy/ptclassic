/****************************************************************
SCCS version identification
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

Programmer: J. Pino, T. M. Parks
Date: 10/1/92

NamedList is container list class.
Each node on the list has an associated unique name.
*******************************************************************/

#ifndef _NamedList_h
#define _NamedList_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"
#include "type.h"

class NamedNode;

class NamedList : private SequentialList
{
    friend class NamedListIter;
public:
    NamedList() {}

    ~NamedList()
    {
	deleteNodes();
    }

    void initialize()
    {
	deleteNodes();
	SequentialList::initialize();
    }

    // Put an object called "name" at the end of the list.
    // If a different object with that name is already
    // in the list, return FALSE.
    int append(Pointer object, const char* name);

    // Put an object called "name" at the beginning of the list.
    int prepend(Pointer object, const char* name);

    // Get the object called "name" from the list.
    // With no name, get the first object in the list.
    // If the named object is not in the list, return NULL.
    Pointer get(const char* name = NULL) const;

    // Remove the object called "name" from the list.
    // With no name, remove the first object in the list.
    // If the named object is not in the list, return FALSE.
    int remove(const char* name = NULL);

private:
    // Delete all the nodes in the list.
    void deleteNodes();

    // Get the NamedNode for the object called "name".
    // If no name is specified, get the first node in the list.
    // If the named object is not in the list, return NULL.
    NamedNode* getNamedNode(const char* name = NULL) const;
};

class NamedListIter : private ListIter
{
public:
    NamedListIter(NamedList& list) : ListIter(list) {}

    Pointer next();
    Pointer operator ++() { return next(); }

    ListIter::reset;
};

#endif
