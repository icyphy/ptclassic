static const char file_id[] = "Matrix.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1993 The Regents of the University of California.
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

 Programmer:  Mike J. Chen
 Date of creation: 9/27/93

 This file defines the implementation of the Matrix Message classes and
 the Matrix Envelope Particles that hold them.

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Matrix.h"
#include "Plasma.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
// Helper functions - operators between 2 Matrices.
/////////////////////////////////////////////////////////////////////////////

ComplexMatrix operator+ (const ComplexMatrix& src1,const ComplexMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("addition used on matrices with incompatible dimensions.\n"); 
    return src1;
  }
  ComplexMatrix result(src1);   // Make a copy of src1
  result += src2;
  return result;
}

ComplexMatrix operator- (const ComplexMatrix& src1,const ComplexMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("subtraction used on matrices with incompatible dimensions.\n"); 
    return src1;
  }
  ComplexMatrix result(src1);   // Make a copy of src1
  result -= src2;
  return result;
}

ComplexMatrix operator* (const ComplexMatrix& src1,const ComplexMatrix& src2) {
  ComplexMatrix result(src1.nRows, src2.nCols);

  if(src1.nCols != src2.nRows) {
    Error::abortRun("multiplication used on matrices with incompatible dimensions.\n");
    return result;
  }
  
  result = 0;
  for(int i = 0; i < src1.nRows; i++)
    for(int j = 0; j < src2.nCols; j++)
      for(int k = 0; k < src1.nCols; k++)
        result[i][j] += src1[i][k] * src2[k][j];
  return result;
}

FixMatrix operator + (const FixMatrix& src1, const FixMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("addition used on matrices with incompatible dimensions.\n"); 
    return src1;
  }
  FixMatrix result(src1);   // Make a copy of src1
  result += src2;
  return result;
}

FixMatrix operator - (const FixMatrix& src1, const FixMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("subtraction used on matrices with incompatible dimensions.\n"); 
    return src1;
  }
  FixMatrix result(src1);   // Make a copy of src1
  result -= src2;
  return result;
}

FixMatrix operator * (const FixMatrix& src1, const FixMatrix& src2) {
  FixMatrix result(src1.nRows, src2.nCols);

  if(src1.nCols != src2.nRows) {
    Error::abortRun("multiplication used on matrices with incompatible dimensions.\n"); 
    return result;
  }
  
  result = Fix(double(0));

  for(int i = 0; i < src1.nRows; i++)
    for(int j = 0; j < src2.nCols; j++)
      for(int k = 0; k < src1.nCols; k++)
        result[i][j] += src1[i][k] * src2[k][j];
  return result;
}

FloatMatrix operator + (const FloatMatrix& src1, const FloatMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("addition used on matrices with incompatible dimensions.\n"); 
    return src1;
  }
  FloatMatrix result(src1);   // Make a copy of src1
  result += src2;
  return result;
}

FloatMatrix operator - (const FloatMatrix& src1, const FloatMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("subtraction used on matrices with incompatible dimensions.\n"); 
    return src1;
  }
  FloatMatrix result(src1);   // Make a copy of src1
  result -= src2;
  return result;
}

FloatMatrix operator * (const FloatMatrix& src1, const FloatMatrix& src2) {
  FloatMatrix result(src1.nRows, src2.nCols);

  if(src1.nCols != src2.nRows) {
    Error::abortRun("multiplication used on matrices with incompatible dimensions.\n");
    return result;
  }
  
  result = 0;
  for(int i = 0; i < src1.nRows; i++)
    for(int j = 0; j < src2.nCols; j++)
      for(int k = 0; k < src1.nCols; k++)
        result[i][j] += src1[i][k] * src2[k][j];
  return result;
}

IntMatrix operator + (const IntMatrix& src1, const IntMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("addition used on matrices with incompatible dimensions.\n"); 
    return src1;
  }
  IntMatrix result(src1);   // Make a copy of src1
  result += src2;
  return result;
}

IntMatrix operator - (const IntMatrix& src1, const IntMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("subtraction used on matrices with incompatible dimensions.\n"); 
    return src1;
  }
  IntMatrix result(src1);   // Make a copy of src1
  result -= src2;
  return result;
}

IntMatrix operator * (const IntMatrix& src1, const IntMatrix& src2) {
  IntMatrix result(src1.nRows, src2.nCols);

  if(src1.nCols != src2.nRows) {
    Error::abortRun("multiplication used on matrices with incompatible dimensions.\n"); 
    return result;
  }
  
  result = 0;
  for(int i = 0; i < src1.nRows; i++)
    for(int j = 0; j < src2.nCols; j++)
      for(int k = 0; k < src1.nCols; k++)
        result[i][j] += src1[i][k] * src2[k][j];
  return result;
}

/////////////////////////////////////////////////////////////
// Complex Matrix Message Classe - holds data of type Complex
/////////////////////////////////////////////////////////////

// Constructor: make an unitialized matrix with no data
ComplexMatrix::ComplexMatrix() {
  nRows = nCols = totalDataSize = 0;
}

// Constructor: make an unitialized matrix with the given dimensions
ComplexMatrix::ComplexMatrix(int numRow, int numCol) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Complex[totalDataSize];
}

