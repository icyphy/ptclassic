static const char file_id[] = "BooleanMatrix.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

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
