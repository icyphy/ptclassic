#ifndef _Matrix_h
#define _Matrix_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
$Id$

Copyright (c) 199-1993 The Regents of the University of California.
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
 the Matrix Envelope classes that hold them.  Matrices store data in
 a one dimensional array, in row major form (ie. first row, then second
 row, etc).

**************************************************************************/
#include "Message.h"
#include "ComplexSubset.h"
#include "ComplexArrayState.h"
#include "Fix.h"
#include "FixArrayState.h"
#include "FloatArrayState.h"
#include "IntArrayState.h"
#include "PortHole.h"

extern const DataType COMPLEX_MATRIX_ENV;
extern const DataType FIX_MATRIX_ENV;
extern const DataType FLOAT_MATRIX_ENV;
extern const DataType INT_MATRIX_ENV;

class ComplexMatrix;
class FixMatrix;
class FloatMatrix;
class IntMatrix;

/////////////////////////////////////////////////
// Common base for derived Matrix Message classes
/////////////////////////////////////////////////
class Matrix : public Message {
 public:
  int numCols() const { return nCols; }
  int numRows() const { return nRows; }

 protected:
  int nRows;
  int nCols;
  int totalDataSize;     // number of elements
};

///////////////////////////////
//  ComplexMatrix Message Class  
///////////////////////////////

class ComplexMatrix: public Matrix {
 protected:
  Complex *data;       // made protected for SubMatrix class use
 public:
  // return data entry
  virtual Complex& entry(int element) const { return data[element]; }

  const char* dataType() const { return "ComplexMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"ComplexMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor: makes an unitialized matrix with no data
  ComplexMatrix();

  // constructor: makes an unitialized matrix with the given dimensions
  ComplexMatrix(int numRow, int numCol);

  // constructor:
  // initialized with the data given in the Particles of the PortHole
  ComplexMatrix(int numRow, int numCol, PortHole& ph);

  // constructor:
  // initialized with the data given in a data ComplexArrayState
  ComplexMatrix(int numRow, int numCol, ComplexArrayState& dataArray);

  // copy constructor
  ComplexMatrix(const ComplexMatrix& src);

  // copy constructor, copies only a submatrix of the original, as
  // specified by the starting row and col, and the number of rows and
  // cols of the submatrix.  Result is undefined if the dimensions of the
  // submatrix go beyond the dimensions of the original matrix.
  ComplexMatrix(const ComplexMatrix&, int sRow, int sCol, int nRow, int nCol);

  // clone  
  Message* clone() const { LOG_NEW; return new ComplexMatrix(*this); }
 
  // print
  StringList print() const;

  // Operators
  virtual Complex* operator [] (int row) { return &data[row*nCols]; }
  virtual const Complex* operator[] (int row) const {return &data[row*nCols]; }
  virtual int operator == (const ComplexMatrix& src);

  // cast conversion operators
  operator FixMatrix () const;
  operator FloatMatrix () const;
  operator IntMatrix () const;

  // destructive manipulations
  virtual ComplexMatrix& operator = (const ComplexMatrix& src);
  ComplexMatrix& operator = (Complex value);
  ComplexMatrix& operator += (const ComplexMatrix& src);
  ComplexMatrix& operator += (Complex value);
  ComplexMatrix& operator -= (const ComplexMatrix& src);
  ComplexMatrix& operator -= (Complex value);

  // non-destructive manipulations
  ComplexMatrix transpose() const;    // non-destructive, does not change this
  ComplexMatrix hermitian() const;    // non-destructive, does not change this
  ComplexMatrix inverse() const;      // non-destructive, does not change this

  // binary operators
  friend ComplexMatrix operator + (const ComplexMatrix&, const ComplexMatrix&);
  friend ComplexMatrix operator - (const ComplexMatrix&, const ComplexMatrix&);
  friend ComplexMatrix operator * (const ComplexMatrix&, const ComplexMatrix&);

  // destructor
  ~ComplexMatrix();
};

//////////////////////////
// FixMatrix Message Class
//////////////////////////
class FixMatrix: public Matrix {
 protected:
  Fix *data;             // made protected for SubMatrix class use
 public:
  // return data entry
  virtual Fix& entry(int element) const { return data[element]; } 