// constructor:
// initialized with the data given in the Particles of the PortHole
ComplexMatrix::ComplexMatrix(int numRow, int numCol, PortHole& ph) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Complex[totalDataSize];

  // Load the data from the PortHole into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = Complex(ph%(totalDataSize - i - 1));
}

// constructor:
// initialized with the data given in a dataArray ComplexArrayState
ComplexMatrix::ComplexMatrix(int numRow, int numCol, ComplexArrayState& dataArray) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;

  if(dataArray.size() < totalDataSize) {
    Error::abortRun ("not enough arguments in ComplexArrayState input",
                     " to the constructor\n");
    return;
  }
  LOG_NEW; data = new Complex[totalDataSize];

  // Load the data from the dataArray ComplexArrayState into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = dataArray[i];
}

// Copy Constructor
ComplexMatrix::ComplexMatrix(const ComplexMatrix& src) {
  nRows = src.nRows;
  nCols = src.nCols;
  totalDataSize = src.totalDataSize;
  LOG_NEW; data = new Complex[totalDataSize];

  for(int i = 0; i < totalDataSize; i++)
    entry(i) = src.entry(i);
}

// Copy Constructor, copying only a submatrix of the original.  Needs
// the starting row and col to copy from the original, as well as the
// dimensions of the submatrix.  Undefined if the dimensions of the submatrix
// are go beyond the dimensions of the original matrix.
ComplexMatrix::ComplexMatrix(const ComplexMatrix& src, int startRow, int startCol, int numRow, int numCol) {
  if(startRow + numRow > src.nRows) {
    Error::abortRun("submatrix rows extend beyond dimensions of src matrix\n");
    nRows = src.nRows - startRow;
  }
  else nRows = numRow;
  if(startCol + numCol > src.nCols) {
    Error::abortRun("submatrix cols extend beyond dimensions of src matrix\n");
    nCols = src.nCols - startCol;
  }
  else nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Complex[totalDataSize];

  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nCols; col++)
      (*this)[row][col] = src[(startRow + row - 1)][(startCol + col - 1)];
}

// Prints matrices in standard row column form.
StringList ComplexMatrix::print() const {
  StringList out = "ComplexMatrix:\n";
  for(int row = 0; row < nRows; row++) {
    for(int col = 0; col < nCols; col++) {
      out += real((*this)[row][col]);
      out += "+";
      out += imag((*this)[row][col]);
      out += "j ";
    }
    out += "\n";
  }
  return out;
}

int ComplexMatrix::operator == (const ComplexMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols))
    return 0;
  for(int i = 0; i < totalDataSize; i++)
    if(entry(i) != src.entry(i))
      return 0;
  return 1;
}

// cast conversion operators
ComplexMatrix::operator FixMatrix () const {
  FixMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = Fix(abs(entry(i)));
  return result;
}

ComplexMatrix::operator FloatMatrix () const {
  FloatMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = abs(entry(i));
  return result;
}

ComplexMatrix::operator IntMatrix () const {
  IntMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = (int)abs(entry(i));
  return result;
}

// destructive manipulations
ComplexMatrix& ComplexMatrix::operator = (const ComplexMatrix& src) {
// WARNING: any SubMatrices refering to the data in this matrix 
// will become invalid if this matrix's storage is reallocated.
  if(totalDataSize != src.totalDataSize) {
    LOG_DEL; delete[] data;
    totalDataSize = src.totalDataSize;
    data = new Complex[totalDataSize];
  }
  nRows = src.nRows;
  nCols = src.nCols;
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = src.entry(i);
  return *this;
}

ComplexMatrix& ComplexMatrix::operator = (Complex value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

ComplexMatrix& ComplexMatrix::operator += (const ComplexMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("+= used on matrices of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += src.entry(i);
  return *this;
}

ComplexMatrix& ComplexMatrix::operator += (Complex value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += value;
  return *this;
}

ComplexMatrix& ComplexMatrix::operator -= (const ComplexMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("-= used on matrices of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= src.entry(i);
  return *this;
}

ComplexMatrix& ComplexMatrix::operator -= (Complex value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= value;
  return *this;
}

// non-destructive manipulations
ComplexMatrix ComplexMatrix::transpose() const {
  ComplexMatrix result(nCols,nRows);
  for(int i = 0; i < nCols; i++)
    for(int j = 0; j < nRows; j++)
      result[i][j] = (*this)[j][i];

  return result;
}

// Hermitian transpose
ComplexMatrix ComplexMatrix::hermitian() const {
  ComplexMatrix result(nCols,nRows);
  for(int i = 0; i < nCols; i++)
    for(int j = 0; j < nRows; j++)
      result[i][j] = conj((*this)[j][i]);

  return result;
}

ComplexMatrix ComplexMatrix::inverse() const {
  ComplexMatrix result(nRows,nRows);
  if(nRows != nCols) {
    Error::abortRun("attempt to invert a matrix that is not square.\n");
    return result;
  }

  ComplexMatrix work(*this);                  // make a working copy of this
  Complex temp;

  // set result to be the identity matrix
  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nRows; col++)
      if(col == row) result[row][col] = 1;
      else result[row][col] = 0;

  for(int i = 0; i < nRows; i++) {
    // check that the element in (i,i) is not zero
    if(work[i][i] == 0) {
      // swap with a row below this one that has a non-zero element
      // in the same column
      for(row = i+1; row < nRows; row++)
        if(work[row][i] != 0)
          break;
      if(row == nRows) {
        Error::abortRun("couldn't invert matrix, possibly singular.\n");
        return result;
      }
      // swap rows
      for(col = 0; col < nRows; col++) {
        temp = work[i][col];
        work[i][col] = work[row][col];
        work[row][col] = temp;
        temp = result[i][col];
        result[i][col] = result[row][col];
        result[row][col] = temp;
      }
    }
    // divide every element in the row by element (i,i)
    temp = work[i][i];
    for(col = 0; col < nRows; col++) {
      work[i][col] /= temp;
      result[i][col] /= temp;
    }
    // zero out the rest of column i
    for(row = 0; row < nRows; row++) {
      if(row != i) {
        temp = work[row][i];
        for(col = nRows - 1; col >= 0; col--) {
          work[row][col] -= (temp * work[i][col]);
          result[row][col] -= (temp * result[i][col]);
        }
      }
    }
  }
  return result;
}

ComplexMatrix::~ComplexMatrix() {
  if(data) {
    LOG_DEL; delete[] data; 
  }
}

///////////////////////////////////////////////////
// FixMatrix Message Class - holds data of type Fix
///////////////////////////////////////////////////

// Constructor: make an unitialized matrix with no data
FixMatrix::FixMatrix() {
  nRows = nCols = totalDataSize = 0;
}

// Constructor: make an unitialized matrix with the given dimensions
FixMatrix::FixMatrix(int numRow, int numCol) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];
}

