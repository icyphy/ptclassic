/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino, J. Buck

 SymbolList stuff.

*******************************************************************/
#ifndef _CGSymbol_h
#define _CGSymbol_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"
#include "NameList.h"

// A symbol consists of a key and a unique symbol defined to the key.
class Symbol {
public:
    Symbol(const char* key, const char* symbol);
    ~Symbol();
    const char* key;
    const char* symbol;
};

// All SymbolList classes must be derived from BaseSymbolList.  This
// class contains the count numSymbols that makes each symbol unique.
class BaseSymbolList : private SequentialList {
public:
    	BaseSymbolList(char sep= '_') {separator = sep;};
	~BaseSymbolList();
    	void initialize();
	void setSeparator(char sep= '_') {separator = sep;};
    	// Reset the unique symbol generator counter.
    	static void reset();
	const char* lookup(const char* name);
protected:
	const char* add(const char* name);
	Symbol* top() { return (Symbol*)head(); } ;
	int remove(Symbol* s);
private:
    	static int numSymbols;
    	char separator;
};

class SymbolList : private BaseSymbolList {
public:
	SymbolList(char sep='_'):BaseSymbolList(sep) {};
	const char* lookup(const char* name);
	const char* BaseSymbolList::initialize();
	void BaseSymbolList::setSeparator(char sep='_');
};

// Class for unique nested symbol generation.
class NestedSymbolList:private BaseSymbolList {
public:
	NestedSymbolList(char sep='_'):BaseSymbolList(sep) {};
	const char* push(const char* tag="L");
	const char* pop();
	const char* lookup(const char* name=NULL);
	const char* BaseSymbolList::initialize();
	void BaseSymbolList::setSeparator(char sep = '_');
};

class SymbolListList : private NameList {
public:
        // adds a new SymbolList called name to the list.  If a SymbolList
        // called name already exists in the list and it is SymbolList* is
	// different return FALSE else return TRUE
        int add(const char* name, SymbolList* slist) { 
		return NameList::add(name,slist);
        }
         
        // return the pointer for the object of the specified name.  if the
        // SymbolList does not exist, return NULL
        SymbolList* get(const char* name) {
                return (SymbolList*)NameList::get(name);
        }
         
        // remove a SymbolList from the list.  if the StringList
        // does not exist, return FALSE.
        int NameList::remove(const char* name);
};
#endif
