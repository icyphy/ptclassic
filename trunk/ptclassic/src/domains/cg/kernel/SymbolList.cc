static const char file_id[] = "$RCSfile$";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino, J. Buck, T. M. Parks

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SymbolList.h"
#include "StringList.h"
#include "miscFuncs.h"

StringList BaseSymbolList::symbol(const char* string)
{
    StringList sym;
    sym << string << separator << (*counter)++;
    return sym;
}

// Put a named symbol in the list.
// Return NULL on error.
const char* BaseSymbolList::append(const char* name)
{
    if (counter == NULL) return NULL;

    char* sym = savestring(symbol(name));
    if (NamedList::append(sym, name)) return sym;
    else return NULL;
}

// Put a named symbol at the head of the list.
// Return NULL on error.
const char* BaseSymbolList::prepend(const char* name)
{
    if (counter == NULL) return NULL;

    char* sym = savestring(symbol(name));
    NamedList::prepend(sym, name);
    return sym;
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

const char* ScopedSymbolList::lookup(const char* scope, const char* name)
{
    SymbolList* list = get(scope);
    if (list == NULL)
    {
	LOG_NEW; list = new SymbolList(separator, counter);
	append(list, scope);
    }
    const char* symbol = list->lookup(name);
    return symbol;
}

// Delete all the SymbolLists in the list.
void ScopedSymbolList::deleteSymbolLists()
{
    NamedListIter list(*this);
    SymbolList* l;

    while((l = (SymbolList*)list++) != NULL)
    {
	LOG_DEL; delete l;
    }
}

void ScopedSymbolList::setSeparator(char sep)
{
    separator = sep;
    NamedListIter list(*this);
    SymbolList* l;

    while((l = (SymbolList*)list++) != NULL)
    {
	l->setSeparator(separator);
    }
}

void ScopedSymbolList::setCounter(int *count)
{
    counter = count;
    NamedListIter list(*this);
    SymbolList* l;

    while((l = (SymbolList*)list++) != NULL)
    {
	l->setCounter(counter);
    }
}