// constructor:
// initialized with the data given in the Particles of the PortHole
FixMatrix::FixMatrix(int numRow, int numCol, PortHole& ph) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];

  // Load the data from the PortHole into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    data[i] = Fix(ph%(totalDataSize - i - 1));
}

// constructor:
// initialized with the data given in a dataArray FixArrayState
FixMatrix::FixMatrix(int numRow, int numCol, FixArrayState& dataArray)
{
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;

  if(dataArray.size() < totalDataSize) {
    Error::abortRun ("not enough arguments in FixArrayState input",
                     " to the constructor\n");
    return;
  }
  LOG_NEW; data = new Fix[totalDataSize];

  // Load the data from the dataArray FixArrayState into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    data[i] = dataArray[i];
}

// Copy Constructor
FixMatrix::FixMatrix(const FixMatrix& src) {
  nRows = src.nRows;
  nCols = src.nCols;
  totalDataSize = src.totalDataSize;
  LOG_NEW; data = new Fix[totalDataSize];

  for(int i = 0; i < totalDataSize; i++)
    data[i] = src.data[i];
}

// Copy Constructor, copying only a submatrix of the original.  Needs
// the starting row and col to copy from the original, as well as the
// dimensions of the submatrix.  Undefined if the dimensions of the submatrix
// are go beyond the dimensions of the original matrix.
FixMatrix::FixMatrix(const FixMatrix& src, int startRow, int startCol, int numRow, int numCol) {
  if(startRow + numRow > src.nRows) {
    Error::abortRun("submatrix rows extend beyond dimensions of src matrix\n");
    nRows = src.nRows - startRow;
  }
  else nRows = numRow;
  if(startCol + numCol > src.nCols) {
    Error::abortRun("submatrix cols extend beyond dimensions of src matrix\n");
    nCols = src.nCols - startCol;
  }
  else nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];

  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nCols; col++)
      (*this)[row][col] = src[(startRow + row - 1)][(startCol + col - 1)];
}

// Prints matrices in standard row column form.
StringList FixMatrix::print() const {
  StringList out = "FixMatrix:\n";
  for(int row = 0; row < nRows; row++) {
    for(int col = 0; col < nCols; col++) {
      out += (*this)[row][col];
      out += " ";
    }
    out += "\n";
  }
  return out;
}

int FixMatrix::operator == (const FixMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols))
    return 0;
  for(int i = 0; i < totalDataSize; i++)
    if(entry(i) != src.entry(i))
      return 0;
  return 1;
}

// cast conversion operators
FixMatrix::operator ComplexMatrix () const {
  ComplexMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = double(entry(i));
  return result;
}

FixMatrix::operator FloatMatrix () const {
  FloatMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = double(entry(i));
  return result;
}

FixMatrix::operator IntMatrix () const {
  IntMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = int(entry(i));
  return result;
}

// destructive manipulations
FixMatrix& FixMatrix::operator = (const FixMatrix& src) {
// WARNING: any SubMatrices refering to the data in this matrix 
// will become invalid if this matrix's storage is reallocated.
  if(totalDataSize != src.totalDataSize) {
    LOG_DEL; delete[] data;
    totalDataSize = src.totalDataSize;
    data = new Fix[totalDataSize];
  }
  nRows = src.nRows;
  nCols = src.nCols;
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = src.entry(i);
  return *this;
}

