#ifndef _IntArray_h
#define _IntArray_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

Programmer: Soonhoi Ha
Date of last revision: 

Array data structure.

*****************************************************************/

#if defined(__GNUG__) && __GNUG__ == 1
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
	IntArray(int n): data(0), memSz(0) { create(n); }
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

