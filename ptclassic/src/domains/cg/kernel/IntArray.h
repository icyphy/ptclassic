#ifndef _IntArray_h
#define _IntArray_h
#ifdef __GNUG__
#pragma once
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

Array data structure.

*****************************************************************/

#ifdef __GNUG__
#define DEL_ARRAY(name,size) LOG_DEL; delete [size] name; delete name
#else
#define DEL_ARRAY(name,size) LOG_DEL; delete [] name
#endif

#include "type.h"

			   /////////////////
			   // class Array //
			   /////////////////

class BaseArray {
protected:
	int sz;		// array size
public:
	int empty() { return sz ? 0 : 1;}
	int size()  { return sz; }
	
	// constructor
	BaseArray() { sz = 0; }
	BaseArray(int n) { sz = n; }
};

			   ////////////////////
			   // class IntArray //
			   ////////////////////

// An array of integers

class IntArray : public BaseArray {

	// size of allocated memory
	int memSz;

protected:
	// internal data structure
	int* data;

public:
	// constructors
	IntArray(): data(0), memSz(0) {}
	IntArray(int n) { create(n); }
	~IntArray();

	// data access
	int& operator [] (int index) { return elem(index); }
	int& elem (int index);

	// initialize
	void initialize() { for (int i = 0; i < sz; i++) data[i] = 0 ;}

	// run-time creation
	void create(int n);

	// use the subset. Just specified the size to be used.
	void truncate(int n) {  if (n > memSz) create(n);
				else sz = n; }
}; 
	
#endif