FixMatrix& FixMatrix::operator = (Fix value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

FixMatrix& FixMatrix::operator += (const FixMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("+= used on matrices of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += src.entry(i);
  return *this;
}

FixMatrix& FixMatrix::operator += (Fix value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += value;
  return *this;
}

FixMatrix& FixMatrix::operator -= (const FixMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("-= used on matrices of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= src.entry(i);
  return *this;
}

FixMatrix& FixMatrix::operator -= (Fix value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= value;
  return *this;
}

// non-destructive manipulations
FixMatrix FixMatrix::transpose() const {
  FixMatrix result(nCols,nRows);
  for(int i = 0; i < nCols; i++)
    for(int j = 0; j < nRows; j++)
      result[i][j] = (*this)[j][i];

  return result;
}

FixMatrix FixMatrix::inverse() const {
  FixMatrix result(nRows,nRows);
  if(nRows != nCols) {
    Error::abortRun("attempt to invert a matrix that is not square.\n");
    return result;
  }
  FixMatrix work(*this);                  // make a working copy of this
  Fix temp;

  // set result to be the identity matrix
  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nRows; col++)
      if(col == row) result[row][col] = Fix(double(1)); // make a one
      else result[row][col] = Fix(double(0));          // make a zero

  for(int i = 0; i < nRows; i++) {
    // check that the element in (i,i) is not zero
    if(work[i][i] == Fix(double(0))) {
      // swap with a row below this one that has a non-zero element
      // in the same column
      for(row = i+1; row < nRows; row++)
        if(work[row][i] != Fix(double(0)))
          break;
      if(row == nRows) {
        Error::abortRun("couldn't invert matrix, possibly singular.\n");
        return result;
      }
      // swap rows
      for(col = 0; col < nRows; col++) {
        temp = work[i][col];
        work[i][col] = work[row][col];
        work[row][col] = temp;
        temp = result[i][col];
        result[i][col] = result[row][col];
        result[row][col] = temp;
      }
    }
    // divide every element in the row by element (i,i)
    temp = work[i][i];
    for(col = 0; col < nRows; col++) {
      work[i][col] /= temp;
      result[i][col] /= temp;
    }
    // zero out the rest of column i
    for(row = 0; row < nRows; row++) {
      if(row != i) {
        temp = work[row][i];
        for(col = nRows - 1; col >= 0; col--) {
          work[row][col] -= (temp * work[i][col]);
          result[row][col] -= (temp * result[i][col]);
        }
      }
    }
  }
  return result;
}

// Destructor
FixMatrix::~FixMatrix() {
  if(data) {
    LOG_DEL; delete[] data; 
  }
}

/////////////////////////////////////////////////////////
// Float Matrix Message Class - holds data of type double
/////////////////////////////////////////////////////////

// Constructor: make an unitialized matrix with no data
FloatMatrix::FloatMatrix() {
  nRows = nCols = totalDataSize = 0;
}

// Constructor: make an unitialized matrix with the given dimensions
FloatMatrix::FloatMatrix(int numRow, int numCol) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new double[totalDataSize];
}

// constructor:
// initialized with the data given in the Particles of the PortHole
FloatMatrix::FloatMatrix(int numRow, int numCol, PortHole& ph) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new double[totalDataSize];

  // Load the data from the PortHole into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = double(ph%(totalDataSize - i - 1));
}

// constructor:
// initialized with the data given in a dataArray FloatArrayState
FloatMatrix::FloatMatrix(int numRow, int numCol, FloatArrayState& dataArray) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;

  if(dataArray.size() < totalDataSize) {
    Error::abortRun ("not enough arguments in FloatArrayState input",
                     " to the constructor\n");
    return;
  }
  LOG_NEW; data = new double[totalDataSize];

  // Load the data from the dataArray FloatArrayState into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = dataArray[i];
}

// Copy Constructor
FloatMatrix::FloatMatrix(const FloatMatrix& src) {
  nRows = src.nRows;
  nCols = src.nCols;
  totalDataSize = src.totalDataSize;
  LOG_NEW; data = new double[totalDataSize];

  for(int i = 0; i < totalDataSize; i++)
    entry(i) = src.entry(i);
}

// Copy Constructor, copying only a submatrix of the original.  Needs
// the starting row and col to copy from the original, as well as the
// dimensions of the submatrix.  Undefined if the dimensions of the submatrix
// are go beyond the dimensions of the original matrix.
FloatMatrix::FloatMatrix(const FloatMatrix& src, int startRow, int startCol, int numRow, int numCol) {
  if(startRow + numRow > src.nRows) {
    Error::abortRun("submatrix rows extend beyond dimensions of src matrix\n");
    nRows = src.nRows - startRow;
  }
  else nRows = numRow;
  if(startCol + numCol > src.nCols) {
    Error::abortRun("submatrix cols extend beyond dimensions of src matrix\n");
    nCols = src.nCols - startCol;
  }
  else nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new double[totalDataSize];

  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nCols; col++)
      (*this)[row][col] = src[(startRow + row - 1)][(startCol + col - 1)];
}

// Prints matrices in standard row column form.
StringList FloatMatrix::print() const {
  StringList out = "FloatMatrix:\n";
  for(int row = 0; row < nRows; row++) {
    for(int col = 0; col < nCols; col++) {
      out += (*this)[row][col];
      out += " ";
    }
    out += "\n";
  }
  return out;
}

int FloatMatrix::operator == (const FloatMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols))
    return 0;
  for(int i = 0; i < totalDataSize; i++)
    if(entry(i) != src.entry(i))
      return 0;
  return 1;
}

