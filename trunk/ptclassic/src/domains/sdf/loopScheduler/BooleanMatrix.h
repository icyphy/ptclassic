/******************************************************************
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

 Programmer:  J. Buck

*******************************************************************/
#ifndef _BooleanMatrix_h
#define _BooleanMatrix_h

// This class implements a two dimensional matrix of boolean values.
// The matrix uses one bit to represent each entry. The rows and
// columns are indexed from 0.

// The matrix is stored as a 2d array.

// We explicitly assume that there at least 32 bits in an unsigned long.
// It is harmless if there are actually more than 32 bits, and
// the ANSI C standard guarantees that there are always at least 32.
// To reduce space on an architecture with more bits, you can change
// the following typedef:

typedef unsigned long BoolWord;

///////////////////////////
// class BooleanMatrix  //
//////////////////////////
class BooleanMatrix {
public:
	// Constructor with number of rows and columns as the
	// first and second argument respectively. 
	BooleanMatrix(int m, int n); 

	int rows() const { return nRows;}
	int cols() const { return nCols;}

	// Get element [m,n]. Indices start at 0. 
	int getElem(int m, int n) const {
		BoolWord w = data[m][n>>5];
		return (w >> (n & 31)) & 1;
	}

	// Set the element [m,n] to "value". 
	void setElem(int m, int n, int value) {
		BoolWord& w = data[m][n>>5];
		BoolWord mask = 1L << (n & 31);
		if (value) w |= mask;
		else w &= ~mask;
	}

	// "OR" the element [m,n] with "value"
	// (set it if value is true, otherwise do nothing)
	void orElem(int m, int n, int value) {
		if (value) setElem(m,n,1);
	}

	// "OR" together two rows, destRow and srcRow, leaving the 
	// result in destRow.
	void orRows(int destRow,int srcRow);
	// Initialize the matrix to contain all zeros.
	void clear();

	~BooleanMatrix();

private:
	// The array which stores the boolean matrix.
	BoolWord **data;

	// Number of rows in the matrix.
	int nRows;

	// Number of columns in the matrix.
	int nCols;

	// Number of chars for the column entries
	int nColWords;
};

#endif
