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
	// class LineOfCode
	////////////////////////////////////

// This class should be replaced with a derived class for each specific
// code generator.  The method XXXCGStar::codeLine() should create
// an instance of this class, and store it in the codeBlock.

class LineOfCode {
public:
	LineOfCode(char* line) { text = line; }
	// Is the following needed also?
	void setText(char* line) {text = line;}
	char* getText() {return text;}
private:
	char* text;
};


	////////////////////////////////////
	// class BlockOfCode
	////////////////////////////////////

class BlockOfCode : public SequentialList {
public:
	// For many code generators, this method will parse the code lines
	void put(LineOfCode* line) {
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
	CodeBlockIter(const BlockOfCode& c) : ListIter (c) {}
	LineOfCode* next() { return (LineOfCode*)ListIter::next();}
	LineOfCode* operator++() { return next();}
	ListIter::reset;
};

	////////////////////////////////////
	// class CodeVector
	////////////////////////////////////

// The following class stores a vector of BlockOfCode*'s.
// It is different from vector primarily in that the "elem" method
// creates new vector entries if the index is larger than the number
// of entries already allocated.

class CodeVector : public Vector {
public:
	// Add an indexed element
	const BlockOfCode* elem (int index) {
		while (dimension() <= index) {
			put(new BlockOfCode);
		}
		return(Vector::elem(index));
	}
};

#endif
