static const char file_id[] = "SubMatrix.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  Mike J. Chen
 Date of creation: 9/27/93

 This file defines the implementation of the SubMatrix classes.
 SubMatrices act as access structures to subsets of the allocated
 memory of Matrices.

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif
#include <strstream.h>
#include "SubMatrix.h"
#include "Plasma.h"
#include "Error.h"

#define SMALL_STRING 32   // should be same as for StringList.cc

/////////////////////////////////////
// Helper functions
/////////////////////////////////////

///////////////////////////////////////////////////////
// ComplexSubMatrix - holds data of type Complex
///////////////////////////////////////////////////////
// Prints matrices in standard row column form.
StringList ComplexSubMatrix::print() const {
  ostrstream strm;
  char buf[SMALL_STRING];
  strm << "ComplexSubMatrix: (";
  strm << nRows;
  strm << ",";
  strm << nCols;
  strm << ")\n";
  for(int row = 0; row < nRows; row++) {
    for(int col = 0; col < nCols; col++) {
      sprintf(buf,"%22.15g", real((*this)[row][col]));
      strm << buf;
      strm << "+";
      sprintf(buf,"%22.15g", imag((*this)[row][col]));
      strm << buf;
      strm << "j ";
    }
    strm << "\n";
  }
  strm << ends;
  return StringList(strm.str());
}

// constructor, given just the number of rows and columns.
// this allocates actual memory
ComplexSubMatrix::ComplexSubMatrix(int nRow, int nCol) {
  LOG_NEW; parent = new ComplexMatrix(nRow,nCol);
  nRows = nRow;
  nCols = nCol;
  totalDataSize = parent->totalDataSize;
  data = &((*parent)[0][0]);
}

// Constructor.  Takes a parent ComplexMatrix (accessed from a sibling
// SubMatrix), the starting row and col (relative) to the parent matrix,
// and the dimensions of this SubMatrix. 
// Error if the dimensions of the SubMatrix go beyond the dimensions of
// parent.
ComplexSubMatrix::ComplexSubMatrix(ComplexSubMatrix& src,
                                   int sRow, int sCol, int nRow, int nCol){
  parent = src.parent;
  if(sRow + nRow > parent->numRows()) {
    Error::abortRun("submatrix rows extend beyond dimensions of src matrix\n");
    nRows = parent->numRows() - sRow;
  }
  else nRows = nRow;
  if(sCol + nCol > parent->numCols()) {
    Error::abortRun("submatrix cols extend beyond dimensions of src matrix\n");
    nCols = parent->numCols() - sCol;
  }
  else nCols = nCol;
  totalDataSize = nRows * nCols;
  data = &(src[sRow][sCol]);
}

// Copy Constructor, makes a duplicate of src ComplexSubMatrix
ComplexSubMatrix::ComplexSubMatrix(const ComplexSubMatrix& src) {
  nRows = src.nRows;
  nCols = src.nCols;
  totalDataSize = src.totalDataSize;
  data = src.data;
  parent = src.parent;
}

// General operators
int ComplexSubMatrix::operator == (const PtMatrix& m) {
  if(typesEqual(m)) {
    const ComplexSubMatrix& src = *((const ComplexSubMatrix*)&m);
    if((parent == src.parent) && (data = src.data) &&
       (nRows == src.nRows) && (nCols == src.nCols))
      return 1;
  }
  return 0;
}

PtMatrix& ComplexSubMatrix::operator = (const PtMatrix& src) {
  if(dataType() != src.dataType() && parent->dataType() != src.dataType())
    Error::abortRun("improper type in attempt to copy to ComplexSubMatrix");
  else {
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = ((const ComplexSubMatrix*)&src)->entry(i);
  }
  return *this;
}

// gcc-2.6.x requires this.  See the docs for -Wsynth for more info
ComplexSubMatrix& ComplexSubMatrix::operator = (const ComplexSubMatrix& src) {
  if(dataType() != src.dataType() && parent->dataType() != src.dataType())
    Error::abortRun("improper type in attempt to copy to ComplexSubMatrix");
  else {
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = ((const ComplexSubMatrix*)&src)->entry(i);
  }
  return *this;
}


