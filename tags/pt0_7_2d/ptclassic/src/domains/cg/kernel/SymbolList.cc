static const char file_id[] = "SymbolList.cc";
/******************************************************************
Version identification:
@(#)SymbolList.cc	1.13	10/6/95

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

    StringList temp = symbol(name);
    char* sym = savestring(temp);
    if (NamedList::append(sym, name)) {
      return sym;
    }
    else {
      delete [] sym;
      return NULL;
    }
}

// Put a named symbol at the head of the list.
// Return NULL on error.
const char* BaseSymbolList::prepend(const char* name)
{
    if (counter == NULL) return NULL;

    StringList temp = symbol(name);
    char* sym = savestring(temp);
    NamedList::prepend(sym, name);
    return sym;
}

// Remove a named symbol from the list.
// Return FALSE on error.
int BaseSymbolList::remove(const char* name)
{ 
    char* sym = (char*)NamedList::get(name);
    if (sym != NULL)
    {
	LOG_DEL; delete [] sym;
	return NamedList::remove(name);
    }
    else return FALSE;
}

// Delete all the symbols in the list.
void BaseSymbolList::deleteSymbols()
{
    NamedListIter sym(*this);
    char* s;

    while((s = (char*)sym++) != NULL)
    {
	LOG_DEL; delete [] s;
    }
}

const char* SymbolList::lookup(const char* name)
{
    const char* sym = get(name);
    if (sym == NULL) sym = append(name);
    return sym;
}
	
StringList SymbolStack::pop()
{
    StringList sym = get();
    remove();
    return sym;
}

const char* ScopedSymbolList::lookup(const char* scope, const char* name)
{
    SymbolList* list = (SymbolList*)get(scope);
    if (list == NULL)
    {
	LOG_NEW; list = new SymbolList(separator, counter);
	append(list, scope);
    }
    const char* sym = list->lookup(name);
    return sym;
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
