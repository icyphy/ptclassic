#ifndef _Matrix_h
#define _Matrix_h 1

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

class Galaxy;

/////////////////////////////////////////////////
// Common base for derived Matrix Message classes
/////////////////////////////////////////////////
class PtMatrix : public Message {
 public:
  int numCols() const { return nCols; }
  int numRows() const { return nRows; }

  virtual PtMatrix& operator = (const PtMatrix &) = 0;
  virtual int operator == (const PtMatrix &) const = 0;
  virtual int operator != (const PtMatrix &) const = 0;

  int totalDataSize;     // number of elements
 protected:
  int typesEqual(const PtMatrix& m) const {
    return (dataType() == m.dataType());
  }
  int compareType(const PtMatrix& m) const;

  int nRows;
  int nCols;
};

///////////////////////////////
//  ComplexMatrix Message Class  
///////////////////////////////

class ComplexMatrix: public PtMatrix {
 public:
  // return data entry
  virtual Complex& entry(int element) const { return data[element]; }

  const char* dataType() const { return "ComplexMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"ComplexMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor: makes an un-initialized matrix with no data
  ComplexMatrix();

  // constructor: makes an un-initialized matrix with the given dimensions
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
  virtual int operator == (const PtMatrix& src) const;
  virtual int operator != (const PtMatrix& src) const {return !(*this == src); }

  // cast conversion operators
  operator FixMatrix () const;
  operator FloatMatrix () const;
  operator IntMatrix () const;

  // destructive replacement operators 
     // assignment
  virtual PtMatrix& operator = (const PtMatrix& src);
  ComplexMatrix& operator = (const Complex& value);          
     // element wise operators
  ComplexMatrix& operator += (const ComplexMatrix& src);
  ComplexMatrix& operator += (const Complex& value);
  ComplexMatrix& operator -= (const ComplexMatrix& src);
  ComplexMatrix& operator -= (const Complex& value);
  ComplexMatrix& operator *= (const ComplexMatrix& B); // note: element-wise *
  ComplexMatrix& operator *= (const Complex& value);
  ComplexMatrix& operator /= (const ComplexMatrix& src);
  ComplexMatrix& operator /= (const Complex& value);
     // make this an identity matrix
  ComplexMatrix& identity();  

  // non-destructive operators, returns a new PtMatrix
     // prefix unary operators
  ComplexMatrix operator- () const;
  ComplexMatrix operator~ () const { return transpose(); }
  ComplexMatrix operator! () const { return inverse(); }
     // binary operator
  ComplexMatrix operator^ (int exponent) const;  // matrix to a power
     // functions
  ComplexMatrix transpose() const;
  ComplexMatrix conjugate() const;                 // complex conjugate
  ComplexMatrix hermitian() const;                 // conjugate transpose
  ComplexMatrix inverse() const;

  // non-member operators
     // binary operators
  friend ComplexMatrix operator + (const ComplexMatrix&, const ComplexMatrix&);
  friend ComplexMatrix operator + (const Complex&, const ComplexMatrix&);
  friend ComplexMatrix operator + (const ComplexMatrix&, const Complex&);
  friend ComplexMatrix operator - (const ComplexMatrix&, const ComplexMatrix&);
  friend ComplexMatrix operator - (const Complex&, const ComplexMatrix&);
  friend ComplexMatrix operator - (const ComplexMatrix&, const Complex&);
  friend ComplexMatrix operator * (const ComplexMatrix&, const ComplexMatrix&);
  friend ComplexMatrix operator * (const Complex&, const ComplexMatrix&);
  friend ComplexMatrix operator * (const ComplexMatrix&, const Complex&);
     // faster ternary function, avoids extra copying step
  friend ComplexMatrix& multiply (const ComplexMatrix& left, 
                                  const ComplexMatrix& right,
                                  ComplexMatrix& result);

  // destructor
  ~ComplexMatrix();
 protected:
  Complex *data;       // made protected for SubMatrix class use
};

//////////////////////////
// FixMatrix Message Class
//////////////////////////
class FixMatrix: public PtMatrix {
 public:
  // return data entry
  virtual Fix& entry(int element) const { return data[element]; } 

