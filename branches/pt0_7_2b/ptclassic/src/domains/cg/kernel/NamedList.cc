static const char *file_id = "NameList.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.
Programmer: J. Pino
*************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <stream.h>
#include "NameList.h"
#include "miscFuncs.h"

NameNode :: NameNode(const char* nm, Pointer a) 
{
    object=a;
    name = savestring(nm);
}

NameNode :: ~NameNode()
{
    LOG_DEL ; delete(name);
}

//Add a new name, object pair
int NameList :: add(const char* name, Pointer object)
{
	NameNode* foo = getNameNode(name);
	if (foo) {
		if (foo->object != object) 
			return FALSE;
		else
			return TRUE;
	}
	LOG_NEW; NameNode* node = new NameNode(name,object);
    	put(node);
   	return TRUE;
}

// Lookup object by name, if one doesn't exist, return NULL
Pointer NameList::get(const char* name) 
{
	NameNode* foo = getNameNode(name);
	if (foo) return foo->object;
	return NULL;
}

// remove object by name, if object doesn't exist, return FALSE
int NameList::remove(const char* name)
{
	NameNode* foo = getNameNode(name);
	if (foo) 
	{
		remove(foo);
		return TRUE;
	}
	return FALSE;
}

	
void NameList::initialize() 
{
    ListIter next(*this);
    for (int i=size(); i > 0; i--) 
    {
        NameNode* p = (NameNode*)next++;
        LOG_DEL; delete p;
    }
    SequentialList::initialize();
}

// destructor -- a subset of initialize since baseclass will do the
// rest.
NameList::~NameList() 
{
    ListIter next(*this);
    for (int i=size(); i > 0; i--) 
    {
		NameNode* p = (NameNode*)next++;
		LOG_DEL; delete p;
    }
}

// private method - return the NameNode pointer named 'name'
NameNode* NameList:: getNameNode(const char* name)
{
	ListIter next(*this);
	NameNode* p;
    while ((p = (NameNode*)next++) != 0)
    	if(!strcmp(name,p->name)) return p;
	return NULL;
}