  const char* dataType() const { return "FixMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"FixMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor: makes an unitialized matrix with no data
  FixMatrix();

  // constructor: makes an unitialized matrix with the given dimensions
  FixMatrix(int numRow, int numCol);

  // constructor: 
  // initialized with the data given in the Particles of the PortHole
  FixMatrix(int numRow, int numCol, PortHole& ph);

  // constructor:
  // initialized with the data given in a data FixArrayState
  FixMatrix(int numRow, int numCol, FixArrayState& dataArray);

  // copy constructor
  FixMatrix(const FixMatrix& src);

  // copy constructor, copies only a submatrix of the original, as
  // specified by the starting row and col, and the number of rows and
  // cols of the submatrix.  Result is undefined if the dimensions of the
  // submatrix go beyond the dimensions of the original matrix.
  FixMatrix(const FixMatrix& src, int sRow, int sCol, int nRow, int nCol);

  // clone
  Message* clone() const { LOG_NEW; return new FixMatrix(*this); }
 
  // print
  StringList print() const;

  // Operators 
  virtual Fix* operator [] (int row) { return &data[row*nCols]; }
  virtual const Fix* operator [] (int row) const { return &data[row*nCols]; }
  virtual int operator == (const FixMatrix& src);

  // cast conversion operators
  operator ComplexMatrix () const;
  operator FloatMatrix () const;
  operator IntMatrix () const;

  // destructive manipulations
  virtual FixMatrix& operator = (const FixMatrix& src);
  FixMatrix& operator = (Fix value);
  FixMatrix& operator += (const FixMatrix& src);
  FixMatrix& operator += (Fix value);
  FixMatrix& operator -= (const FixMatrix& src);
  FixMatrix& operator -= (Fix value);

  // non-destructive manipulations
  FixMatrix transpose() const;     // non-destructive, does not change this
  FixMatrix inverse() const;       // non-destructive, does not change this

  // binary operators
  friend FixMatrix operator + (const FixMatrix& src1, const FixMatrix& src2);
  friend FixMatrix operator - (const FixMatrix& src1, const FixMatrix& src2);
  friend FixMatrix operator * (const FixMatrix& src1, const FixMatrix& src2);

  // destructor
  ~FixMatrix();
};

/////////////////////////////
//  FloatMatrix Message Class  
/////////////////////////////

class FloatMatrix: public Matrix {
 protected:
  double *data;             // made protected for SubMatrix class use
 public:
  // return data entry
  virtual double& entry(int element) const { return data[element]; } 

  const char* dataType() const { return "FloatMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"FloatMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor: makes an unitialized matrix with no data
  FloatMatrix();

  // constructor: makes an unitialized matrix with the given dimensions
  FloatMatrix(int numRow, int numCol);

  // constructor:
  // initialized with the data given in the Particles of the PortHole
  FloatMatrix(int numRow, int numCol, PortHole& ph);

  // constructor:
  // initialized with the data given in a data FloatArrayState
  FloatMatrix(int numRow, int numCol, FloatArrayState& dataArray);

  // copy constructor
  FloatMatrix(const FloatMatrix& src);

  // copy constructor, copies only a submatrix of the original, as
  // specified by the starting row and col, and the number of rows and
  // cols of the submatrix.  Result is undefined if the dimensions of the
  // submatrix go beyond the dimensions of the original matrix.
  FloatMatrix(const FloatMatrix& src, int sRow, int sCol, int nRow, int nCol);

  // clone  
  Message* clone() const { LOG_NEW; return new FloatMatrix(*this); }
 
  // print
  StringList print() const;

  // Operators
  virtual double* operator [] (int row) { return &data[row*nCols]; }
  virtual const double* operator[] (int row) const { return &data[row*nCols]; }
  virtual int operator == (const FloatMatrix& src);

  // cast conversion operators
  operator ComplexMatrix () const;
  operator FixMatrix () const;
  operator IntMatrix () const;

  // destructive manipulations
  virtual FloatMatrix& operator = (const FloatMatrix& src);
  FloatMatrix& operator = (float value);
  FloatMatrix& operator += (const FloatMatrix& src);
  FloatMatrix& operator += (float value);
  FloatMatrix& operator -= (const FloatMatrix& src);
  FloatMatrix& operator -= (float value);

  // non-destructive manipulations
  FloatMatrix transpose() const;      // non-destructive, does not change this
  FloatMatrix inverse() const;        // non-destructive, does not change this

  // binary operators
  friend FloatMatrix operator + (const FloatMatrix&, const FloatMatrix&);
  friend FloatMatrix operator - (const FloatMatrix&, const FloatMatrix&);
  friend FloatMatrix operator * (const FloatMatrix&, const FloatMatrix&);

  // destructor
  ~FloatMatrix();
};


//////////////////////////
// IntMatrix Message Class
//////////////////////////
class IntMatrix: public Matrix {
 protected:
  int *data;             // made protected for SubMatrix class use
 public:
  // return data entry
  virtual int& entry(int element) const { return data[element]; } 

