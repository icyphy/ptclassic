/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino, J. Buck

 SymbolList stuff.

*******************************************************************/
#ifndef _SymbolList_h
#define _SymbolList_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "NamedList.h"
#include "StringList.h"

// All SymbolList classes must be derived from BaseSymbolList.  This
// class contains the count numSymbols that makes each symbol unique.
class BaseSymbolList : private NamedList
{
public:
	BaseSymbolList(char sep='_')
	{
	    setSeparator(sep);
	}

	~BaseSymbolList()
	{
	    deleteSymbols();
	}

	void setSeparator(char sep= '_')
	{
	    separator = sep;
	}

    	void initialize()
	{
	    deleteSymbols();
	    NamedList::initialize();
	}

    	static void reset()
	{
	    numSymbols = 0;
	}

	const char* get(const char* name = NULL) const
	{
	    return (const char*)NamedList::get(name);
	}

	const char* prepend(const char* name);
	const char* append(const char* name);
	int remove(const char* name = NULL);
private:
	// Make a unique symbol.
	StringList symbol(const char*);

	void deleteSymbols();
    	static int numSymbols;
    	char separator;
};

class SymbolList : private BaseSymbolList {
public:
	SymbolList(char sep='_') : BaseSymbolList(sep) {}
	~SymbolList() {}
	const char* lookup(const char* name);
	BaseSymbolList::setSeparator;
	BaseSymbolList::initialize;
};

// Class for unique nested symbol generation.
class SymbolStack : private BaseSymbolList {
public:
	SymbolStack(char sep='_') : BaseSymbolList(sep) {}
	~SymbolStack() {}

	const char* push(const char* tag="L")
	{
	    return prepend(tag);
	}

	StringList pop();
	BaseSymbolList::get;
	BaseSymbolList::setSeparator;
	BaseSymbolList::initialize;
};

// For temporary backward compatibility.
typedef SymbolStack NestedSymbolList;

class SymbolListList : private NamedList {
public:
	SymbolListList() {}
	~SymbolListList() {}

	// For temporary backward compatibility.
	int add(const char* name, SymbolList* list)
	{
	    return append(list, name);
	}

        // put a new SymbolList called name to the list.
	// Return FALSE if a different SymbolList with the same name
	// is already in the list.
        int append(SymbolList* list, const char* name)
	{ 
	    return NamedList::append(list, name);
        }
         
        // return the pointer for the object of the specified name.  if the
        // SymbolList does not exist, return NULL
        SymbolList* get(const char* name) const
	{
	    return (SymbolList*)NamedList::get(name);
        }
         
        // remove a SymbolList from the list.  if the StringList
        // does not exist, return FALSE.
	int remove(const char* name)
	{
            return NamedList::remove(name);
	}
};
#endif