  const char* dataType() const { return "FixMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"FixMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor: makes an un-initialized matrix with no data
  FixMatrix();

  // constructor: makes an un-initialized matrix with the given dimensions
  //   using the default Fix precision as specified in the Fix class
  FixMatrix(int numRow, int numCol);

  // constructor: makes an un-initialized matrix with the given dimensions
  //   using Fix elements of the given length "ln" and integer bits "ib"
  FixMatrix(int numRow, int numCol, int ln, int ib);

  // constructor: 
  // initialized with the data given in the Particles of the PortHole
  //   using the default Fix precision as specified in the Fix class
  FixMatrix(int numRow, int numCol, PortHole& ph);

  // constructor: 
  // initialized with the data given in the Particles of the PortHole
  //   using Fix elements of the given length "ln" and integer bits "ib"
  FixMatrix(int numRow, int numCol, int ln, int ib, PortHole& ph);

  // constructor:
  // initialized with the data given in a data FixArrayState
  //   using the default Fix precision as specified in the Fix class
  FixMatrix(int numRow, int numCol, FixArrayState& dataArray);

  // constructor:
  // initialized with the data given in a data FixArrayState
  //   using Fix elements of the given length "ln" and integer bits "ib"
  FixMatrix(int numRow, int numCol, int ln, int ib, FixArrayState& dataArray);

  // copy constructor
  FixMatrix(const FixMatrix& src);
  // special copy constructors with precision and masking
  FixMatrix(const ComplexMatrix& src, int ln, int ib, int round);
  FixMatrix(const FloatMatrix& src, int ln, int ib, int round);
  FixMatrix(const IntMatrix& src, int ln, int ib, int round);

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
  virtual int operator == (const PtMatrix& src) const;
  virtual int operator != (const PtMatrix& src) const {return !(*this == src); }

  // cast conversion operators
  operator ComplexMatrix () const;
  operator FloatMatrix () const;
  operator IntMatrix () const;

  // destructive replacement operators 
     // assignment
  virtual PtMatrix& operator = (const PtMatrix& src);
  FixMatrix& operator = (const Fix& value);
     // element wise operators
  FixMatrix& operator += (const FixMatrix& src);
  FixMatrix& operator += (const Fix& value);
  FixMatrix& operator -= (const FixMatrix& src);
  FixMatrix& operator -= (const Fix& value);
  FixMatrix& operator *= (const FixMatrix& B); // note: element-wise *
  FixMatrix& operator *= (const Fix& value);
  FixMatrix& operator /= (const FixMatrix& src);
  FixMatrix& operator /= (const Fix& value);
     // make this an identity matrix
  FixMatrix& identity();  

  // non-destructive operators, returns a new PtMatrix
     // prefix unary operators
  FixMatrix operator- () const ;
  FixMatrix operator~ () const { return transpose(); }
  FixMatrix operator! () const { return inverse(); }
     // binary operator
  FixMatrix operator^ (int exponent) const;  // matrix to a power
     // functions
  FixMatrix transpose() const;     // non-destructive, does not change this
  FixMatrix inverse() const;       // non-destructive, does not change this

  // binary operators
  friend FixMatrix operator + (const FixMatrix&, const FixMatrix&);
  friend FixMatrix operator + (const Fix&, const FixMatrix&);
  friend FixMatrix operator + (const FixMatrix&, const Fix&);
  friend FixMatrix operator - (const FixMatrix&, const FixMatrix&);
  friend FixMatrix operator - (const Fix&, const FixMatrix&);
  friend FixMatrix operator - (const FixMatrix&, const Fix&);
  friend FixMatrix operator * (const FixMatrix&, const FixMatrix&);
  friend FixMatrix operator * (const Fix&, const FixMatrix&);
  friend FixMatrix operator * (const FixMatrix&, const Fix&);
     // faster ternary function, avoids extra copying step
  friend FixMatrix& multiply (const FixMatrix& left, const FixMatrix& right,
                              FixMatrix& result);

  // destructor
  ~FixMatrix();
 protected:
  Fix *data;             // made protected for SubMatrix class use
};

/////////////////////////////
//  FloatMatrix Message Class  
/////////////////////////////

class FloatMatrix: public PtMatrix {
 public:
  // return data entry
  virtual double& entry(int element) const { return data[element]; } 

