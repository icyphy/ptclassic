static const char file_id[] = "$RCSfile$";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino, J. Buck

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SymbolList.h"
#include "StringList.h"
#include "miscFuncs.h"

int BaseSymbolList::numSymbols = 0;

StringList BaseSymbolList::symbol(const char* string)
{
    StringList sym;
    sym << string << separator << numSymbols++;
    return sym;
}

// Put a named symbol in the list.
// Return NULL on error.
const char* BaseSymbolList::append(const char* name)
{
    char* sym = savestring(symbol(name));
    if (NamedList::append(sym, name)) return sym;
    else return NULL;
}

/*********************
TEMPORARY FIX:
Use unique names for prepend.
NamedList should allow duplicate names for prepend.
*********************/

// Put a named symbol at the head of the list.
// Return NULL on error.
const char* BaseSymbolList::prepend(const char* name)
{
    char* sym = savestring(symbol(name));
    if (NamedList::prepend(sym, sym)) return sym;
    else return NULL;
}

// Remove a named symbol from the list.
// Return FALSE on error.
int BaseSymbolList::remove(const char* name)
{ 
    char* symbol = (char*)NamedList::get(name);
    if (symbol != NULL)
    {
	LOG_DEL; delete symbol;
	return NamedList::remove(name);
    }
    else return FALSE;
}

// Delete all the symbols in the list.
void BaseSymbolList::deleteSymbols()
{
    NamedListIter symbol(*this);
    char* s;

    while((s = (char*)symbol++) != NULL)
    {
	LOG_DEL; delete s;
    }
}

const char* SymbolList::lookup(const char* name)
{
    const char* symbol = get(name);
    if (symbol == NULL) symbol = append(name);
    return symbol;
}
	
StringList SymbolStack::pop()
{
    StringList symbol = get();
    remove();
    return symbol;
}