  const char* dataType() const { return "IntMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"IntMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor: makes an unitialized matrix with no data
  IntMatrix();

  // constructor: makes an unitialized matrix with the given dimensions
  IntMatrix(int numRow, int numCol);

  // constructor: 
  // initialized with the data given in the Particles of the PortHole
  IntMatrix(int numRow, int numCol, PortHole& ph);

  // constructor:
  // initialized with the data given in a data IntArrayState
  IntMatrix(int numRow, int numCol, IntArrayState& dataArray);

  // copy constructor
  IntMatrix(const IntMatrix& src);

  // copy constructor, copies only a submatrix of the original, as
  // specified by the starting row and col, and the number of rows and
  // cols of the submatrix.  Result is undefined if the dimensions of the
  // submatrix go beyond the dimensions of the original matrix.
  IntMatrix(const IntMatrix& src, int sRow, int sCol, int nRow, int nCol);

  // clone
  Message* clone() const { LOG_NEW; return new IntMatrix(*this); }
 
  // print
  StringList print() const;

  // Operators
  virtual int* operator [] (int row) { return &data[row*nCols]; }
  virtual const int* operator [] (int row) const { return &data[row*nCols]; }
  virtual int operator == (const IntMatrix& src);

  // cast conversion operators
  operator ComplexMatrix () const;
  operator FixMatrix () const;
  operator FloatMatrix () const;

  // destructive manipulations
  virtual IntMatrix& operator = (const IntMatrix& src);
  IntMatrix& operator = (int value);
  IntMatrix& operator += (const IntMatrix& src);
  IntMatrix& operator += (int value);
  IntMatrix& operator -= (const IntMatrix& src);
  IntMatrix& operator -= (int value);

  // non-destructive manipulations
  IntMatrix transpose() const;      // non-destructive, does not change this
  IntMatrix inverse() const;        // non-destructive, does not change this

  // binary operators
  friend IntMatrix operator + (const IntMatrix& src1, const IntMatrix& src2);
  friend IntMatrix operator - (const IntMatrix& src1, const IntMatrix& src2);
  friend IntMatrix operator * (const IntMatrix& src1, const IntMatrix& src2);

  // destructor
  ~IntMatrix();
};

////////////////////////////////////////////////////////////////////////////
//  MatrixEnvParticle classes - Particles that hold the Envelopes that
//                              hold the Matrix Message classes.
////////////////////////////////////////////////////////////////////////////

#include "Particle.h"

// MatrixEnvParticle base class
class MatrixEnvParticle : public Particle {
 public:
  // constructor
  MatrixEnvParticle()  {};

  // All these conversions return as an error.
  operator int () const;
  operator float () const;
  operator double () const;
  operator Complex () const;
  operator Fix () const;

  StringList print() const;

  int errorConvert(const char*) const;

  void getMessage (Envelope& p);
  void accessMessage (Envelope& p) const;
  Particle& initialize();

  // These assignments return an error
  void operator << (const Complex& c);
  void operator << (double d);
  void operator << (const Fix& c);
  void operator << (float f);
  void operator << (int i);

  // These return an error.  Should be redined by derived classes as needed.
  virtual void operator << (const ComplexMatrix& m);
  virtual void operator << (const FixMatrix& m);
  virtual void operator << (const FloatMatrix& m);
  virtual void operator << (const IntMatrix& m);

  // compare particles
  int operator == (const Particle&);
 protected:
  virtual void errorAssign(const char*) const = 0;
  virtual DataType type() const = 0;
  Envelope data;
};

// Particle used to transmit ComplexMatrices (which are enclosed in Envelopes)
class ComplexMatrixEnvParticle : public MatrixEnvParticle { 
 public:
  DataType type() const;

  ComplexMatrixEnvParticle(const Envelope& p);
  ComplexMatrixEnvParticle();

  // load with data
  void operator << (const ComplexMatrix& m);
  void operator << (const Envelope& p);

  // particle copy
  Particle& operator = (const Particle& p);

  // clone, useNew, die analogous to other particles.
  Particle* clone() const;
  Particle* useNew() const;
  void die();

private:
  void errorAssign(const char*) const;
};

// Particle used to transmit FixMatrices (which are enclosed in Envelopes)
class FixMatrixEnvParticle : public MatrixEnvParticle {
 public:
  DataType type() const;

  FixMatrixEnvParticle(const Envelope& p);
  FixMatrixEnvParticle();

  // load with data
  void operator << (const FixMatrix& m);
  void operator << (const Envelope& p);

  // particle copy
  Particle& operator = (const Particle& p);

  // clone, useNew, die analogous to other particles.
  Particle* clone() const;
  Particle* useNew() const;
  void die();

private:
  void errorAssign(const char*) const;
};


// Particle used to transmit FloatMatrices (which are enclosed in Envelopes)
class FloatMatrixEnvParticle : public MatrixEnvParticle {
 public:
  DataType type() const;

  FloatMatrixEnvParticle(const Envelope& p);
  FloatMatrixEnvParticle();

  // load with data
  void operator << (const FloatMatrix& m);
  void operator << (const Envelope& p);

  // particle copy
  Particle& operator = (const Particle& p);

  // clone, useNew, die analogous to other particles.
  Particle* clone() const;
  Particle* useNew() const;
  void die();

private:
  void errorAssign(const char*) const;
};

// Particle used to transmit IntMatrices (which are enclosed in Envelopes)
class IntMatrixEnvParticle : public MatrixEnvParticle {
 public:
  DataType type() const;

  IntMatrixEnvParticle(const Envelope& p);
  IntMatrixEnvParticle();

  // load with data
  void operator << (const IntMatrix& m);
  void operator << (const Envelope& p);

  // particle copy
  Particle& operator = (const Particle& p);

  // clone, useNew, die analogous to other particles.
  Particle* clone() const;
  Particle* useNew() const;
  void die();

private:
  void errorAssign(const char*) const;
};

#endif