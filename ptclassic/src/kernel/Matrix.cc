static const char file_id[] = "Matrix.cc";
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

 This file defines the implementation of the Matrix Message classes and
 the Matrix Envelope Particles that hold them.

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include <strstream.h>
#include "Matrix.h"
#include "Plasma.h"
#include "Error.h"

#define SMALL_STRING 32   // should be same as for StringList.cc

/**************************************************************************
 Global operators, not members of the various Matrix classes.
 It is possible to define some of these functions in terms of others,
 for example:

  PtMatrix operator + (const PtMatrix& src1, const PtMatrix& src2) {
    PtMatrix result(src1);
    result += src2;
    return result;
  }
  PtMatrix& operator += (const PtMatrix& src2) {
    // do add stuff
  }

 or defining += in terms of the binary +, but it is most efficient to
 have the full operations defined in each function because copying the
 results is too expensive.
**************************************************************************/

// Add two matricies
ComplexMatrix operator+ (const ComplexMatrix& src1,const ComplexMatrix& src2) {
  if((src1.nRows != src2.nRows) || (src1.nCols != src2.nCols)) {
    Error::abortRun("addition used on matricies of different dimensions\n");
    return src1;
  }
  ComplexMatrix result(src1.nRows,src1.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = src1.entry(i) + src2.entry(i);
  return result;
}

// Pre-add matrix with a scalar
ComplexMatrix operator+ (const Complex& scalar,const ComplexMatrix& matrix) {
  ComplexMatrix result(matrix.nRows,matrix.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = matrix.entry(i) + scalar;
  return result;
}

// Post-add matrix with a scalar
ComplexMatrix operator+ (const ComplexMatrix& matrix,const Complex& scalar) {
  return (scalar + matrix);
}

// Subtract two matricies
ComplexMatrix operator- (const ComplexMatrix& src1,const ComplexMatrix& src2) {
  if((src1.nRows != src2.nRows) || (src1.nCols != src2.nCols)) {
    Error::abortRun("subtraction used on matricies of different dimensions\n");
    return src1;
  }
  ComplexMatrix result(src1.nRows,src1.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = src1.entry(i) - src2.entry(i);
  return result;
}

// Pre-subtract matrix with a scalar
ComplexMatrix operator- (const Complex& scalar,const ComplexMatrix& matrix) {
  return ((-scalar) + matrix);
}

// Post-subtract matrix with a scalar
ComplexMatrix operator- (const ComplexMatrix& matrix,const Complex& scalar) {
  return ((-scalar) + matrix);
}

// Multiply two matricies, true matrix multiply, this is a fairly fast
// algorithm, especially when optimized by the compiler.
ComplexMatrix operator* (const ComplexMatrix& src1,const ComplexMatrix& src2) {
  ComplexMatrix result(src1.nRows, src2.nCols);

  if(src1.nCols != src2.nRows) {
    Error::abortRun("multiplication used on matricies with incompatible dimensions.\n");
    return result;
  }
  
  Complex temp;
  for(int i = 0; i < src1.nRows; i++)
    for(int j = 0; j < src2.nCols; j++) {
      temp = src1[i][0] * src2[0][j];
      for(int k = 1; k < src1.nCols; k++)
        temp += src1[i][k] * src2[k][j];
      result[i][j] = temp;
    }
  return result;
}

// Pre-multiply matrix with a scalar element wise
ComplexMatrix operator* (const Complex& scalar,const ComplexMatrix& matrix) {
  ComplexMatrix result(matrix.nRows,matrix.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = matrix.entry(i) * scalar;
  return result;
}

// Post-multiply matrix with a scalar element wise
ComplexMatrix operator* (const ComplexMatrix& matrix,const Complex& scalar) {
  return (scalar * matrix);
}

// Multiply two matricies, and put result in the third.  This is similar to
// the binary operator * but faster because by passing the reference to the
// result, we avoid an extra copy constructor step.
ComplexMatrix& multiply (const ComplexMatrix& left,const ComplexMatrix& right,
                         ComplexMatrix& result) {
  if((left.nCols != right.nRows) || (left.nRows != result.nRows) ||
     (right.nCols != result.nCols)) {
    Error::abortRun("multiplication used on matricies with incompatible dimensions.\n");
    return result;
  }
  
  Complex temp;
  for(int i = 0; i < left.nRows; i++)
    for(int j = 0; j < right.nCols; j++) {
      temp = left[i][0] * right[0][j];
      for(int k = 1; k < left.nCols; k++)
        temp += left[i][k] * right[k][j];
      result[i][j] = temp;
    }
  return result;
}

// Add two matricies
FixMatrix operator + (const FixMatrix& src1, const FixMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("addition used on matricies of different dimensions.\n"); 
    return src1;
  }
  FixMatrix result(src1.nRows,src1.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = src1.entry(i) + src2.entry(i);
  return result;
}

// Pre-add matrix with a scalar
FixMatrix operator+ (const Fix& scalar,const FixMatrix& matrix) {
  FixMatrix result(matrix.nRows,matrix.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = matrix.entry(i) + scalar;
  return result;
}

// Post-add matrix with a scalar
FixMatrix operator+ (const FixMatrix& matrix,const Fix& scalar) {
  return (scalar + matrix);
}

// Subtract two matricies
FixMatrix operator - (const FixMatrix& src1, const FixMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("subtraction used on matricies of different dimensions\n");
    return src1;
  }
  FixMatrix result(src1.nRows,src1.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = src1.entry(i) - src2.entry(i);
  return result;
}

// Pre-subtract matrix with a scalar
// Note this takes scalar by value because unary operator - on Fix is
// destructive (i.e. replaces the Fix with its complement).
FixMatrix operator- (const Fix& scalar,const FixMatrix& matrix) {
  return ((-scalar) + matrix);
}

// Post-subtract matrix with a scalar
// Note this takes scalar by value because unary operator - on Fix is
// destructive (i.e. replaces the Fix with its complement).
FixMatrix operator- (const FixMatrix& matrix,const Fix& scalar) {
  return ((-scalar) + matrix);
}

// Multiply two matricies, true matrix multiply, this is a fairly fast
// algorithm, especially when optimized by the compiler.
FixMatrix operator * (const FixMatrix& src1, const FixMatrix& src2) {
  FixMatrix result(src1.nRows, src2.nCols);

  if(src1.nCols != src2.nRows) {
    Error::abortRun("multiplication used on matricies with incompatible dimensions.\n"); 
    return result;
  }
  
  Fix temp;
  for(int i = 0; i < src1.nRows; i++)
    for(int j = 0; j < src2.nCols; j++) {
      temp = src1[i][0] * src2[0][j];
      for(int k = 1; k < src1.nCols; k++)
        temp += src1[i][k] * src2[k][j];
      result[i][j] = temp;
    }
  return result;
}

// Pre-multiply matrix with a scalar element wise
FixMatrix operator* (const Fix& scalar,const FixMatrix& matrix) {
  FixMatrix result(matrix.nRows,matrix.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = matrix.entry(i) * scalar;
  return result;
}

// Post-multiply matrix with a scalar element wise
FixMatrix operator* (const FixMatrix& matrix,const Fix& scalar) {
  return (scalar * matrix);
}

// Multiply two matricies, and put result in the third.  This is similar to
// the binary operator * but faster because by passing the reference to the
// result, we avoid an extra copy constructor step.
FixMatrix& multiply (const FixMatrix& left,const FixMatrix& right,
                     FixMatrix& result) {
  if((left.nCols != right.nRows) || (left.nRows != result.nRows) ||
     (right.nCols != result.nCols)) {
    Error::abortRun("multiplication used on matricies with incompatible dimensions.\n");
    return result;
  }
  
  Fix temp;
  for(int i = 0; i < left.nRows; i++)
    for(int j = 0; j < right.nCols; j++) {
      temp = left[i][0] * right[0][j];
      for(int k = 1; k < left.nCols; k++)
        temp += left[i][k] * right[k][j];
      result[i][j] = temp;
    }
  return result;
}

// Add two matricies
FloatMatrix operator + (const FloatMatrix& src1, const FloatMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("addition used on matricies of different dimensions.\n"); 
    return src1;
  }
  FloatMatrix result(src1.nRows,src1.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = src1.entry(i) + src2.entry(i);
  return result;
}

// Pre-add matrix with a scalar
FloatMatrix operator+ (double scalar,const FloatMatrix& matrix) {
  FloatMatrix result(matrix.nRows,matrix.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = matrix.entry(i) + scalar;
  return result;
}

// Post-add matrix with a scalar
FloatMatrix operator+ (const FloatMatrix& matrix,double scalar) {
  return (scalar + matrix);
}

// Subtract two matricies
FloatMatrix operator - (const FloatMatrix& src1, const FloatMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("subtraction used on matricies of different dimensions\n");
    return src1;
  }
  FloatMatrix result(src1.nRows,src1.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = src1.entry(i) - src2.entry(i);
  return result;
}

// Pre-subtract matrix with a scalar
FloatMatrix operator- (double scalar,const FloatMatrix& matrix) {
  return ((-scalar) + matrix);
}

// Post-subtract matrix with a scalar
FloatMatrix operator- (const FloatMatrix& matrix,double scalar) {
  return ((-scalar) + matrix);
}

// Multiply two matricies, true matrix multiply, this is a fairly fast
// algorithm, especially when optimized by the compiler.
FloatMatrix operator * (const FloatMatrix& src1, const FloatMatrix& src2) {
  FloatMatrix result(src1.nRows, src2.nCols);

  if(src1.nCols != src2.nRows) {
    Error::abortRun("multiplication used on matricies with incompatible dimensions.\n");
    return result;
  }
  
  double temp;
  for(int i = 0; i < src1.nRows; i++)
    for(int j = 0; j < src2.nCols; j++) {
      temp = src1[i][0] * src2[0][j];
      for(int k = 1; k < src1.nCols; k++)
        temp += src1[i][k] * src2[k][j];
      result[i][j] = temp;
    }
  return result;
}

// Pre-multiply matrix with a scalar element wise
FloatMatrix operator* (double scalar,const FloatMatrix& matrix) {
  FloatMatrix result(matrix.nRows,matrix.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = matrix.entry(i) * scalar;
  return result;
}

// Post-multiply matrix with a scalar element wise
FloatMatrix operator* (const FloatMatrix& matrix,double scalar) {
  return (scalar * matrix);
}

// Multiply two matricies, and put result in the third.  This is similar to
// the binary operator * but faster because by passing the reference to the
// result, we avoid an extra copy constructor step.
FloatMatrix& multiply (const FloatMatrix& left,const FloatMatrix& right,
                       FloatMatrix& result) {
  if((left.nCols != right.nRows) || (left.nRows != result.nRows) ||
     (right.nCols != result.nCols)) {
    Error::abortRun("multiplication used on matricies with incompatible dimensions.\n");
    return result;
  }
  
  double temp;
  for(int i = 0; i < left.nRows; i++)
    for(int j = 0; j < right.nCols; j++) {
      temp = left[i][0] * right[0][j];
      for(int k = 1; k < left.nCols; k++)
        temp += left[i][k] * right[k][j];
      result[i][j] = temp;
    }
  return result;
}

// Add two matricies
IntMatrix operator + (const IntMatrix& src1, const IntMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("addition used on matricies of different dimensions.\n"); 
    return src1;
  }
  IntMatrix result(src1.nRows,src1.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = src1.entry(i) + src2.entry(i);
  return result;
}

// Pre-add matrix with a scalar
IntMatrix operator+ (int scalar,const IntMatrix& matrix) {
  IntMatrix result(matrix.nRows,matrix.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = matrix.entry(i) + scalar;
  return result;
}

// Post-add matrix with a scalar
IntMatrix operator+ (const IntMatrix& matrix,int scalar) {
  return (scalar + matrix);
}

// Subtract two matricies
IntMatrix operator - (const IntMatrix& src1, const IntMatrix& src2) {
  if((src1.nRows != src2.nRows)||(src1.nCols != src2.nCols)) {
    Error::abortRun("subtraction used on matricies of different dimensions\n");
    return src1;
  }
  IntMatrix result(src1.nRows,src1.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = src1.entry(i) - src2.entry(i);
  return result;
}

// Pre-subtract matrix with a scalar
IntMatrix operator- (int scalar,const IntMatrix& matrix) {
  return ((-scalar) + matrix);
}

// Post-subtract matrix with a scalar
IntMatrix operator- (const IntMatrix& matrix,int scalar) {
  return ((-scalar) + matrix);
}

// Multiply two matricies, true matrix multiply, this is a fairly fast
// algorithm, especially when optimized by the compiler.
IntMatrix operator * (const IntMatrix& src1, const IntMatrix& src2) {
  IntMatrix result(src1.nRows, src2.nCols);

  if(src1.nCols != src2.nRows) {
    Error::abortRun("multiplication used on matricies with incompatible dimensions.\n"); 
    return result;
  }
  
  int temp;
  for(int i = 0; i < src1.nRows; i++)
    for(int j = 0; j < src2.nCols; j++) {
      temp = src1[i][0] * src2[0][j];
      for(int k = 1; k < src1.nCols; k++)
        temp += src1[i][k] * src2[k][j];
      result[i][j] = temp;
    }
  return result;
}

// Pre-multiply matrix with a scalar element wise
IntMatrix operator* (int scalar,const IntMatrix& matrix) {
  IntMatrix result(matrix.nRows,matrix.nCols);
  for(int i = 0; i < result.totalDataSize; i++)
    result.entry(i) = matrix.entry(i) * scalar;
  return result;
}

// Post-multiply matrix with a scalar element wise
IntMatrix operator* (const IntMatrix& matrix,int scalar) {
  return (scalar * matrix);
}

// Multiply two matricies, and put result in the third.  This is similar to
// the binary operator * but faster because by passing the reference to the
// result, we avoid an extra copy constructor step.
IntMatrix& multiply (const IntMatrix& left,const IntMatrix& right,
                     IntMatrix& result) {
  if((left.nCols != right.nRows) || (left.nRows != result.nRows) ||
     (right.nCols != result.nCols)) {
    Error::abortRun("multiplication used on matricies with incompatible dimensions.\n");
    return result;
  }
  
  int temp;
  for(int i = 0; i < left.nRows; i++)
    for(int j = 0; j < right.nCols; j++) {
      temp = left[i][0] * right[0][j];
      for(int k = 1; k < left.nCols; k++)
        temp += left[i][k] * right[k][j];
      result[i][j] = temp;
    }
  return result;
}

/////////////////////////////////////////////////////////////
// Matrix Message Class
/////////////////////////////////////////////////////////////

int PtMatrix::compareType(const PtMatrix & m) const {
  if(typesEqual(m)) return 1;
  StringList msg = "Attempt to copy ";
  msg += m.dataType();
  msg += " Ptolemy Matrix to ";
  msg += dataType();
  msg += " Ptolemy Matrix";
  Error::abortRun (msg);
  return 0;
}

/////////////////////////////////////////////////////////////
// Complex Matrix Message Class - holds data of type Complex
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
      (*this)[row][col] = src[(startRow + row)][(startCol + col)];
}

// Prints matricies in standard row column form.
StringList ComplexMatrix::print() const {
  ostrstream strm;
  char buf[SMALL_STRING];
  strm << "ComplexMatrix: (";
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

int ComplexMatrix::operator == (const PtMatrix& src) const {
  if(!typesEqual(src)) return 0;
  if((nRows != ((const ComplexMatrix*)&src)->nRows) || 
     (nCols != ((const ComplexMatrix*)&src)->nCols))
    return 0;
  for(int i = 0; i < totalDataSize; i++)
    if(entry(i) != ((const ComplexMatrix*)&src)->entry(i))
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

// destructive replacement operators
PtMatrix& ComplexMatrix::operator = (const PtMatrix& m) {
// WARNING: any SubMatricies refering to the data in this matrix 
// will become invalid if this matrix's storage is reallocated.
  if(compareType(m)) {
    const ComplexMatrix& src = *((const ComplexMatrix*)&m);
    if(totalDataSize != src.totalDataSize) {
      LOG_DEL; delete[] data;
      totalDataSize = src.totalDataSize;
      data = new Complex[totalDataSize];
    }
    nRows = src.nRows;
    nCols = src.nCols;
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = src.entry(i);
  }
  return *this;
}

ComplexMatrix& ComplexMatrix::operator = (const Complex& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

ComplexMatrix& ComplexMatrix::operator += (const ComplexMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("+= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += src.entry(i);
  return *this;
}

ComplexMatrix& ComplexMatrix::operator += (const Complex& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += value;
  return *this;
}

ComplexMatrix& ComplexMatrix::operator -= (const ComplexMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("-= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= src.entry(i);
  return *this;
}

ComplexMatrix& ComplexMatrix::operator -= (const Complex& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= value;
  return *this;
}

// Note: this is element-wise multiplication and not regular matrix multiply
ComplexMatrix& ComplexMatrix::operator *= (const ComplexMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("*= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) *= src.entry(i);
  return *this;
}

ComplexMatrix& ComplexMatrix::operator *= (const Complex& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) *= value;
  return *this;
}

ComplexMatrix& ComplexMatrix::operator /= (const ComplexMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("/= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) /= src.entry(i);
  return *this;
}

ComplexMatrix& ComplexMatrix::operator /= (const Complex& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) /= value;
  return *this;
}

// Initialize this into an identity matrix, this does not have to be square.
ComplexMatrix& ComplexMatrix::identity() {
  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nCols; col++) {
      if(row == col)
        (*this)[row][col] = 1;
      else
        (*this)[row][col] = 0;
    }
  return *this;
}

// non-destructive operators, returns new Matricies
// Return a new matrix that is element-wise the negative of this.
ComplexMatrix ComplexMatrix::operator - () const {
  ComplexMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = -entry(i);
  return result;
}

// Calculate the powers of a square matrix
ComplexMatrix ComplexMatrix::operator ^ (int exponent) const {
  if(nRows != nCols) {
    Error::abortRun("attempt to take the power of a non-square matrix\n");
    return *this;
  }
  if(exponent < 0)
    return (!(*this^(-exponent)));  // invert the power at the end
  if(exponent == 0) {               // matrix to zero-th power is identity
    ComplexMatrix result(nRows,nCols);
    return result.identity();
  }
  if(exponent == 1)
    return *this;
  if(exponent % 2 == 1)             // matrix to odd power
    return (*this * (*this^(exponent - 1)));
  else                              // matrix to even power
    return ((*this^(exponent/2)) * (*this^(exponent/2)));
}

ComplexMatrix ComplexMatrix::transpose() const {
  ComplexMatrix result(nCols,nRows);
  for(int i = 0; i < nCols; i++)
    for(int j = 0; j < nRows; j++)
      result[i][j] = (*this)[j][i];

  return result;
}

// complex conjugate
ComplexMatrix ComplexMatrix::conjugate() const {
  ComplexMatrix result(nRows,nCols);
  for(int i = 0; i < nRows; i++)
    for(int j = 0; j < nCols; j++)
      result[i][j] = conj((*this)[i][j]);

  return result;
}

// Hermitian transpose, i.e. conjugate transpose
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
  int row, col;

  // set result to be the identity matrix
  result.identity();

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

// Constructor: make an unitialized matrix with the given dimensions,
// using Fix elements of the default size as defined in the Fix class
FixMatrix::FixMatrix(int numRow, int numCol) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];
}

// Constructor: make an unitialized matrix with the given dimensions,
// using Fix elements of the given length "ln" and number of integer
// bits "ib"
FixMatrix::FixMatrix(int numRow, int numCol, int ln, int ib) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];
  for(int i = 0; i < totalDataSize; i++)
    data[i] = Fix(ln,ib);
}


// constructor:
// initialized with the data given in the Particles of the PortHole,
// using Fix elements of the default size as defined in the Fix class
FixMatrix::FixMatrix(int numRow, int numCol, PortHole& ph) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];

  // Load the data from the PortHole into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    data[i] = Fix(ph%(totalDataSize - i - 1));
}

// Constructor: 
// initialized with the data given in the Particles of the PortHole,
// using Fix elements of the given length "ln" and number of integer
// bits "ib"
FixMatrix::FixMatrix(int numRow, int numCol, int ln, int ib, PortHole& ph) {
  nRows = numRow;
  nCols = numCol;
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];

  // Load the data from the PortHole into the matrix.
  for(int i = 0; i < totalDataSize; i++) {
    double ph_data = ph%(totalDataSize - i - 1);
    data[i] = Fix(ln,ib,ph_data);
  }
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

// constructor:
// initialized with the data given in a dataArray FixArrayState
// using Fix elements of the given length "ln" and number of integer
// bits "ib"
FixMatrix::FixMatrix(int rows,int cols,int ln,int ib,FixArrayState& dataArray)
{
  nRows = rows;
  nCols = cols;
  totalDataSize = nRows * nCols;

  if(dataArray.size() < totalDataSize) {
    Error::abortRun ("not enough arguments in FixArrayState input",
                     " to the constructor\n");
    return;
  }
  LOG_NEW; data = new Fix[totalDataSize];

  // Load the data from the dataArray FixArrayState into the matrix.
  for(int i = 0; i < totalDataSize; i++)
    data[i] = Fix(ln,ib,dataArray[i]);
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

// special conversion copy constructors
FixMatrix::FixMatrix(const ComplexMatrix& src, int ln, int ib, int round) {
  nRows = src.numRows();
  nCols = src.numCols();
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];

  for(int i = 0; i < totalDataSize; i++) {
    data[i].set_rounding(round);
    data[i] = Fix(ln,ib,(double)abs(src.entry(i)));
  }
}

FixMatrix::FixMatrix(const FloatMatrix& src, int ln, int ib, int round) {
  nRows = src.numRows();
  nCols = src.numCols();
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];

  for(int i = 0; i < totalDataSize; i++) {
    data[i].set_rounding(round);
    data[i] = Fix(ln,ib,src.entry(i));
  }
}

FixMatrix::FixMatrix(const IntMatrix& src, int ln, int ib, int round) {
  nRows = src.numRows();
  nCols = src.numCols();
  totalDataSize = nRows * nCols;
  LOG_NEW; data = new Fix[totalDataSize];

  for(int i = 0; i < totalDataSize; i++) {
    data[i].set_rounding(round);
    data[i] = Fix(ln,ib,(double)src.entry(i));
  }
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
      (*this)[row][col] = src[(startRow + row)][(startCol + col)];
}

// Prints matricies in standard row column form.
StringList FixMatrix::print() const {
  ostrstream strm;
  char buf[SMALL_STRING];
  strm << "FixMatrix: (";
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

int FixMatrix::operator == (const PtMatrix& src) const {
  if(!typesEqual(src)) return 0;
  if((nRows != ((const FixMatrix*)&src)->nRows) || 
     (nCols != ((const FixMatrix*)&src)->nCols))
    return 0;
  for(int i = 0; i < totalDataSize; i++)
    if(entry(i) != ((const FixMatrix*)&src)->entry(i))
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

// destructive replacement operators
PtMatrix& FixMatrix::operator = (const PtMatrix& m) {
// WARNING: any SubMatricies refering to the data in this matrix 
// will become invalid if this matrix's storage is reallocated.
  if(compareType(m)) {
    const FixMatrix& src = *((const FixMatrix*)&m);
    if(totalDataSize != src.totalDataSize) {
      LOG_DEL; delete[] data;
      totalDataSize = src.totalDataSize;
      data = new Fix[totalDataSize];
    }
    nRows = src.nRows;
    nCols = src.nCols;
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = src.entry(i);
  }
  return *this;
}

FixMatrix& FixMatrix::operator = (const Fix& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

FixMatrix& FixMatrix::operator += (const FixMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("+= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += src.entry(i);
  return *this;
}

FixMatrix& FixMatrix::operator += (const Fix& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += value;
  return *this;
}

FixMatrix& FixMatrix::operator -= (const FixMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("-= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= src.entry(i);
  return *this;
}

FixMatrix& FixMatrix::operator -= (const Fix& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= value;
  return *this;
}

// Note: this is element-wise multiplication and not regular matrix multiply
FixMatrix& FixMatrix::operator *= (const FixMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("*= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) *= src.entry(i);
  return *this;
}

FixMatrix& FixMatrix::operator *= (const Fix& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) *= value;
  return *this;
}

FixMatrix& FixMatrix::operator /= (const FixMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("/= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) /= src.entry(i);
  return *this;
}

FixMatrix& FixMatrix::operator /= (const Fix& value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) /= value;
  return *this;
}

// Initialize this into an identity matrix, this does not have to be square.
FixMatrix& FixMatrix::identity() {
  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nCols; col++) {
      if(row == col)
        (*this)[row][col] = double(1);
      else
        (*this)[row][col] = double(0);
    }
  return *this;
}

// non-destructive operators, returns new Matricies
// Return a new matrix that is element-wise the negative of this.
FixMatrix FixMatrix::operator - () const {
  FixMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = -entry(i);
  return result;
}

// Calculate the powers of a square matrix
FixMatrix FixMatrix::operator ^ (int exponent) const {
  if(nRows != nCols) {
    Error::abortRun("attempt to take the power of a non-square matrix\n");
    return *this;
  }
  if(exponent < 0)
    return (!(*this^(-exponent)));  // invert the power at the end
  if(exponent == 0) {               // matrix to zero-th power is identity
    FixMatrix result(nRows,nCols);
    return result.identity();
  }
  if(exponent == 1)
    return *this;
  if(exponent % 2 == 1)             // matrix to odd power
    return (*this * (*this^(exponent - 1)));
  else                              // matrix to even power
    return ((*this^(exponent/2)) * (*this^(exponent/2)));
}

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
  int row, col;

  // set result to be the identity matrix
  result.identity();

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
      (*this)[row][col] = src[(startRow + row)][(startCol + col)];
}

// Prints matricies in standard row column form.
StringList FloatMatrix::print() const {
  ostrstream strm;
  char buf[SMALL_STRING];
  strm << "FloatMatrix: (";
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

int FloatMatrix::operator == (const PtMatrix& src) const {
  if(!typesEqual(src)) return 0;
  if((nRows != ((const FloatMatrix*)&src)->nRows) || 
     (nCols != ((const FloatMatrix*)&src)->nCols))
    return 0;
  for(int i = 0; i < totalDataSize; i++)
    if(entry(i) != ((const FloatMatrix*)&src)->entry(i))
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

// destructive replacement operators
PtMatrix& FloatMatrix::operator = (const PtMatrix& m) {
// WARNING: any SubMatricies refering to the data in this matrix 
// will become invalid if this matrix's storage is reallocated.
  if(compareType(m)) {
    const FloatMatrix& src = *((const FloatMatrix*)&m);
    if(totalDataSize != src.totalDataSize) {
      LOG_DEL; delete[] data;
      totalDataSize = src.totalDataSize;
      data = new double[totalDataSize];
    }
    nRows = src.nRows;
    nCols = src.nCols;
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = src.entry(i);
  }
  return *this;
}

FloatMatrix& FloatMatrix::operator = (double value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

FloatMatrix& FloatMatrix::operator += (const FloatMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("+= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += src.entry(i);
  return *this;
}

FloatMatrix& FloatMatrix::operator += (double value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) += value;
  return *this;
}

FloatMatrix& FloatMatrix::operator -= (const FloatMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("-= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= src.entry(i);
  return *this;
}

FloatMatrix& FloatMatrix::operator -= (double value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) -= value;
  return *this;
}

// Note: this is element-wise multiplication and not regular matrix multiply
FloatMatrix& FloatMatrix::operator *= (const FloatMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("*= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) *= src.entry(i);
  return *this;
}

FloatMatrix& FloatMatrix::operator *= (double value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) *= value;
  return *this;
}

FloatMatrix& FloatMatrix::operator /= (const FloatMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("/= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) /= src.entry(i);
  return *this;
}

FloatMatrix& FloatMatrix::operator /= (double value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) /= value;
  return *this;
}

// Initialize this into an identity matrix, this does not have to be square.
FloatMatrix& FloatMatrix::identity() {
  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nCols; col++) {
      if(row == col)
        (*this)[row][col] = 1;
      else
        (*this)[row][col] = 0;
    }
  return *this;
}

// non-destructive operators, returns new Matricies
// Return a new matrix that is element-wise the negative of this.
FloatMatrix FloatMatrix::operator - () const {
  FloatMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = -entry(i);
  return result;
}

// Calculate the powers of a square matrix
FloatMatrix FloatMatrix::operator ^ (int exponent) const {
  if(nRows != nCols) {
    Error::abortRun("attempt to take the power of a non-square matrix\n");
    return *this;
  }
  if(exponent < 0)
    return (!(*this^(-exponent)));  // invert the power at the end
  if(exponent == 0) {               // matrix to zero-th power is identity
    FloatMatrix result(nRows,nCols);
    return result.identity();
  }
  if(exponent == 1)
    return *this;
  if(exponent % 2 == 1)             // matrix to odd power
    return (*this * (*this^(exponent - 1)));
  else                              // matrix to even power
    return ((*this^(exponent/2)) * (*this^(exponent/2)));
}

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
  int row, col;

  // set result to be the identity matrix
  result.identity();

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
      (*this)[row][col] = src[(startRow + row)][(startCol + col)];
}

// Prints matricies in standard row column form.
StringList IntMatrix::print() const {
  ostrstream strm;
  char buf[SMALL_STRING];
  strm << "IntMatrix: (";
  strm << nRows;
  strm << ",";
  strm << nCols;
  strm << ")\n";
  for(int row = 0; row < nRows; row++) {
    for(int col = 0; col < nCols; col++) {
      sprintf(buf,"%11d", (*this)[row][col]);
      strm << buf;
      strm << " ";
    }
    strm << "\n";
  }
  strm << ends;
  return StringList(strm.str());
}

int IntMatrix::operator == (const PtMatrix& src) const {
  if(!typesEqual(src)) return 0;
  if((nRows != ((const IntMatrix*)&src)->nRows) || 
     (nCols != ((const IntMatrix*)&src)->nCols))
    return 0;
  for(int i = 0; i < totalDataSize; i++)
    if(entry(i) != ((const IntMatrix*)&src)->entry(i))
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

// destructive replacement operators
PtMatrix& IntMatrix::operator = (const PtMatrix& m) {
// WARNING: any SubMatricies refering to the data in this matrix 
// will become invalid if this matrix's storage is reallocated.
  if(compareType(m)) {
    const IntMatrix& src = *((const IntMatrix*)&m);
    if(totalDataSize != src.totalDataSize) {
      LOG_DEL; delete[] data;
      totalDataSize = src.totalDataSize;
      data = new int[src.totalDataSize];
    }
    nRows = src.nRows;
    nCols = src.nCols;
    for(int i = 0; i < totalDataSize; i++)
      entry(i) = src.entry(i);
  }
  return *this;
}

IntMatrix& IntMatrix::operator = (int value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) = value;
  return *this;
}

IntMatrix& IntMatrix::operator += (const IntMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("+= used on matricies of different dimensions\n");
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
    Error::abortRun("-= used on matricies of different dimensions\n");
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

// Note: this is element-wise multiplication and not regular matrix multiply
IntMatrix& IntMatrix::operator *= (const IntMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("*= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) *= src.entry(i);
  return *this;
}

IntMatrix& IntMatrix::operator *= (int value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) *= value;
  return *this;
}

IntMatrix& IntMatrix::operator /= (const IntMatrix& src) {
  if((nRows != src.nRows) || (nCols != src.nCols)) {
    Error::abortRun("/= used on matricies of different dimensions\n");
    return *this;
  }
  for(int i = 0; i < totalDataSize; i++)
    entry(i) /= src.entry(i);
  return *this;
}

IntMatrix& IntMatrix::operator /= (int value) {
  for(int i = 0; i < totalDataSize; i++)
    entry(i) /= value;
  return *this;
}

// Initialize this into an identity matrix, this does not have to be square.
IntMatrix& IntMatrix::identity() {
  for(int row = 0; row < nRows; row++)
    for(int col = 0; col < nCols; col++) {
      if(row == col)
        (*this)[row][col] = 1;
      else
        (*this)[row][col] = 0;
    }
  return *this;
}

// non-destructive operators, returns new Matricies
// Return a new matrix that is element-wise the negative of this.
IntMatrix IntMatrix::operator - () const {
  IntMatrix result(nRows,nCols);
  for(int i = 0; i < totalDataSize; i++)
    result.entry(i) = -entry(i);
  return result;
}

// Calculate the powers of a square matrix
IntMatrix IntMatrix::operator ^ (int exponent) const {
  if(nRows != nCols) {
    Error::abortRun("attempt to take the power of a non-square matrix\n");
    return *this;
  }
  if(exponent < 0)
    return (!(*this^(-exponent)));  // invert the power at the end
  if(exponent == 0) {               // matrix to zero-th power is identity
    IntMatrix result(nRows,nCols);
    return result.identity();
  }
  if(exponent == 1)
    return *this;
  if(exponent % 2 == 1)             // matrix to odd power
    return (*this * (*this^(exponent - 1)));
  else                              // matrix to even power
    return ((*this^(exponent/2)) * (*this^(exponent/2)));
}

IntMatrix IntMatrix::transpose() const {
  IntMatrix result(nCols,nRows);
  for(int i = 0; i < nCols; i++)
    for(int j = 0; j < nRows; j++)
      result[i][j] = (*this)[j][i];

  return result;
}

// Note, if A is the original matrix and !A is its inverse as an IntMatrix,
//   then A * !A does not necessarily equal the identity matrix because
//   A's inverse might not be representable as an IntMatrix.
IntMatrix IntMatrix::inverse() const {
  IntMatrix result(nRows,nRows);
  if(nRows != nCols) {
    Error::abortRun("attempt to invert a matrix that is not square.\n");
    return result;
  }
  IntMatrix work(*this);                  // make a working copy of this
  int temp;
  int row, col;

  // set result to be the identity matrix
  result.identity();

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
 MatrixEnvParticles, used to hold envelopes which hold the actual matricies.
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
  msg << type();
  msg << "': invalid conversion to ";
  msg << arg;
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

// Initialize a given ParticleStack with the values in delay,
// obtaining other particles from the given plasma.  Returns
// the number of toal particles initialized, including this one.
// Assumes that the delay string is in the following format:
// "nrows ncols (A[0][0].real,A[0][0].imag) (A[0][1].real,A[0][1].imag), ... A[0][ncols-1] A[1][0] ... (A[nrows][ncols].real,A[nrows][ncols].imag)"
// 3/2/94 added
int ComplexMatrixEnvParticle::initParticleStack(Block* parent,
                                                ParticleStack& pstack,
                                                Plasma* myPlasma,
                                                const char* delay)  {
  ComplexArrayState initDelays;
  initDelays.setState("initDelays",parent,delay);
  initDelays.initialize();
  if(initDelays.size() < 3) {
    Error::abortRun("improper initial values for delay on ComplexMatrix arc");
    return 0;
  }
  int numRows = int(initDelays[0].real());
  int numCols = int(initDelays[1].real());
  int matrixSize = numRows * numCols;

  // the number of complete Matricies that we have initial values for
  int numInitialParticles = (initDelays.size() - 2) / matrixSize;

  if(numInitialParticles < 1) {
    Error::abortRun("not enough initial values for delay on ComplexMatrix arc");
    return 0;
  }

  // create a new matrix for this particle
  ComplexMatrix *matrix = new ComplexMatrix(numRows,numCols);
  for(int k = 0; k < matrixSize; k++)
    matrix->entry(k) = initDelays[k + 2];
  Envelope p(*matrix);
  data = p;

  // create new matricies, initialize them, stuff them into Envelopes, put
  // the envelopes into MatrixEnvParticles obtained from the plasma
  for(int i = 1; i < numInitialParticles; i++) {
    ComplexMatrix *matrix = new ComplexMatrix(numRows,numCols);
    for(int k = 0; k < matrixSize; k++)
      matrix->entry(k) = initDelays[i * matrixSize + k + 2];
    Particle* p = myPlasma->get();
    *p << *matrix;
    pstack.putTail(p);
  }
  return numInitialParticles;
}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void ComplexMatrixEnvParticle::operator << (const Complex&) { 
  errorAssign("Complex"); }
void ComplexMatrixEnvParticle::operator << (double) { errorAssign("double"); }
void ComplexMatrixEnvParticle::operator << (const Fix&) { errorAssign("Fix"); }
void ComplexMatrixEnvParticle::operator << (int) { errorAssign("int"); }
void ComplexMatrixEnvParticle::operator << (FixMatrix&) { 
  errorAssign("FixMatrix"); }
void ComplexMatrixEnvParticle::operator << (FloatMatrix&) { 
  errorAssign("FloatMatrix"); }
void ComplexMatrixEnvParticle::operator << (IntMatrix&) { 
  errorAssign("IntMatrix"); }

void ComplexMatrixEnvParticle::operator << (const Envelope& p) { 
  if(!p.typeCheck("ComplexMatrix") && !p.empty())
    errorAssign(p.dataType());
  else data = p;
}

void ComplexMatrixEnvParticle::operator << (ComplexMatrix& m) { 
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
  msg << argType;
  msg << " to a ComplexMatrixEnvParticle";
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

// Initialize a given ParticleStack with the values in delay,
// obtaining other particles from the given plasma.  Returns
// the number of toal particles initialized, including this one.
// Assumes that the delay string is in the following format:
// "nrows ncols A[0][0] A[0][1] ... A[0][ncols-1] A[1][0] ... A[nrows][ncols]"
// 3/2/94 added
int FixMatrixEnvParticle::initParticleStack(Block* parent,
                                            ParticleStack& pstack,
                                            Plasma* myPlasma, 
                                            const char* delay) {
  FixArrayState initDelays;
  initDelays.setState("initDelays",parent,delay);
  initDelays.initialize();
  if(initDelays.size() < 3) {
    Error::abortRun("improper initial values for delay on FixMatrix arc");
    return 0;
  }
  int numRows = int(initDelays[0]);
  int numCols = int(initDelays[1]);
  int matrixSize = numRows * numCols;

  // the number of complete Matricies that we have initial values for
  int numInitialParticles = (initDelays.size() - 2) / matrixSize;

  if(numInitialParticles < 1) {
    Error::abortRun("not enough initial values for delay on FixMatrix arc");
    return 0;
  }

  // create a new matrix for this particle
  FixMatrix *matrix = new FixMatrix(numRows,numCols);
  for(int k = 0; k < matrixSize; k++)
    matrix->entry(k) = initDelays[k + 2];
  Envelope p(*matrix);
  data = p;

  // create new matricies, initialize them, stuff them into Envelopes, put
  // the envelopes into MatrixEnvParticles obtained from the plasma
  for(int i = 1; i < numInitialParticles; i++) {
    FixMatrix *matrix = new FixMatrix(numRows,numCols);
    for(int k = 0; k < matrixSize; k++)
      matrix->entry(k) = initDelays[i * matrixSize + k + 2];
    Particle* p = myPlasma->get();
    *p << *matrix;
    pstack.putTail(p);
  }
  return numInitialParticles;
}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void FixMatrixEnvParticle::operator << (const Complex&) { 
  errorAssign("Complex"); }
void FixMatrixEnvParticle::operator << (double) { errorAssign("double"); }
void FixMatrixEnvParticle::operator << (const Fix&) { errorAssign("Fix"); }
void FixMatrixEnvParticle::operator << (int) { errorAssign("int"); }
void FixMatrixEnvParticle::operator << (ComplexMatrix&) {
  errorAssign("ComplexMatrix"); }
void FixMatrixEnvParticle::operator << (FloatMatrix&) { 
  errorAssign("FloatMatrix"); }
void FixMatrixEnvParticle::operator << (IntMatrix&) { 
  errorAssign("IntMatrix"); }

void FixMatrixEnvParticle::operator << (const Envelope& p) { 
  if(!p.typeCheck("FixMatrix") && !p.empty())
    errorAssign(p.dataType());
  else data = p;
}

void FixMatrixEnvParticle::operator << (FixMatrix& m) { 
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
  msg << argType;
  msg << " to a FixMatrixEnvParticle";
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

// Initialize a given ParticleStack with the values in delay,
// obtaining other particles from the given plasma.  Returns
// the number of toal particles initialized, including this one.
// Assumes that the delay string is in the following format:
// "nrows ncols A[0][0] A[0][1] ... A[0][ncols-1] A[1][0] ... A[nrows][ncols]"
// 3/2/94 added
int FloatMatrixEnvParticle::initParticleStack(Block* parent,
                                              ParticleStack& pstack,
                                              Plasma* myPlasma, 
                                              const char* delay) {
                                              
  FloatArrayState initDelays;
  initDelays.setState("initDelays",parent,delay);
  initDelays.initialize();
  if(initDelays.size() < 3) {
    Error::abortRun("improper initial values for delay on FloatMatrix arc");
    return 0;
  }
  int numRows = int(initDelays[0]);
  int numCols = int(initDelays[1]);
  int matrixSize = numRows * numCols;

  // the number of complete Matricies that we have initial values for
  int numInitialParticles = (initDelays.size() - 2) / matrixSize;

  if(numInitialParticles < 1) {
    Error::abortRun("not enough initial values for delay on FloatMatrix arc");
    return 0;
  }

  // create a new matrix for this particle
  FloatMatrix *matrix = new FloatMatrix(numRows,numCols);
  for(int k = 0; k < matrixSize; k++)
    matrix->entry(k) = initDelays[k + 2];
  Envelope p(*matrix);
  data = p;

  // create new matricies, initialize them, stuff them into Envelopes, put
  // the envelopes into MatrixEnvParticles obtained from the plasma
  for(int i = 1; i < numInitialParticles; i++) {
    FloatMatrix *matrix = new FloatMatrix(numRows,numCols);
    for(int k = 0; k < matrixSize; k++)
      matrix->entry(k) = initDelays[i * matrixSize + k + 2];
    Particle* p = myPlasma->get();
    *p << *matrix;
    pstack.putTail(p);
  }
  return numInitialParticles;
}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void FloatMatrixEnvParticle::operator << (const Complex&) { 
  errorAssign("Complex"); }
void FloatMatrixEnvParticle::operator << (double) { errorAssign("double"); }
void FloatMatrixEnvParticle::operator << (const Fix&) { errorAssign("Fix"); }
void FloatMatrixEnvParticle::operator << (int) { errorAssign("int"); }
void FloatMatrixEnvParticle::operator << (ComplexMatrix&) {
  errorAssign("ComplexMatrix"); }
void FloatMatrixEnvParticle::operator << (FixMatrix&) { 
  errorAssign("FixMatrix"); }
void FloatMatrixEnvParticle::operator << (IntMatrix&) { 
  errorAssign("IntMatrix"); }

void FloatMatrixEnvParticle::operator << (const Envelope& p) { 
  if(!p.typeCheck("FloatMatrix") && !p.empty())
    errorAssign(p.dataType());
  else data = p;
}

void FloatMatrixEnvParticle::operator << (FloatMatrix& m) { 
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
  msg << argType;
  msg << " to a FloatMatrixEnvParticle";
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

// Initialize a given ParticleStack with the values in delay,
// obtaining other particles from the given plasma.  Returns
// the number of toal particles initialized, including this one.
// Assumes that the delay string is in the following format:
// "nrows ncols A[0][0] A[0][1] ... A[0][ncols-1] A[1][0] ... A[nrows][ncols]"
// 3/2/94 added
int IntMatrixEnvParticle::initParticleStack(Block* parent,
                                            ParticleStack& pstack,
                                            Plasma* myPlasma, 
                                            const char* delay) {
  IntArrayState initDelays;
  initDelays.setState("initDelays",parent,delay);
  initDelays.initialize();
  if(initDelays.size() < 3) {
    Error::abortRun("improper initial values for delay on IntMatrix arc");
    return 0;
  }
  int numRows = initDelays[0];
  int numCols = initDelays[1];
  int matrixSize = numRows * numCols;

  // the number of complete Matricies that we have initial values for
  int numInitialParticles = (initDelays.size() - 2) / matrixSize;

  if(numInitialParticles < 1) {
    Error::abortRun("not enough initial values for delay on IntMatrix arc");
    return 0;
  }

  // create a new matrix for this particle
  IntMatrix *matrix = new IntMatrix(numRows,numCols);
  for(int k = 0; k < matrixSize; k++)
    matrix->entry(k) = initDelays[k + 2];
  Envelope p(*matrix);
  data = p;

  // create new matricies, initialize them, stuff them into Envelopes, put
  // the envelopes into MatrixEnvParticles obtained from the plasma
  for(int i = 1; i < numInitialParticles; i++) {
    IntMatrix *matrix = new IntMatrix(numRows,numCols);
    for(int k = 0; k < matrixSize; k++)
      matrix->entry(k) = initDelays[i * matrixSize + k + 2];
    Particle* p = myPlasma->get();
    *p << *matrix;
    pstack.putTail(p);
  }
  return numInitialParticles;
}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void IntMatrixEnvParticle::operator << (const Complex&) { 
  errorAssign("Complex"); }
void IntMatrixEnvParticle::operator << (double) { errorAssign("double"); }
void IntMatrixEnvParticle::operator << (const Fix&) { errorAssign("Fix"); }
void IntMatrixEnvParticle::operator << (int) { errorAssign("int"); }
void IntMatrixEnvParticle::operator << (ComplexMatrix&) {
  errorAssign("ComplexMatrix"); }
void IntMatrixEnvParticle::operator << (FixMatrix&) { 
  errorAssign("FixMatrix"); }
void IntMatrixEnvParticle::operator << (FloatMatrix&) { 
  errorAssign("FloatMatrix"); }

void IntMatrixEnvParticle::operator << (const Envelope& p) { 
  if(!p.typeCheck("IntMatrix") && !p.empty())
    errorAssign(p.dataType());
  else data = p;
}

void IntMatrixEnvParticle::operator << (IntMatrix& m) { 
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
  msg << argType;
  msg << " to a IntMatrixEnvParticle";
  Error::abortRun(msg);
}