// cast conversion operators
FloatMatrix::operator ComplexMatrix () const {
  ComplexMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = entry(i);
  return result;
}

FloatMatrix::operator FixMatrix () const {
  FixMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = entry(i);
  return result;
}

FloatMatrix::operator IntMatrix () const {
  IntMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = int(entry(i));
  return result;
}

// destructive manipulations
FloatMatrix& FloatMatrix::operator = (const FloatMatrix& src) {
// WARNING: any SubMatrices refering to the data in this matrix 
// will become invalid if this matrix's storage is reallocated.
  if(totalDataSize != src.totalDataSize) {
    LOG_DEL; delete[] data;
    totalDataSize = src.totalDataSize;
    data = new double[totalDataSize];
  }
  nRows = src.nRows;
  nCols = src.nCols;
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = src.entry(i);
  return *this;
}

FloatMatrix& FloatMatrix::operator = (float value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

FloatMatrix& FloatMatrix::operator += (const FloatMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("+= used on matrices of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += src.entry(i);
  return *this;
}

FloatMatrix& FloatMatrix::operator += (float value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += value;
  return *this;
}

FloatMatrix& FloatMatrix::operator -= (const FloatMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("-= used on matrices of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= src.entry(i);
  return *this;
}

FloatMatrix& FloatMatrix::operator -= (float value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= value;
  return *this;
}

// non-destructive manipulations
FloatMatrix FloatMatrix::transpose() const {
  FloatMatrix result(nCols,nRows);
  for(int i = 0; i < nCols; i++)
    for(int j = 0; j < nRows; j++)
      result[i][j] = (*this)[j][i];

  return result;
}

FloatMatrix FloatMatrix::inverse() const {
  FloatMatrix result(nRows,nRows);
  if(nRows != nCols) {
    Error::abortRun("attempt to invert a matrix that is not square.\n");
    return result;
  }

  FloatMatrix work(*this);                  // make a working copy of this
  double temp;

  // set result to be the identity matrix
  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nRows; col++)
      if(col == row) result[row][col] = 1;
      else result[row][col] = 0;

  for(int i = 0; i < nRows; i++) {
    // check that the element in (i,i) is not zero
    if(work[i][i] == 0) {
      // swap with a row below this one that has a non-zero element
      // in the same column
      for(row = i+1; row < nRows; row++)
        if(work[row][i] != 0)
          break;
      if(row == nRows) {
        Error::abortRun("couldn't invert matrix, possibly singular.\n");
        return result;
      }
      // swap rows
      for(col = 0; col < nRows; col++) {
        temp = work[i][col];
        work[i][col] = work[row][col];
        work[row][col] = temp;
        temp = result[i][col];
        result[i][col] = result[row][col];
        result[row][col] = temp;
      }
    }
    // divide every element in the row by element (i,i)
    temp = work[i][i];
    for(col = 0; col < nRows; col++) {
      work[i][col] /= temp;
      result[i][col] /= temp;
    }
    // zero out the rest of column i
    for(row = 0; row < nRows; row++) {
      if(row != i) {
        temp = work[row][i];
        for(col = nRows - 1; col >= 0; col--) {
          work[row][col] -= (temp * work[i][col]);
          result[row][col] -= (temp * result[i][col]);
        }
      }
    }
  }
  return result;
}

FloatMatrix::~FloatMatrix() {
  if(data) {
    LOG_DEL; delete[] data; 
  }
}

///////////////////////////////////////////////////
// IntMatrix Message Class - holds data of type int
///////////////////////////////////////////////////

// Constructor: make an unitialized matrix with no data
IntMatrix::IntMatrix() {
  nRows = nCols = totalDataSize = 0;
}

// Constructor: make an unitialized matrix with the given dimensions
IntMatrix::IntMatrix(int numRow, int numCol) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new int[totalDataSize];
}

// constructor:
// initialized with the data given in the Particles of the PortHole
IntMatrix::IntMatrix(int numRow, int numCol, PortHole& ph) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new int[totalDataSize];

  // Load the data from the PortHole into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    data[i] = int(ph%(totalDataSize - i - 1));
}

// constructor:
// initialized with the data given in a dataArray IntArrayState
IntMatrix::IntMatrix(int numRow, int numCol, IntArrayState& dataArray)
{
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;

  if(dataArray.size() < totalDataSize) {
    Error::abortRun ("not enough arguments in IntArrayState input",
                     " to the constructor\n");
    return;
  }
  LOG_NEW; data = new int[totalDataSize];

  // Load the data from the dataArray IntArrayState into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    data[i] = dataArray[i];
}

// Copy Constructor
IntMatrix::IntMatrix(const IntMatrix& src) {
  nRows = src.nRows;
  nCols = src.nCols;
  totalDataSize = src.totalDataSize;
  LOG_NEW; data = new int[totalDataSize];

  for(int i = 0; i < totalDataSize; i++)
    data[i] = src.data[i];
}