  const char* dataType() const { return "FloatMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"FloatMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor: makes an un-initialized matrix with no data
  FloatMatrix();

  // constructor: makes an un-initialized matrix with the given dimensions
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
  virtual int operator == (const PtMatrix& src) const;
  virtual int operator != (const PtMatrix& src) const {return !(*this == src); }

  // cast conversion operators
  operator ComplexMatrix () const;
  operator FixMatrix () const;
  operator IntMatrix () const;

  // destructive replacement operators 
     // assignment
  virtual PtMatrix& operator = (const PtMatrix& src);
  FloatMatrix& operator = (double value);
     // element wise operators
  FloatMatrix& operator += (const FloatMatrix& src);
  FloatMatrix& operator += (double value);
  FloatMatrix& operator -= (const FloatMatrix& src);
  FloatMatrix& operator -= (double value);
  FloatMatrix& operator *= (const FloatMatrix& B); // note: element-wise *
  FloatMatrix& operator *= (double value);
  FloatMatrix& operator /= (const FloatMatrix& src);
  FloatMatrix& operator /= (double value);
     // make this an identity matrix
  FloatMatrix& identity();  

  // non-destructive operators, returns a new PtMatrix
     // prefix unary operators
  FloatMatrix operator- () const;
  FloatMatrix operator~ () const { return transpose(); }
  FloatMatrix operator! () const { return inverse(); }
     // binary operator
  FloatMatrix operator^ (int exponent) const;  // matrix to a power
     // functions
  FloatMatrix transpose() const;     // non-destructive, does not change this
  FloatMatrix inverse() const;       // non-destructive, does not change this

  // binary operators
  friend FloatMatrix operator + (const FloatMatrix&, const FloatMatrix&);
  friend FloatMatrix operator + (double, const FloatMatrix&);
  friend FloatMatrix operator + (const FloatMatrix&, double);
  friend FloatMatrix operator - (const FloatMatrix&, const FloatMatrix&);
  friend FloatMatrix operator - (double, const FloatMatrix&);
  friend FloatMatrix operator - (const FloatMatrix&, double);
  friend FloatMatrix operator * (const FloatMatrix&, const FloatMatrix&);
  friend FloatMatrix operator * (double, const FloatMatrix&);
  friend FloatMatrix operator * (const FloatMatrix&, double);
     // faster ternary function, avoids extra copying step
  friend FloatMatrix& multiply (const FloatMatrix& left, 
                                const FloatMatrix& right, FloatMatrix& result);

  // destructor
  ~FloatMatrix();
 protected:
  double *data;             // made protected for SubMatrix class use
};


//////////////////////////
// IntMatrix Message Class
//////////////////////////
class IntMatrix: public PtMatrix {
 public:
  // return data entry
  virtual int& entry(int element) const { return data[element]; } 

  const char* dataType() const { return "IntMatrix"; }

  int isA(const char* typ) const {
    if(strcmp(typ,"IntMatrix") == 0) return TRUE;
    else return Message::isA(typ);
  }

  // constructor: makes an un-initialized matrix with no data
  IntMatrix();

  // constructor: makes an un-initialized matrix with the given dimensions
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
  virtual int operator == (const PtMatrix& src) const;
  virtual int operator != (const PtMatrix& src) const {return !(*this == src); }

  // cast conversion operators
  operator ComplexMatrix () const;
  operator FixMatrix () const;
  operator FloatMatrix () const;

  // destructive replacement operators 
     // assignment
  virtual PtMatrix& operator = (const PtMatrix& src);
  IntMatrix& operator = (int value);
     // element wise operators
  IntMatrix& operator += (const IntMatrix& src);
  IntMatrix& operator += (int value);
  IntMatrix& operator -= (const IntMatrix& src);
  IntMatrix& operator -= (int value);
  IntMatrix& operator *= (const IntMatrix& B); // note: element-wise *
  IntMatrix& operator *= (int value);
  IntMatrix& operator /= (const IntMatrix& src);
  IntMatrix& operator /= (int value);
     // make this an identity matrix
  IntMatrix& identity();  

  // non-destructive operators, returns a new PtMatrix
     // prefix unary operators
  IntMatrix operator- () const;
  IntMatrix operator~ () const { return transpose(); }
  IntMatrix operator! () const { return inverse(); }
     // binary operator
  IntMatrix operator^ (int exponent) const;  // matrix to a power
     // functions
  IntMatrix transpose() const;     // non-destructive, does not change this
  IntMatrix inverse() const;       // non-destructive, does not change this

