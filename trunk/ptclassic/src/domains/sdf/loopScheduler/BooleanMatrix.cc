static const char file_id[] = "BooleanMatrix.cc";
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

 Methods for class BooleanMatrix.

*******************************************************************/
#include "BooleanMatrix.h"
#include "logNew.h"

BooleanMatrix::BooleanMatrix(int m, int n) : nRows(m), nCols(n) {
	LOG_NEW; data = new BoolWord*[nRows];
	nColWords = (nCols + 31) >> 5;
	for (int i = 0; i < nRows; i++) {
		LOG_NEW; data[i] = new BoolWord[nColWords];
		for (int j = 0; j < nColWords; j++) data[i][j] = 0;
	}
}

void BooleanMatrix::clear() {
	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nColWords; j++)
			data[i][j] = 0;
}

BooleanMatrix::~BooleanMatrix() {
	for (int i = 0; i < nRows; i++) { LOG_DEL; delete data[i];}
	LOG_DEL; delete data;
}

// "or" together two rows, destRow and srcRow, leaving the result in destRow.
// we do it a word at a time, instead of a bit at a time.
void BooleanMatrix::orRows(int destRow, int srcRow) {
	BoolWord* dst = data[destRow];
	BoolWord* src = data[srcRow];

	for (int i = 0; i < nColWords; i++)
		dst[i] |= src[i];
}
