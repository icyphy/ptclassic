static const char file_id[] = "FlagArray.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "FlagArray.h"

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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


 Programmer:  E. A. Lee
 Date of creation: 8/15/95

 A lightweight array of integers.  Out of bounds accesses always
 return zero.  Out of bounds assignments increase the size of the array
 (except negative indices, which are an undetected error).
 FlagArray is always initialized with zero-valued elements.

**************************************************************************/

#include "logNew.h"

// Constructor with just the size specified
FlagArray :: FlagArray (int size)  {
	val = new int [nElements = size];
	int * top = &(val[nElements]);
	int * t = val;
	while (t < top)	*t++ = 0;
}
 
// Constructor with the size and initial value specified.
FlagArray :: FlagArray (int size, int fill_value) {
	LOG_NEW; val = new int [nElements = size];
	int * top = &(val[nElements]);
	int * t = val;
	while (t < top)	*t++ = fill_value;
}

// destructor
FlagArray :: ~FlagArray () {LOG_DEL; delete [] val;}

// assignment operator - copying from another FlagArray.
FlagArray& FlagArray :: operator = (const FlagArray & v) {
  if (this != &v) {
    if (v.nElements > 0) {
      if (v.nElements != nElements) {
	// Reallocate array with the new size.
	LOG_DEL; delete [] val;
	LOG_NEW; val  = new int [nElements = v.nElements];
      }
      // Copy the contents from v to this.
      int* top = &(val[nElements]);
      int* t = val;
      int* u = v.val;
      while (t < top) *t++ = *u++;
    } else {
      nElements = 0;
      val = 0;
    }
  }
  return *this;
}

// Array element for assignment or reference.
// An out of bounds reference in the positive
// direction increases the size of the array, preserving
// previous values.
int& FlagArray :: operator [] (int n)  {
  if (n >= nElements)  {
    LOG_NEW; int* newval = new int [n+1];
    int* newv = newval;
    // Copy old elements, if there are any.
    if (nElements > 0) {
      int* oldtop = &(val[nElements]);
      int* old = val;
      while (old < oldtop) *newv++ = *old++;
    }
    // Fill new elements with zero
    int* newtop = &(newval[n+1]);
    while (newv < newtop) *newv++ = 0;
    nElements = n+1;
    LOG_DEL; delete [] val;
    val = newval;
  }
  return val[n];
}
