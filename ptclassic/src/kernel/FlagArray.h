#ifndef _FlagArray_h
#define _FlagArray_h 1

#ifdef __GNUG__
#pragma interface
#endif

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

 A lightweight array of integers.  Out of bounds accesses
 increase the size of the array (except negative indices,
 which are an undetected error).  FlagArray is always
 initialized with zero-valued elements.


**************************************************************************/

///////////////////////////////////////////
// class  FlagArray
///////////////////////////////////////////

class FlagArray
{
public:
	// Constructor for empty array.
	FlagArray () {nElements = 0; val = 0;}

	// Constructor with just a size specified.
	// The array is initialized with zeros.
	FlagArray (int size);

	// Constructor, with a specified fill value.
	FlagArray (int size, int fill_value) ;

	// Destructor
	~FlagArray () ;

	// Assignment operator - copying from another FlagArray.
	FlagArray &	operator = (const FlagArray & v) ;

	// Size
	inline int size() const { return nElements;}

	// Array element for assignment or reference.
	// If necessary, this will increase the size of the array
	// and fill with zeros.
	int & operator [] (int n);
	
protected:
	int	nElements;
	int	*val;
};

#endif
