static const char file_id[] = "CGSymbol.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino, J. Buck

 Symbol stuff.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGSymbol.h"
#include "CGTarget.h"

Symbol::Symbol (CGTarget* target) {
	initialize();
	myTarget = target;
}
	
// Lookup unique symbol, if one doesn't exist, create new symbol.
// Symbols are stored in pairs: key, symbol.
StringList Symbol::lookup(const char* name) {
	StringListIter next(symbols);
	const char* p;
    	StringList s;
    	while ((p = next++) != 0) {
		s = next++;
		if(!strcmp(name,p)) return s;
   	}
   	// name is not on the list.  add it.
  	symbols += name;
  	s = name;
	s += myTarget->separator;
    	s += myTarget->numLabels++;
    	// add the new symbol as well
   	symbols += s;
    	return s;
}
	

NestedSymbol::NestedSymbol (CGTarget* target) {
	initialize();
	myTarget = target;
}
	
const char* NestedSymbol::push(const char* name) {
	StringList s;
   	s = name;
	s += myTarget->separator;
    	s += myTarget->numLabels++;
	const char* temp = s;
   	symbols.push(temp);
    	return savestring(temp);
}

const char* NestedSymbol::pop() {
	if(depth()==0) return 0;
	return symbols.pop();

}
	
