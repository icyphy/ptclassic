/******************************************************************
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

 Programmer: J. Pino, J. Buck, T. M. Parks

 SymbolList stuff.

*******************************************************************/
#ifndef _SymbolList_h
#define _SymbolList_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "NamedList.h"
#include "StringList.h"

const char defaultSepChar = '_';

// All SymbolList classes must be derived from BaseSymbolList.  This
// class contains the count numSymbols that makes each symbol unique.
class BaseSymbolList : private NamedList
{
public:
	BaseSymbolList(char sep=defaultSepChar, int* count=NULL)
	    : separator(sep), counter(count) {}

	~BaseSymbolList() { deleteSymbols(); }

	void setSeparator(char sep) { separator = sep; }
	void setCounter(int *count) { counter = count; }

    	void initialize() { deleteSymbols(); NamedList::initialize(); }

	const char* get(const char* name = NULL) const
	{ return (const char*)NamedList::get(name); }

	const char* prepend(const char* name);
	const char* append(const char* name);
	int remove(const char* name = NULL);
private:
	// Make a unique symbol.
	StringList symbol(const char*);

	void deleteSymbols();
    	int* counter;
    	char separator;
};

class SymbolList : private BaseSymbolList {
public:
	SymbolList(char sep=defaultSepChar, int *count=NULL)
	    : BaseSymbolList(sep, count) {}
	~SymbolList() {}
	const char* lookup(const char* name);
	BaseSymbolList::setSeparator;
	BaseSymbolList::setCounter;
	BaseSymbolList::initialize;
};

// Class for unique nested symbol generation.
class SymbolStack : private BaseSymbolList {
public:
	SymbolStack(char sep=defaultSepChar, int *count=NULL)
	    : BaseSymbolList(sep, count) {}
	~SymbolStack() {}

	const char* push(const char* tag="L")
	{
	    return prepend(tag);
	}

	StringList pop();
	BaseSymbolList::get;
	BaseSymbolList::setSeparator;
	BaseSymbolList::setCounter;
	BaseSymbolList::initialize;
};

// For temporary backward compatibility.
typedef SymbolStack NestedSymbolList;

// Class for generation of unique symbols with named scope.
class ScopedSymbolList : private NamedList
{
public:
    ScopedSymbolList(char sep=defaultSepChar, int *count=NULL)
	: separator(sep), counter(count) {}
    ~ScopedSymbolList() { deleteSymbolLists(); }

    void setSeparator(char);
    void setCounter(int*);

    void initialize() { deleteSymbolLists(); NamedList::initialize(); }

    const char* lookup(const char* scope, const char* name);

private:
    void deleteSymbolLists();
    int *counter;
    char separator;
};
#endif
