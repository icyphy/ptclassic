static const char SccsId [] = "$Id$";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
*************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "NamedList.h"
#include "miscFuncs.h"

// Constructor
NamedNode::NamedNode(Pointer objt, const char* name) 
{
    obj = objt;
    nm = savestring(name);
}

// Destructor
NamedNode::~NamedNode()
{
    LOG_DEL; delete [] nm;
}

// Next object in the list, not the next NamedNode.
Pointer NamedListIter::next()
{
    NamedNode* node = (NamedNode*)ListIter::next();
    if (node) return node->object();
    return 0;
}

// Put a named object at the end of the list.
// Return FALSE on error.
int NamedList::append(Pointer object, const char* name)
{
    NamedNode* node = getNamedNode(name);
    if (node) {
	// Adding two objects with the same name is not allowed, but
	// adding the same object twice is allowed (only one copy is kept).
	return (node->object() == object);
    }

    LOG_NEW; node = new NamedNode(object, name);
    SequentialList::append(node);
    return TRUE;
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
    if (node) return node->object();
    return 0;
}

// Remove a named object from the list.
// Return FALSE on error.
int NamedList::remove(const char* name)
{
    int retval = FALSE;
    NamedNode* node = getNamedNode(name);
    if (node) {
	retval = SequentialList::remove(node);
	LOG_DEL; delete node;
    }
    return retval;
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
    NamedNode* n = 0;
    if (name == 0)
    {
	n = (NamedNode*)SequentialList::head();
    }
    else
    {
	ListIter node(*this);
	while ((n = (NamedNode*)node++) != 0)
	{
	    if (strcmp(name, n->name()) == 0) break;
	}
    }
    return n;
}