ComplexSubMatrix& ComplexSubMatrix::operator = (Complex value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

// If the ComplexArrayState has more members than we need, we simply
// ignore the extra elements.
void ComplexSubMatrix::operator << (ComplexArrayState& src) {
  if(totalDataSize > src.size())
    Error::abortRun("not enough data in ComplexArrayState to init SubMatrix");
  else {
    for(int row = 0; row < nRows; row++)
      for(int col = 0; col < nCols; col++)
        (*this)[row][col] = src[(row*nCols + col)];
  }
}

////////////////////////////////////////////////
// FixSubMatrix class - holds data of type Fix
////////////////////////////////////////////////
// Prints matrices in standard row column form.
StringList FixSubMatrix::print() const {
  ostrstream strm;
  char buf[SMALL_STRING];
  strm << "FixSubMatrix: (";
  strm << nRows;
  strm << ",";
  strm << nCols;
  strm << ")\n";
  for(int row = 0; row < nRows; row++) {
    for(int col = 0; col < nCols; col++) {
      sprintf(buf,"%22.15g", (double)(*this)[row][col]);
      strm << buf;
      strm << " ";
    }
    strm << "\n";
  }
  strm << ends;
  return StringList(strm.str());
}

// constructor, given just the number of rows and columns.
// this allocates actual memory
FixSubMatrix::FixSubMatrix(int nRow, int nCol) {
  LOG_NEW; parent = new FixMatrix(nRow,nCol);
  nRows = nRow;
  nCols = nCol;
  totalDataSize = parent->totalDataSize;
  data = &((*parent)[0][0]);
}

// Constructor.  Takes a parent FixMatrix (accessed from a sibling
// SubMatrix), the starting row and col (relative) to the parent matrix,
// and the dimensions of this SubMatrix. 
// Error if the dimensions of the SubMatrix go beyond the dimensions of
// parent.
FixSubMatrix::FixSubMatrix(FixSubMatrix& src,
                           int sRow, int sCol, int nRow, int nCol){
  parent = src.parent;
  if(sRow + nRow > parent->numRows()) {
    Error::abortRun("submatrix rows extend beyond dimensions of src matrix\n");
    nRows = parent->numRows() - sRow;
  }
  else nRows = nRow;
  if(sCol + nCol > parent->numCols()) {
    Error::abortRun("submatrix cols extend beyond dimensions of src matrix\n");
    nCols = parent->numCols() - sCol;
  }
  else nCols = nCol;
  totalDataSize = nRows * nCols;
  data = &(src[sRow][sCol]);
}

// Copy Constructor, makes a duplicate of src FixSubMatrix
FixSubMatrix::FixSubMatrix(const FixSubMatrix& src) {
  nRows = src.nRows;
  nCols = src.nCols;
  totalDataSize = src.totalDataSize;
  data = src.data;
  parent = src.parent;
}

// General operators
int FixSubMatrix::operator == (const PtMatrix& m) {
  if(typesEqual(m)) {
    const FixSubMatrix& src = *((const FixSubMatrix*)&m);
    if((parent == src.parent) && (data = src.data) &&
       (nRows == src.nRows) && (nCols == src.nCols))
      return 1;
  }
  return 0;
}

PtMatrix& FixSubMatrix::operator = (const PtMatrix& src) {
  if(dataType() != src.dataType() && parent->dataType() != src.dataType())
    Error::abortRun("improper type in attempt to copy to FixSubMatrix");
  else {
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = ((const FixSubMatrix*)&src)->entry(i);
  }
  return *this;
}

// gcc-2.6.x requires this.  See the docs for -Wsynth for more info
FixSubMatrix& FixSubMatrix::operator = (const FixSubMatrix& src) {
  if(dataType() != src.dataType() && parent->dataType() != src.dataType())
    Error::abortRun("improper type in attempt to copy to FixSubMatrix");
  else {
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = ((const FixSubMatrix*)&src)->entry(i);
  }
  return *this;
}


FixSubMatrix& FixSubMatrix::operator = (Fix value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

// If the FixArrayState has more members than we need, we simply
// ignore the extra elements.
void FixSubMatrix::operator << (FixArrayState& src) {
  if(totalDataSize > src.size())
    Error::abortRun("not enough data in FixArrayState to init SubMatrix");
  else {
    for(int row = 0; row < nRows; row++)
      for(int col = 0; col < nCols; col++)
        (*this)[row][col] = src[(row*nCols + col)];
  }
}

///////////////////////////////////////////////////////
// FloatSubMatrix class - holds data of type double
///////////////////////////////////////////////////////
// Prints matrices in standard row- column form.
StringList FloatSubMatrix::print() const {
  ostrstream strm;
  char buf[SMALL_STRING];
  strm << "FloatSubMatrix: (";
  strm << nRows;
  strm << ",";
  strm << nCols;
  strm << ")\n";
  for(int row = 0; row < nRows; row++) {
    for(int col = 0; col < nCols; col++) {
      sprintf(buf,"%22.15g", (*this)[row][col]);
      strm << buf;
      strm << " ";
    }
    strm << "\n";
  }
  strm << ends;
  return StringList(strm.str());
}

// constructor, given just the number of rows and columns.
// this allocates actual memory
FloatSubMatrix::FloatSubMatrix(int nRow, int nCol) {
  LOG_NEW; parent = new FloatMatrix(nRow,nCol);
  nRows = nRow;
  nCols = nCol;
  totalDataSize = parent->totalDataSize;
  data = &((*parent)[0][0]);
}

// Constructor.  Takes a parent FloatMatrix (accessed from a sibling
// SubMatrix), the starting row and col (relative) to the parent matrix,
// and the dimensions of this SubMatrix. 
// Error if the dimensions of the SubMatrix go beyond the dimensions of
// parent.
FloatSubMatrix::FloatSubMatrix(FloatSubMatrix& src,
                               int sRow, int sCol, int nRow, int nCol){
  parent = src.parent;
  if(sRow + nRow > parent->numRows()) {
    Error::abortRun("submatrix rows extend beyond dimensions of src matrix\n");
    nRows = parent->numRows() - sRow;
  }
  else nRows = nRow;
  if(sCol + nCol > parent->numCols()) {
    Error::abortRun("submatrix cols extend beyond dimensions of src matrix\n");
    nCols = parent->numCols() - sCol;
  }
  else nCols = nCol;
  totalDataSize = nRows * nCols;
  data = &(src[sRow][sCol]);
}

// Copy Constructor, makes a duplicate of src FloatSubMatrix
FloatSubMatrix::FloatSubMatrix(const FloatSubMatrix& src) {
  nRows = src.nRows;
  nCols = src.nCols;
  totalDataSize = src.totalDataSize;
  data = src.data;
  parent = src.parent;
}

// General operators
int FloatSubMatrix::operator == (const PtMatrix& m) {
  if(typesEqual(m)) {
    const FloatSubMatrix& src = *((const FloatSubMatrix*)&m);
    if((parent == src.parent) && (data = src.data) &&
       (nRows == src.nRows) && (nCols == src.nCols))
      return 1;
  }
  return 0;
}

PtMatrix& FloatSubMatrix::operator = (const PtMatrix& src) {
  if(dataType() != src.dataType() && parent->dataType() != src.dataType())
      Error::abortRun("improper type in attempt to copy to FloatSubMatrix");
  else {
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = ((const FloatSubMatrix*)&src)->entry(i);
  }
  return *this;
}

// gcc-2.6.x requires this.  See the docs for -Wsynth for more info
FloatSubMatrix& FloatSubMatrix::operator = (const FloatSubMatrix& src) {
  if(dataType() != src.dataType() && parent->dataType() != src.dataType())
      Error::abortRun("improper type in attempt to copy to FloatSubMatrix");
  else {
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = ((const FloatSubMatrix*)&src)->entry(i);
  }
  return *this;
}

FloatSubMatrix& FloatSubMatrix::operator = (double value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

// If the FloatArrayState has more members than we need, we simply
// ignore the extra elements.
void FloatSubMatrix::operator << (FloatArrayState& src) {
  if(totalDataSize > src.size())
    Error::abortRun("not enough data in FloatArrayState to init SubMatrix");
  else {
    for(int row = 0; row < nRows; row++)
      for(int col = 0; col < nCols; col++) {
        (*this)[row][col] = src[(row*nCols + col)];
      }
  }
}

/////////////////////////////////////////////////
// IntSubMatrix class - holds data of type int
/////////////////////////////////////////////////
// Prints matrices in standard row-column form.
StringList IntSubMatrix::print() const {
  ostrstream strm;
  char buf[SMALL_STRING];
  strm << "IntSubMatrix: (";
  strm << nRows;
  strm << ",";
  strm << nCols;
  strm << ")\n";
  for(int row = 0; row < nRows; row++) {
    for(int col = 0; col < nCols; col++) {
      sprintf(buf,"%12d", (*this)[row][col]);
      strm << buf;
      strm << " ";
    }
    strm << "\n";
  }
  strm << ends;
  return StringList(strm.str());
}

// constructor, given just the number of rows and columns.
// this allocates actual memory
IntSubMatrix::IntSubMatrix(int nRow, int nCol) {
  LOG_NEW; parent = new IntMatrix(nRow,nCol);
  nRows = nRow;
  nCols = nCol;
  totalDataSize = parent->totalDataSize;
  data = &((*parent)[0][0]);
}

// Constructor.  Takes a parent IntMatrix (accessed from a sibling
// SubMatrix), the starting row and col (relative) to the parent matrix,
// and the dimensions of this SubMatrix. 
// Error if the dimensions of the SubMatrix go beyond the dimensions of
// parent.
IntSubMatrix::IntSubMatrix(IntSubMatrix& src,
                           int sRow, int sCol, int nRow, int nCol){
  parent = src.parent;
  if(sRow + nRow > parent->numRows()) {
    Error::abortRun("submatrix rows extend beyond dimensions of src matrix\n");
    nRows = parent->numRows() - sRow;
  }
  else nRows = nRow;
  if(sCol + nCol > parent->numCols()) {
    Error::abortRun("submatrix cols extend beyond dimensions of src matrix\n");
    nCols = parent->numCols() - sCol;
  }
  else nCols = nCol;
  totalDataSize = nRows * nCols;
  data = &(src[sRow][sCol]);
}

// Copy Constructor, makes a duplicate of src IntSubMatrix
IntSubMatrix::IntSubMatrix(const IntSubMatrix& src) {
  nRows = src.nRows;
  nCols = src.nCols;
  totalDataSize = src.totalDataSize;
  data = src.data;
  parent = src.parent;
}

// General operators
int IntSubMatrix::operator == (const PtMatrix& m) {
  if(typesEqual(m)) {
    const IntSubMatrix& src = *((const IntSubMatrix*)&m);
    if((parent == src.parent) && (data = src.data) &&
       (nRows == src.nRows) && (nCols == src.nCols))
      return 1;
  }
  return 0;
}

PtMatrix& IntSubMatrix::operator = (const PtMatrix& src) {
  if(dataType() != src.dataType() && parent->dataType() != src.dataType())
    Error::abortRun("improper type in attempt to copy to IntSubMatrix");
  else {
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = ((const IntSubMatrix*)&src)->entry(i);
  }
  return *this;
}

// gcc-2.6.x requires this.  See the docs for -Wsynth for more info
IntSubMatrix& IntSubMatrix::operator = (const IntSubMatrix& src) {
  if(dataType() != src.dataType() && parent->dataType() != src.dataType())
    Error::abortRun("improper type in attempt to copy to IntSubMatrix");
  else {
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = ((const IntSubMatrix*)&src)->entry(i);
  }
  return *this;
}

IntSubMatrix& IntSubMatrix::operator = (int value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

// If the IntArrayState has more members than we need, we simply
// ignore the extra elements.
void IntSubMatrix::operator << (IntArrayState& src) {
  if(totalDataSize > src.size())
    Error::abortRun("not enough data in IntArrayState to init SubMatrix");
  else {
    for(int row = 0; row < nRows; row++)
      for(int col = 0; col < nCols; col++)
        (*this)[row][col] = src[(row*nCols + col)];
  }
}

