/****************************************************************
SCCS version identification
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

Programmer: J. Pino
Date: 10/1/92
NameList is container list class.  Each node on the list, has
an associated unique name.  Four functions are supported:
add, get, remove and initialize.
*******************************************************************/

#ifndef _NameList_h
#define _NameList_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"
#include "type.h"

class NameNode
{
	friend class NameList;

    const char* name;
    Pointer object;
    
    NameNode(const char* nm, Pointer a);
    ~NameNode();
};

class NameList : private SequentialList
{
public:
    // Empty the list.
    void initialize();

    // Destructor
    ~NameList();

    // add a new object called name.  If there exists another object
    // on the list has the same name but points to a different object,
    // return FALSE else return TRUE.
    int add(const char* name, Pointer object);

    // return the pointer for the object of the specified name.  if the
    // named pointer does not exist, return null
    Pointer get(const char* name);

    // remove a NameNode from the list.  if the named
    // pointer does not exist, return FALSE.
    int remove(const char* name);
private:
    // return the pointer to the NameNode named 'name'
    NameNode* getNameNode(const char* name);
};

#endif