// Copy Constructor, copying only a submatrix of the original.  Needs
// the starting row and col to copy from the original, as well as the
// dimensions of the submatrix.  Undefined if the dimensions of the submatrix
// are go beyond the dimensions of the original matrix.
IntMatrix::IntMatrix(const IntMatrix& src, int startRow, int startCol, int numRow, int numCol) {
  if(startRow + numRow > src.nRows) {
    Error::abortRun("submatrix rows extend beyond dimensions of src matrix\n");
    nRows = src.nRows - startRow;
  }
  else nRows = numRow;
  if(startCol + numCol > src.nCols) {
    Error::abortRun("submatrix cols extend beyond dimensions of src matrix\n");
    nCols = src.nCols - startCol;
  }
  else nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new int[totalDataSize];

  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nCols; col++)
      (*this)[row][col] = src[(startRow + row - 1)][(startCol + col - 1)];
}

// Prints matrices in standard row column form.
StringList IntMatrix::print() const {
  StringList out = "IntMatrix:\n";
  for(int row = 0; row < nRows; row++) {
    for(int col = 0; col < nCols; col++) {
      out += (*this)[row][col];
      out += " ";
    }
    out += "\n";
  }
  return out;
}

int IntMatrix::operator == (const IntMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols))
    return 0;
  for(int i = 0; i < totalDataSize; i++)
    if(entry(i) != src.entry(i))
      return 0;
  return 1;
}

// cast conversion operators
IntMatrix::operator ComplexMatrix () const {
  ComplexMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = double(entry(i));
  return result;
}

IntMatrix::operator FixMatrix () const {
  FixMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = Fix(double(entry(i)));
  return result;
}

IntMatrix::operator FloatMatrix () const {
  FloatMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = double(entry(i));
  return result;
}

// destructive manipulations
IntMatrix& IntMatrix::operator = (const IntMatrix& src) {
// WARNING: any SubMatrices refering to the data in this matrix 
// will become invalid if this matrix's storage is reallocated.
  if(totalDataSize != src.totalDataSize) {
    LOG_DEL; delete[] data;
    totalDataSize = src.totalDataSize;
    data = new int[src.totalDataSize];
  }
  nRows = src.nRows;
  nCols = src.nCols;
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = src.entry(i);
  return *this;
}

