#ifndef _SubMatrix_h
#define _SubMatrix_h 1

// SubMatrix class.  Uses reference to parent matrix so no memory allocation
// to store data.

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
#include "Matrix.h"

///////////////////////////
//  ComplexSubMatrix Class  
///////////////////////////

class ComplexSubMatrix: public ComplexMatrix {
 protected:
  ComplexMatrix *parent;
 public:
  // Serial access to the data, treat the data as an array.
  /* virtual */ Complex& entry(int element) const { 
    return data[element/nCols*parent->numCols()+element%nCols]; }

  const char* dataType() const { return "ComplexSubMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"ComplexSubMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor, makes uninitialized structure;
  ComplexSubMatrix() {}

  // constructor, given just the number of rows and columns.
  // this allocates actual memory, like ComplexMatrix
  ComplexSubMatrix(int nRow, int nCol);

  // constructor, given a sibling matrix (giving us access to the parent),
  // specified by the starting row and col, and the number of rows and
  // cols of the submatrix.  Result is undefined if the dimensions of the
  // submatrix go beyond the dimensions of the original matrix.
  ComplexSubMatrix(ComplexSubMatrix& src,int sRow,int sCol,int nRow,int nCol);

  // Copy constructor
  ComplexSubMatrix(const ComplexSubMatrix& src);

  // Destructor, set data pointer to zero so that ~FloatMatrix() won't
  // delete the parent's data.
  ~ComplexSubMatrix() { data = 0; }

  // clone
  Message* clone() const { LOG_NEW; return new ComplexSubMatrix(*this); }
 
  // print
  StringList print() const;

  // General operators 
  /*virtual*/ PtMatrix& operator = (const PtMatrix& src);
  // Necessary for gcc-2.6.xx -Wsynth
  ComplexSubMatrix& operator = (const ComplexSubMatrix& src);
  ComplexSubMatrix& operator = (Complex value);
  /*virtual*/ int operator == (const PtMatrix& src);
  /*virtual*/ int operator != (const PtMatrix& src) { return(!(*this == src)); }
  /*virtual*/ Complex* operator [] (int row) { 
                                       return &data[row*parent->numCols()]; }
  /*virtual*/ const Complex* operator[] (int row) const {
                                       return &data[row*parent->numCols()]; }
  void operator << (ComplexArrayState& src);

  // delete the parent
  void killParent() { delete parent; }
};

///////////////////////
// FixSubMatrix Class
///////////////////////
class FixSubMatrix: public FixMatrix {
 protected:
  FixMatrix *parent;
 public:
  // return data entry
  /* virtual */  Fix& entry(int element) const { 
    return data[element/nCols*parent->numCols()+element%nCols]; }

  const char* dataType() const { return "FixSubMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"FixSubMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor, makes uninitialized structure;
  FixSubMatrix() {}

  // constructor, given just the number of rows and columns.
  // this allocates actual memory, like FixMatrix
  FixSubMatrix(int nRow, int nCol);

  // constructor, given a sibling matrix (giving us access to the parent),
  // specified by the starting row and col, and the number of rows and
  // cols of the submatrix.  Result is undefined if the dimensions of the
  // submatrix go beyond the dimensions of the original matrix.
  FixSubMatrix(FixSubMatrix& src,int sRow,int sCol,int nRow,int nCol);

  // Copy constructor
  FixSubMatrix(const FixSubMatrix& src);

  // Destructor, set data pointer to zero so that ~FloatMatrix() won't
  // delete the parent's data.
  ~FixSubMatrix() { data = 0; }

  // clone
  Message* clone() const { LOG_NEW; return new FixSubMatrix(*this); }
 
  // print
  StringList print() const;

  // General operators 
  /*virtual*/ PtMatrix& operator = (const PtMatrix& src);
  // Necessary for gcc-2.6.xx -Wsynth
  FixSubMatrix& operator = (const FixSubMatrix& src);
  FixSubMatrix& operator = (Fix value);
  /*virtual*/ int operator == (const PtMatrix& src);
  /*virtual*/ int operator != (const PtMatrix& src) { return(!(*this == src)); }
  /*virtual*/ Fix* operator [] (int row) { 
                                        return &data[row*parent->numCols()]; }
  /*virtual*/ const Fix* operator[] (int row) const {
                                        return &data[row*parent->numCols()]; }

  void operator << (FixArrayState& src);

  // delete the parent 
  void killParent() { delete parent; }
};

///////////////////////////
//  FloatSubMatrix Class  
///////////////////////////

class FloatSubMatrix: public FloatMatrix {
 protected:
  FloatMatrix *parent;
 public:
  // Serial access to the data, treat the data as an array.
  /* virtual */ double& entry(int element) const { 
    return data[element/nCols*parent->numCols()+element%nCols]; }