  // binary operators
  friend IntMatrix operator + (const IntMatrix&, const IntMatrix&);
  friend IntMatrix operator + (int, const IntMatrix&);
  friend IntMatrix operator + (const IntMatrix&, int);
  friend IntMatrix operator - (const IntMatrix&, const IntMatrix&);
  friend IntMatrix operator - (int, const IntMatrix&);
  friend IntMatrix operator - (const IntMatrix&, int);
  friend IntMatrix operator * (const IntMatrix&, const IntMatrix&);
  friend IntMatrix operator * (int, const IntMatrix&);
  friend IntMatrix operator * (const IntMatrix&, int);
     // faster ternary function, avoids extra copying step
  friend IntMatrix& multiply (const IntMatrix& left, const IntMatrix& right,
                              IntMatrix& result);

  // destructor
  ~IntMatrix();
 protected:
  int *data;             // made protected for SubMatrix class use
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
  /*virtual*/ Particle& initialize();

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

  // Initialize a given ParticleStack with the values in the delay string,
  // obtaining other Particles from the given Plasma.  Returns the
  // number of total Particles initialized, including this one.
  // 3/2/94 added
  /*virtual*/ int initParticleStack(Block* parent, ParticleStack& pstack,
                                    Plasma* myPlasma, const char* delay = 0);

  // load with data
  // These assignments return an error
  virtual void operator << (int i);
  virtual void operator << (double d);
  virtual void operator << (const Complex& c);
  virtual void operator << (const Fix& c);
  virtual void operator << (FixMatrix& m);
  virtual void operator << (FloatMatrix& m);
  virtual void operator << (IntMatrix& m);

  virtual void operator << (ComplexMatrix& m);
  virtual void operator << (const Envelope& p);

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

  // Initialize a given ParticleStack with the values in the delay string,
  // obtaining other Particles from the given Pplasma.  Returns then
  // number of total Particles initialized, including this one.
  // 3/2/94 added
  /*virtual*/ int initParticleStack(Block* parent, ParticleStack& pstack,
                                    Plasma* myPlasma, const char* delay = 0);

  // load with data
  // These assignments return an error
  virtual void operator << (int i);
  virtual void operator << (double d);
  virtual void operator << (const Complex& c);
  virtual void operator << (const Fix& c);
  virtual void operator << (ComplexMatrix& m);
  virtual void operator << (FloatMatrix& m);
  virtual void operator << (IntMatrix& m);

  virtual void operator << (FixMatrix& m);
  virtual void operator << (const Envelope& p);

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

  // Initialize a given ParticleStack with the values in the delay string,
  // obtaining other Particles from the given Plasma.  Returns the
  // number of total Particles initialized, including this one.
  // 3/2/94 added
  /*virtual*/ int initParticleStack(Block* parent, ParticleStack& pstack,
                                    Plasma* myPlasma, const char* delay = 0);
                                    
  // load with data
  // These assignments return an error
  virtual void operator << (int i);
  virtual void operator << (double d);
  virtual void operator << (const Complex& c);
  virtual void operator << (const Fix& c);
  virtual void operator << (ComplexMatrix& m);
  virtual void operator << (FixMatrix& m);
  virtual void operator << (IntMatrix& m);

  virtual void operator << (FloatMatrix& m);
  virtual void operator << (const Envelope& p);

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

  // Initialize a given ParticleStack with the values in the delay string,
  // obtaining other Particles from the given Plasma.  Returns the
  // number of total Particles initialized, including this one.
  // 3/2/94 added
  /*virtual*/ int initParticleStack(Block* parent, ParticleStack& pstack,
                                    Plasma* myPlasma, const char* delay = 0);

  // load with data
  // These assignments return an error
  virtual void operator << (int i);
  virtual void operator << (double d);
  virtual void operator << (const Complex& c);
  virtual void operator << (const Fix& c);
  virtual void operator << (ComplexMatrix& m);
  virtual void operator << (FixMatrix& m);
  virtual void operator << (FloatMatrix& m);

  virtual void operator << (IntMatrix& m);
  virtual void operator << (const Envelope& p);

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
