static const char file_id[] = "IntArray.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "IntArray.h"
#include "Error.h"
#include "StringList.h"

// dynamic creation of an integer array

void IntArray::create(int n) {
	
	if (n > memSz) {
		LOG_DEL; delete data;
		LOG_NEW; data = new int[n];
		memSz = n;
	}
	sz = n;
	initialize();		// initialize the array
}

int& IntArray :: elem (int index) {
	if (index >= memSz || index < 0) {
		StringList out;
		out += "index ";
		out += index;
		out += " is out of range.";
		Error :: abortRun (out);
		return data[0];
	} 
	return data[index];
}

// destructor
IntArray::~IntArray() { 
	LOG_DEL; delete data;
	data = 0;
}