  const char* dataType() const { return "FloatSubMatrix"; }
  int isA(const char* typ) const {
    if(strcmp(typ,"FloatSubMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor, makes uninitialized structure;
  FloatSubMatrix() {}

  // constructor, given just the number of rows and columns.
  // this allocates actual memory, like FloatMatrix
  FloatSubMatrix(int nRow, int nCol);

  // constructor, given a sibling matrix (giving us access to the parent),
  // specified by the starting row and col, and the number of rows and
  // cols of the submatrix.  Result is undefined if the dimensions of the
  // submatrix go beyond the dimensions of the original matrix.
  FloatSubMatrix(FloatSubMatrix& src,int sRow,int sCol,int nRow,int nCol);

  // Copy constructor
  FloatSubMatrix(const FloatSubMatrix& src);

  // Destructor, set data pointer to zero so that ~FloatMatrix() won't
  // delete the parent's data.
  ~FloatSubMatrix() { data = 0; }

  // clone
  Message* clone() const { LOG_NEW; return new FloatSubMatrix(*this); }
 
  // print
  StringList print() const;

  // General operators 
  /*virtual*/ PtMatrix& operator = (const PtMatrix& src);
  // Necessary for gcc-2.6.xx -Wsynth
  FloatSubMatrix& operator = (const FloatSubMatrix& src);
  FloatSubMatrix& operator = (double value);
  /*virtual*/ int operator == (const PtMatrix& src);
  /*virtual*/ int operator != (const PtMatrix& src) { return(!(*this == src)); }
  /*virtual*/ double* operator [] (int row) { 
                                      return &data[row*parent->numCols()]; }
  /*virtual*/ const double* operator[] (int row) const { 
                                      return &data[row*parent->numCols()]; }
  void operator << (FloatArrayState& src);

  // delete the parent 
  void killParent() { delete parent; }
};

///////////////////////
// IntSubMatrix Class
///////////////////////
class IntSubMatrix: public IntMatrix {
 protected:
  IntMatrix *parent;
 public:
  // return data entry
  /* virtual */ int& entry(int element) const { 
    return data[element/nCols*parent->numCols()+element%nCols]; }

  const char* dataType() const { return "IntSubMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"IntSubMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor, makes uninitialized structure;
  IntSubMatrix() {}

  // constructor, given just the number of rows and columns.
  // this allocates actual memory, like IntMatrix
  IntSubMatrix(int nRow, int nCol);

  // constructor, given a sibling matrix (giving us access to the parent),
  // specified by the starting row and col, and the number of rows and
  // cols of the submatrix.  Result is undefined if the dimensions of the
  // submatrix go beyond the dimensions of the original matrix.
  IntSubMatrix(IntSubMatrix& src,int sRow,int sCol,int nRow,int nCol);

  // Copy constructor
  IntSubMatrix(const IntSubMatrix& src);

  // Destructor, set data pointer to zero so that ~IntMatrix() won't
  // delete the parent's data.
  ~IntSubMatrix() { data = 0; }

  // clone
  Message* clone() const { LOG_NEW; return new IntSubMatrix(*this); }
 
  // print
  StringList print() const;

  // General operators 
  /*virtual*/ PtMatrix& operator = (const PtMatrix& src);
  // Necessary for gcc-2.6.xx -Wsynth
  IntSubMatrix& operator = (const IntSubMatrix& src);
  IntSubMatrix& operator = (int value);
  /*virtual*/ int operator == (const PtMatrix& src);
  /*virtual*/ int operator != (const PtMatrix& src) { return(!(*this == src)); }
  /*virtual*/ int* operator [] (int row) { return &data[row*parent->numCols()]; }
  /*virtual*/ const int* operator[] (int row) const {
                                             return &data[row*parent->numCols()]; }

  void operator << (IntArrayState& src);

  // delete the parent 
  void killParent() { delete parent; }
};


#endif
