static const char file_id[] = "CGSymbol.cc";
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

#include "CGSymbol.h"
#include "miscFuncs.h"
#include "StringList.h"
#include "type.h"
#include <string.h>

Symbol::Symbol(const char* k,const char* s) {
	key = savestring(k);
	symbol = savestring(s);
}

Symbol::~Symbol() {
	LOG_DEL; delete(key);
	LOG_DEL; delete(symbol);
}

int BaseSymbolList::numSymbols = 0;

void BaseSymbolList::reset() { 
	numSymbols = 0;
}

// Lookup unique symbol, if one doesn't exist, create new symbol.
// SymbolLists are stored in pairs: key, symbol.
const char* BaseSymbolList::lookup(const char* name) {
	if (name==NULL) return NULL;
	ListIter next(*this);
	Symbol* p;
	while ((p = (Symbol*)next++) != 0)
		if(!strcmp(name,p->key)) return p->symbol;
	return NULL;
}

const char* BaseSymbolList::add(const char* name) {
	if (name==NULL) return NULL;
	StringList symbol;
	symbol << name << separator << numSymbols++;
	LOG_NEW; Symbol* newSymbol = new Symbol(name,symbol);
	tup(newSymbol);
	return newSymbol->symbol;
}

int BaseSymbolList::remove(Symbol* s) { 
	LOG_DEL; delete(s);
	return SequentialList::remove(s);
}

void BaseSymbolList::initialize() {
	ListIter next(*this);
	for (int i=size(); i > 0; i--) {
		Symbol* p = (Symbol*)next++;
		LOG_DEL; delete p;
	}           
	SequentialList::initialize();
}               

BaseSymbolList::~BaseSymbolList() {               
    ListIter next(*this);
    for (int i=size(); i > 0; i--) {           
                Symbol* p = (Symbol*)next++;
                LOG_DEL; delete p;
    }           
}               

const char* SymbolList::lookup(const char* name) {
	if (name==NULL) return NULL;
	const char* pSymbol = BaseSymbolList::lookup(name);
	if (pSymbol==NULL) pSymbol = add(name);
	return pSymbol;
}
	
const char* NestedSymbolList::push(const char* name) {
    	return add(name);
}

const char* NestedSymbolList::pop() {
	if (top() == NULL) return NULL;
	Symbol* pSymbol = top();
	StringList symbol = pSymbol->symbol;
	remove(pSymbol);	
	return symbol;
}

const char* NestedSymbolList::lookup(const char* name) {
	if (top() == NULL) return NULL;
	if (name == NULL) return top()->symbol;
	return BaseSymbolList::lookup(name);
}
