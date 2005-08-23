/****************************************************************
SCCS version identification
@(#)NamedList.h	1.9 1/12/96

Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

Programmer: J. Pino, T. M. Parks
Date: 10/1/92

NamedList is container list class.
Each node on the list has an associated name.
*******************************************************************/

#ifndef _NamedList_h
#define _NamedList_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"
#include "type.h"

class NamedNode {
public:
    NamedNode(Pointer object, const char* name);
    ~NamedNode();
    char* name() { return nm; }
    Pointer object() { return obj; }
private:
    friend class NamedList;
    friend class NamedListIter;
    char* nm;
    Pointer obj;
};

class NamedList : public SequentialList
{
    friend class NamedListIter;
public:
    NamedList() {}
    ~NamedList() { deleteNodes(); }
    void initialize() { deleteNodes(); SequentialList::initialize(); }

    // Put an object called "name" at the end of the list.
    // If a different object with that name is already
    // in the list, return FALSE.
    int append(Pointer object, const char* name);

    // Put an object called "name" at the beginning of the list.
    // Multiple objects with the same "name" are allowed.
    // Only the most recently prepened object with "name" is visible.
    void prepend(Pointer object, const char* name);

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
    // If the named node is not in the list, return NULL.
    NamedNode* getNamedNode(const char* name = NULL) const;
};

// This iterator returns the pointers to the objects
class NamedListIter : private ListIter
{
public:
    NamedListIter(NamedList& l) : ListIter(l) {}
    Pointer next();
    Pointer operator ++(POSTFIX_OP) { return next(); }
    ListIter::reset;
};

#endif
