static const char *file_id = "$RCSfile$";
/**************************************************************************
Version identification:
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
Programmer: J. Pino, T. M. Parks
*************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "NamedList.h"
#include "miscFuncs.h"

// Hidden class used by NamedList.
class NamedNode
{
    friend class NamedList;
    friend class NamedListIter;

    NamedNode(Pointer object, const char* name);
    ~NamedNode();
    char* name;
    Pointer object;
};

NamedNode::NamedNode(Pointer obj, const char* nm) 
{
    object = obj;
    name = savestring(nm);
}

NamedNode::~NamedNode()
{
    LOG_DEL; delete name;
}

// Next object in the list, not the next NamedNode.
Pointer NamedListIter::next()
{
    NamedNode* node = (NamedNode*)ListIter::next();
    if (node != NULL) return node->object;
    else return NULL;
}

// Put a named object at the end of the list.
// Return FALSE on error.
int NamedList::append(Pointer object, const char* name)
{
    NamedNode* node = getNamedNode(name);
    if (node != NULL)
    {
	// Adding two objects with the same name is not allowed.
	if (node->object != object) return FALSE;
	// Adding the same object twice is allowed. Only one copy is kept.
	else return TRUE;
    }
    else
    {
	LOG_NEW; node = new NamedNode(object, name);
	SequentialList::put(node);
	return TRUE;
    }
}


// Put a named object at the beginning of the list.
void NamedList::prepend(Pointer object, const char* name)
{
    LOG_NEW; NamedNode* node = new NamedNode(object, name);
    SequentialList::prepend(node);
}

// Get a named object from the list.
// Return NULL on error.
Pointer NamedList::get(const char* name) const
{
    NamedNode* node = getNamedNode(name);
    if (node != NULL) return node->object;
    else return NULL;
}

// Remove a named object from the list.
// Return FALSE on error.
int NamedList::remove(const char* name)
{
    NamedNode* node = getNamedNode(name);
    if (node != NULL) 
    {
	LOG_DEL; delete node;
	return SequentialList::remove(node);
    }
    else return FALSE;
}

// Delete all the nodes in the list.
void NamedList::deleteNodes()
{
    ListIter node(*this);
    NamedNode* n;
    while ((n = (NamedNode*)node++) != NULL)
    {
	LOG_DEL; delete n;
    }
}

// Get the NamedNode for a named object in the list.
// Return NULL on error.
NamedNode* NamedList::getNamedNode(const char* name) const
{
    NamedNode* n;
    if (name == NULL)
    {
	n = (NamedNode*)SequentialList::head();
    }
    else
    {
	ListIter node(*this);
	while ((n = (NamedNode*)node++) != NULL)
	{
	    if(strcmp(name, n->name) == 0) break;
	}
    }
    return n;
}