IntMatrix& IntMatrix::operator = (int value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

IntMatrix& IntMatrix::operator += (const IntMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("+= used on matrices of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += src.entry(i);
  return *this;
}

IntMatrix& IntMatrix::operator += (int value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += value;
  return *this;
}

IntMatrix& IntMatrix::operator -= (const IntMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("-= used on matrices of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= src.entry(i);
  return *this;
}

IntMatrix& IntMatrix::operator -= (int value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= value;
  return *this;
}

// non-destructive manipulations
IntMatrix IntMatrix::transpose() const {
  IntMatrix result(nCols,nRows);
  for(int i = 0; i < nCols; i++)
    for(int j = 0; j < nRows; j++)
      result[i][j] = (*this)[j][i];

  return result;
}

IntMatrix IntMatrix::inverse() const {
  IntMatrix result(nRows,nRows);
  if(nRows != nCols) {
    Error::abortRun("attempt to invert a matrix that is not square.\n");
    return result;
  }
  IntMatrix work(*this);                  // make a working copy of this
  int temp;

  // set result to be the identity matrix
  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nRows; col++)
      if(col == row) result[row][col] = 1;
      else result[row][col] = 0;

  for(int i = 0; i < nRows; i++) {
    // check that the element in (i,i) is not zero
    if(work[i][i] == 0) {
      // swap with a row below this one that has a non-zero element
      // in the same column
      for(row = i+1; row < nRows; row++)
        if(work[row][i] != 0)
          break;
      if(row == nRows) {
        Error::abortRun("couldn't invert matrix, possibly singular.\n");
        return result;
      }
      // swap rows
      for(col = 0; col < nRows; col++) {
        temp = work[i][col];
        work[i][col] = work[row][col];
        work[row][col] = temp;
        temp = result[i][col];
        result[i][col] = result[row][col];
        result[row][col] = temp;
      }
    }
    // divide every element in the row by element (i,i)
    temp = work[i][i];
    for(col = 0; col < nRows; col++) {
      work[i][col] /= temp;
      result[i][col] /= temp;
    }
    // zero out the rest of column i
    for(row = 0; row < nRows; row++) {
      if(row != i) {
        temp = work[row][i];
        for(col = nRows - 1; col >= 0; col--) {
          work[row][col] -= (temp * work[i][col]);
          result[row][col] -= (temp * result[i][col]);
        }
      }
    }
  }
  return result;
}

// Destructor
IntMatrix::~IntMatrix() {
  if(data) {
    LOG_DEL; delete[] data; 
  }
}

/****************************************************************************
 MatrixEnvParticles, used to hold envelopes which hold the actual matrices.
*****************************************************************************/
extern const DataType COMPLEX_MATRIX_ENV = "COMPLEX_MATRIX_ENV";
extern const DataType FIX_MATRIX_ENV = "FIX_MATRIX_ENV";
extern const DataType FLOAT_MATRIX_ENV = "FLOAT_MATRIX_ENV";
extern const DataType INT_MATRIX_ENV = "INT_MATRIX_ENV";

///////////////////////////////
// MatrixEnvParticle Base Class
///////////////////////////////

static Envelope dummy;

MatrixEnvParticle::operator int() const { return errorConvert("int");}
MatrixEnvParticle::operator double() const { return errorConvert("double"); }
MatrixEnvParticle::operator float() const { return errorConvert("float"); }
MatrixEnvParticle::operator Complex() const { return errorConvert("complex");}
MatrixEnvParticle::operator Fix () const { 
  Fix dummy;
  errorConvert("fix");
  return dummy;
}

StringList MatrixEnvParticle::print() const { return data.print(); }

int MatrixEnvParticle::errorConvert(const char* arg) const {
  StringList msg = "Message type, '";
  msg += type();
  msg += "': invalid conversion to ";
  msg += arg;
  Error::abortRun(msg);
  return 0;
}

// get the Matrix from the MatrixEnvParticle.
void MatrixEnvParticle::accessMessage (Envelope& p) const {
  p = data;
}

// get the Matrix and remove the reference (by setting data to dummy)
void MatrixEnvParticle::getMessage (Envelope& p) {
  p = data;
  data = dummy;
}

Particle& MatrixEnvParticle::initialize() { data = dummy; return *this;}

void MatrixEnvParticle::operator << (const Complex& c) { 
  errorAssign("Complex"); }
void MatrixEnvParticle::operator << (double d) { errorAssign("double"); }
void MatrixEnvParticle::operator << (const Fix& c) { errorAssign("Fix"); }
void MatrixEnvParticle::operator << (float f) { errorAssign("float"); }
void MatrixEnvParticle::operator << (int i) { errorAssign("int"); }
void MatrixEnvParticle::operator << (const ComplexMatrix& m) {
  errorAssign("ComplexMatrix"); }
void MatrixEnvParticle::operator << (const FixMatrix& m) { 
  errorAssign("FixMatrix"); }
void MatrixEnvParticle::operator << (const FloatMatrix& m) { 
  errorAssign("FloatMatrix"); }
void MatrixEnvParticle::operator << (const IntMatrix& m) { 
  errorAssign("IntMatrix"); }

// particle compare: considered equal if Matrix addresses are the same.
int MatrixEnvParticle :: operator == (const Particle& p) {
  if (!typesEqual(p)) return 0;
  Envelope pkt;
  ((const MatrixEnvParticle&)p).accessMessage(pkt);
  return data.myData() == pkt.myData();
}

//////////////////////////////////
// ComplexMatrixEnvParticle Class
//////////////////////////////////

static ComplexMatrixEnvParticle cmepproto;
static Plasma ComplexMatrixEnvPlasma(cmepproto);
static PlasmaGate ComplexMatrixEnvGate(ComplexMatrixEnvPlasma);

// constructors
ComplexMatrixEnvParticle::ComplexMatrixEnvParticle(const Envelope& p) { data = p; }

ComplexMatrixEnvParticle::ComplexMatrixEnvParticle() {}

DataType ComplexMatrixEnvParticle::type() const { return COMPLEX_MATRIX_ENV;}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void ComplexMatrixEnvParticle::operator << (const Envelope& p) { 
  if(!p.typeCheck("ComplexMatrix"))
    errorAssign(p.dataType());
  else data = p;
}

void ComplexMatrixEnvParticle::operator << (const ComplexMatrix& m) { 
  Envelope p(m);
  data = p;
}

// particle copy
Particle& ComplexMatrixEnvParticle::operator = (const Particle& p) {
  if (compareType(p)) {
    const ComplexMatrixEnvParticle& ps = *(const ComplexMatrixEnvParticle*)&p;
    data = ps.data;
  }
  return *this;
}

// clone, useNew, die analogous to other particles.

Particle* ComplexMatrixEnvParticle::clone() const {
  Particle * p = ComplexMatrixEnvPlasma.get();
  ((ComplexMatrixEnvParticle*)p)->data = data;
  return p;
}

Particle* ComplexMatrixEnvParticle::useNew() const {
  LOG_NEW; return new ComplexMatrixEnvParticle;
}

// We assign "dummy" to the Envelope before returning it to the Plasma.
// this removes references to other ComplexMatrix objects and allows them
// to be deleted when no longer required.
void ComplexMatrixEnvParticle::die() {
  data = dummy;
  ComplexMatrixEnvPlasma.put(this);
}

void ComplexMatrixEnvParticle::errorAssign(const char* argType) const {
 StringList msg = "Attempt to assign type ";
  msg += argType;
  msg += " to a ComplexMatrixEnvParticle";
  Error::abortRun(msg);
}

//////////////////////////////
// FixMatrixEnvParticle Class
//////////////////////////////

static FixMatrixEnvParticle fixmepproto;
static Plasma FixMatrixEnvPlasma(fixmepproto);
static PlasmaGate FixMatrixEnvGate(FixMatrixEnvPlasma);

// constructors
FixMatrixEnvParticle::FixMatrixEnvParticle(const Envelope& p) { data = p; }

FixMatrixEnvParticle::FixMatrixEnvParticle() {}

DataType FixMatrixEnvParticle::type() const { return FIX_MATRIX_ENV;}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void FixMatrixEnvParticle::operator << (const Envelope& p) { 
  if(!p.typeCheck("FixMatrix"))
    errorAssign(p.dataType());
  else data = p;
}

void FixMatrixEnvParticle::operator << (const FixMatrix& m) { 
  Envelope p(m);
  data = p;
}

// particle copy
Particle& FixMatrixEnvParticle::operator = (const Particle& p) {
  if (compareType(p)) {
    const FixMatrixEnvParticle& ps = *(const FixMatrixEnvParticle*)&p;
    data = ps.data;
  }
  return *this;
}

// clone, useNew, die analogous to other particles.

Particle* FixMatrixEnvParticle::clone() const {
  Particle * p = FixMatrixEnvPlasma.get();
  ((FixMatrixEnvParticle*)p)->data = data;
  return p;
}

Particle* FixMatrixEnvParticle::useNew() const {
  LOG_NEW; return new FixMatrixEnvParticle;
}

// We assign "dummy" to the Envelope before returning it to the Plasma.
// this removes references to other FixMatrix objects and allows them
// to be deleted when no longer required.
void FixMatrixEnvParticle::die() {
  data = dummy;
  FixMatrixEnvPlasma.put(this);
}

void FixMatrixEnvParticle::errorAssign(const char* argType) const {
  StringList msg = "Attempt to assign type ";
  msg += argType;
  msg += " to a FixMatrixEnvParticle";
  Error::abortRun(msg);
}

///////////////////////////////
// FloatMatrixEnvParticle Class
///////////////////////////////

static FloatMatrixEnvParticle fmepproto;
static Plasma FloatMatrixEnvPlasma(fmepproto);
static PlasmaGate FloatMatrixEnvGate(FloatMatrixEnvPlasma);

// constructors
FloatMatrixEnvParticle::FloatMatrixEnvParticle(const Envelope& p) { data = p; }

FloatMatrixEnvParticle::FloatMatrixEnvParticle() {}

DataType FloatMatrixEnvParticle::type() const { return FLOAT_MATRIX_ENV;}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void FloatMatrixEnvParticle::operator << (const Envelope& p) { 
  if(!p.typeCheck("FloatMatrix"))
    errorAssign(p.dataType());
  else data = p;
}

void FloatMatrixEnvParticle::operator << (const FloatMatrix& m) { 
  Envelope p(m);
  data = p;
}

// particle copy
Particle& FloatMatrixEnvParticle::operator = (const Particle& p) {
  if (compareType(p)) {
    const FloatMatrixEnvParticle& ps = *(const FloatMatrixEnvParticle*)&p;
    data = ps.data;
  }
  return *this;
}

// clone, useNew, die analogous to other particles.

Particle* FloatMatrixEnvParticle::clone() const {
  Particle * p = FloatMatrixEnvPlasma.get();
  ((FloatMatrixEnvParticle*)p)->data = data;
  return p;
}

Particle* FloatMatrixEnvParticle::useNew() const {
  LOG_NEW; return new FloatMatrixEnvParticle;
}

// We assign "dummy" to the Envelope before returning it to the Plasma.
// this removes references to other FloatMatrix objects and allows them
// to be deleted when no longer required.
void FloatMatrixEnvParticle::die() {
  data = dummy;
  FloatMatrixEnvPlasma.put(this);
}

void FloatMatrixEnvParticle::errorAssign(const char* argType) const {
  StringList msg = "Attempt to assign type ";
  msg += argType;
  msg += " to a FloatMatrixEnvParticle";
  Error::abortRun(msg);
}

//////////////////////////////
// IntMatrixEnvParticle Class
//////////////////////////////

static IntMatrixEnvParticle imepproto;
static Plasma IntMatrixEnvPlasma(imepproto);
static PlasmaGate IntMatrixEnvGate(IntMatrixEnvPlasma);

// constructors
IntMatrixEnvParticle::IntMatrixEnvParticle(const Envelope& p) { data = p; }

IntMatrixEnvParticle::IntMatrixEnvParticle() {}

DataType IntMatrixEnvParticle::type() const { return INT_MATRIX_ENV;}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void IntMatrixEnvParticle::operator << (const Envelope& p) { 
  if(!p.typeCheck("IntMatrix"))
    errorAssign(p.dataType());
  else data = p;
}

void IntMatrixEnvParticle::operator << (const IntMatrix& m) { 
  Envelope p(m);
  data = p;
}

// particle copy
Particle& IntMatrixEnvParticle::operator = (const Particle& p) {
  if (compareType(p)) {
    const IntMatrixEnvParticle& ps = *(const IntMatrixEnvParticle*)&p;
    data = ps.data;
  }
  return *this;
}

// clone, useNew, die analogous to other particles.

Particle* IntMatrixEnvParticle::clone() const {
  Particle * p = IntMatrixEnvPlasma.get();
  ((IntMatrixEnvParticle*)p)->data = data;
  return p;
}

Particle* IntMatrixEnvParticle::useNew() const {
  LOG_NEW; return new IntMatrixEnvParticle;
}

// We assign "dummy" to the Envelope before returning it to the Plasma.
// this removes references to other IntMatrix objects and allows them
// to be deleted when no longer required.
void IntMatrixEnvParticle::die() {
  data = dummy;
  IntMatrixEnvPlasma.put(this);
}

void IntMatrixEnvParticle::errorAssign(const char* argType) const {
  StringList msg = "Attempt to assign type ";
  msg += argType;
  msg += " to a IntMatrixEnvParticle";
  Error::abortRun(msg);
}
