#ifndef _Code_h
#define _Code_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

*******************************************************************/

#include "DataStruct.h"
#include "type.h"

	////////////////////////////////////
	// class CodeBlock
	////////////////////////////////////

// This class should be replaced with a derived class for each specific
// code generator.  The constructor XXXCodeBlock, where XXX is the domain
// name, will normally process the input code in some way.

class CodeBlock {
public:
	// Constructor for the default case just stores the code
	CodeBlock(char* b) { text = b; }
	// Is the following needed also?
	void setText(char* line) {text = line;}
	char* getText() {return text;}
	void printCode ();
private:
	char* text;
};


	////////////////////////////////////
	// class CodeBlockList
	////////////////////////////////////

class CodeBlockList : public SequentialList {
public:
	// For many code generators, this method will parse the code lines
	void put(CodeBlock* line) {
		SequentialList::put(line);
	}

	void printCode ();
};

	////////////////////////////////////
	// class CodeBlockIter
	////////////////////////////////////

// For now, this is identical with ListIter, except for a cast
class CodeBlockIter : private ListIter {
public:
	CodeBlockIter(const CodeBlockList& c) : ListIter (c) {}
	CodeBlock* next() { return (CodeBlock*)ListIter::next();}
	CodeBlock* operator++() { return next();}
	ListIter::reset;
};

	////////////////////////////////////
	// class CodeVector
	////////////////////////////////////

// The following class stores a vector of CodeBlockList*'s.
// It is different from vector primarily in that the "elem" method
// creates new vector entries if the index is larger than the number
// of entries already allocated.

class CodeVector : public Vector {
public:
	// Add an indexed element
	const CodeBlockList* elem (int index) {
		while (dimension() <= index) {
			put(new CodeBlockList);
		}
		return(Vector::elem(index));
	}
};

#endif
