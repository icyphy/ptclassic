static const char file_id[] = "MatrixParticle.cc";
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

 This file defines the implementation of the MatrixParticle classes.
 These are not full Particles in the normal sense, but act as a
 source of SubMatrices for the MDSDF domain.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif
#include "MatrixParticle.h"
#include "Plasma.h"
#include "Error.h"

/***********************************************************************
 MatrixParticles, used to hold the actual matrices.
***********************************************************************/
extern const DataType COMPLEX_MATRIX = "COMPLEX_MATRIX";
extern const DataType FIX_MATRIX = "FIX_MATRIX";
extern const DataType FLOAT_MATRIX = "FLOAT_MATRIX";
extern const DataType INT_MATRIX = "INT_MATRIX";

/////////////////////////////
// MatrixParticle Base Class
/////////////////////////////

MatrixParticle::operator int() const { return errorConvert("int");}
MatrixParticle::operator double() const { return errorConvert("double"); }
MatrixParticle::operator float() const { return errorConvert("float"); }
MatrixParticle::operator Complex() const { return errorConvert("complex");}
MatrixParticle::operator Fix () const { 
  Fix dummy;
  errorConvert("fix");
  return dummy;
}
MatrixParticle::operator ComplexSubMatrix* () const {
  errorConvert("ComplexSubMatrix");
  return NULL;
}
MatrixParticle::operator FixSubMatrix* () const {
  errorConvert("FixSubMatrix");
  return NULL;
}
MatrixParticle::operator FloatSubMatrix* () const { 
  errorConvert("FloatSubMatrix");
  return NULL;
}
MatrixParticle::operator IntSubMatrix* () const { 
  errorConvert("IntSubMatrix");
  return NULL;
}

int MatrixParticle::errorConvert(const char* arg) const {
  StringList msg = "Message type, '";
  msg += type();
  msg += "': invalid conversion to ";
  msg += arg;
  Error::abortRun(msg);
  return 0;
}

//////////////////////////////////
// ComplexMatrixParticle Class
//////////////////////////////////

static ComplexMatrixParticle cmpproto;
static Plasma ComplexMatrixPlasma(cmpproto);
static PlasmaGate ComplexMatrixGate(ComplexMatrixPlasma);

// ComplexMatrixParticle methods

DataType ComplexMatrixParticle::type() const { return COMPLEX_MATRIX;}

ComplexMatrixParticle::ComplexMatrixParticle() : data(0) {}

ComplexMatrixParticle::operator ComplexSubMatrix* () const { 
  return data; }

// Setup the data field.  Should only be called once.
void ComplexMatrixParticle::initMatrix(int nRows, int nCols, int rowDelays,
				       int colDelays) {
  LOG_NEW; data = new ComplexSubMatrix(nRows,nCols);
  int row, col;
  for(row = 0; row < rowDelays; row++)
    for(col = 0; col < nCols; col++)
      (*data)[row][col] = Complex(0,0);
  for(row = rowDelays; row < nRows; row++)
    for(col = 0; col < colDelays; col++)
      (*data)[row][col] = Complex(0,0);
}

// Set the data field to the given SubMatrix
void ComplexMatrixParticle::initialize(PtMatrix *m, MatrixParticle *p) {
  // should we check the type?
  if(data) {
    LOG_DEL; delete data;
  }
  data = (ComplexSubMatrix*)m;
  parent = p;
}

// Reset the data and parent pointers to NULL
Particle& ComplexMatrixParticle::initialize() {
  if(data) {
    LOG_DEL; delete data;
  }
  data = NULL;
  return *this; 
}

StringList ComplexMatrixParticle::print() const { return data->print(); }

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void ComplexMatrixParticle::operator << (const FixMatrix&) {
  errorAssign("FixMatrix"); }
void ComplexMatrixParticle::operator << (const FloatMatrix&) { 
  errorAssign("FloatMatrix"); }
void ComplexMatrixParticle::operator << (const IntMatrix&) {
  errorAssign("IntMatrix"); }

void ComplexMatrixParticle::operator << (int i) { 
  (ComplexMatrix)*data = Complex(i); }
void ComplexMatrixParticle::operator << (double f) { 
  (ComplexMatrix)*data = Complex(f); }
void ComplexMatrixParticle::operator << (const Complex& c) { 
  (ComplexMatrix)*data = c;}
void ComplexMatrixParticle::operator << (const Fix& x) { 
  (ComplexMatrix)*data = Complex(double(x)); }
void ComplexMatrixParticle::operator << (const ComplexMatrix& m) { 
  (ComplexMatrix)*data = m; }

// Create a subMatrix using the data as the parent.  Takes
// the starting row and col, and the size of the submatrix desired.
PtMatrix* ComplexMatrixParticle::subMatrix(int startRow, int startCol,
					 int numRows, int numCols) {
  return (new ComplexSubMatrix(*data,startRow,startCol,numRows,numCols));
}

// particle copy, this->data needs to be allocated.
Particle& ComplexMatrixParticle::operator = (const Particle& p) {
  if (compareType(p)) {
    *data = *(ComplexSubMatrix*)p;
  }
  return *this;
}

// particle compare
int ComplexMatrixParticle :: operator == (const Particle& p) {
  if (!typesEqual(p)) return 0;
  return *data == *(ComplexSubMatrix*)p;
}

// clone, useNew, die analogous to other particles.

Particle* ComplexMatrixParticle::clone() const {
  Particle * p = ComplexMatrixPlasma.get();
  *(((ComplexMatrixParticle*)p)->data) = *data;
  return p;
}

Particle* ComplexMatrixParticle::useNew() const {
  LOG_NEW; return new ComplexMatrixParticle;
}

// recycle this particle
void ComplexMatrixParticle::die() {
  // if no parent, then we have the original copy, kill it 
  if(!parent)
    data->killParent();
  delete data;
  data = NULL; parent = NULL;
  ComplexMatrixPlasma.put(this);
}

void ComplexMatrixParticle::errorAssign(const char* argType) const {
  StringList msg = "Attempt to assign type ";
  msg += argType;
  msg += " to a ComplexMatrixParticle";
  Error::abortRun(msg);
}


//////////////////////////////
// FixMatrixParticle Class
//////////////////////////////

static FixMatrixParticle fixmpproto;
static Plasma FixMatrixPlasma(fixmpproto);
static PlasmaGate FixMatrixGate(FixMatrixPlasma);

// FixMatrixParticle methods

DataType FixMatrixParticle::type() const { return FIX_MATRIX;}

FixMatrixParticle::FixMatrixParticle() : data(0) {}

FixMatrixParticle::operator FixSubMatrix* () const { 
  return data; }

// Setup the data field.  Should only be called once.
void FixMatrixParticle::initMatrix(int nRows, int nCols, int rowDelays,
				   int colDelays) {
  LOG_NEW; data = new FixSubMatrix(nRows,nCols);
  int row, col;
  for(row = 0; row < rowDelays; row++)
    for(col = 0; col < nCols; col++)
      (*data)[row][col] = Fix(0.0);
  for(row = rowDelays; row < nRows; row++)
    for(col = 0; col < colDelays; col++)
      (*data)[row][col] = Fix(0.0);
}

// Set the data field to the given SubMatrix and the pointer to the
// MatrixParticle containing the parent matrix
void FixMatrixParticle::initialize(PtMatrix *m, MatrixParticle* p) {
  // should we check the type?
  if(data) {
    parent->die();
    LOG_DEL; delete data;
  }
  data = (FixSubMatrix*)m;
  parent = p;
}

// Reset the data and parent pointers to NULL
Particle& FixMatrixParticle::initialize() {
  if(data) {
    parent->die();
    LOG_DEL; delete data;
  }
  data = NULL;
  parent = NULL;
  return *this; 
}

StringList FixMatrixParticle::print() const { return data->print(); }

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void FixMatrixParticle::operator << (const ComplexMatrix&) { 
  errorAssign("ComplexMatrix"); }
void FixMatrixParticle::operator << (const FloatMatrix&) { 
  errorAssign("FloatMatrix"); }
void FixMatrixParticle::operator << (const IntMatrix&) {
  errorAssign("IntMatrix"); }

//void FixMatrixParticle::operator << (int i) { 
//  (FixMatrix)*data = Fix(i); }
void FixMatrixParticle::operator << (int i) { 
  (FixMatrix)*data = Fix(double(i)); }
void FixMatrixParticle::operator << (double f) { 
  (FixMatrix)*data = Fix(f); }
void FixMatrixParticle::operator << (const Complex& c) { 
  (FixMatrix)*data = Fix(abs(c));}
void FixMatrixParticle::operator << (const Fix& x) { 
  (FixMatrix)*data = x; }
void FixMatrixParticle::operator << (const FixMatrix& m) { 
  (FixMatrix)*data = m; }

// Create a subMatrix using the data as the parent.  Takes
// the starting row and col, and the size of the submatrix desired.
// Assumes the data for this is of type Matrix and not SubMatrix.
PtMatrix* FixMatrixParticle::subMatrix(int startRow, int startCol,
				     int numRows, int numCols) {
  return (new FixSubMatrix(*data,startRow,startCol,numRows,numCols));
}

// particle copy, this->data needs to be allocated.
Particle& FixMatrixParticle::operator = (const Particle& p) {
  if (compareType(p)) {
    *data = *(FixSubMatrix*)p;
  }
  return *this;
}

// particle compare
int FixMatrixParticle :: operator == (const Particle& p) {
  if (!typesEqual(p)) return 0;
  return *data == *(FixSubMatrix*)p;
}

// clone, useNew, die analogous to other particles.

Particle* FixMatrixParticle::clone() const {
  Particle * p = FixMatrixPlasma.get();
  *(((FixMatrixParticle*)p)->data) = *data;
  return p;
}

Particle* FixMatrixParticle::useNew() const {
  LOG_NEW; return new FixMatrixParticle;
}

// recycle this particle
void FixMatrixParticle::die() {
  // if no parent, then we have the original copy, kill it 
  if(!parent)
    data->killParent();
  delete data;
  data = NULL; parent = NULL;
  FixMatrixPlasma.put(this);
}

void FixMatrixParticle::errorAssign(const char* argType) const {
  StringList msg = "Attempt to assign type ";
  msg += argType;
  msg += " to a FixMatrixParticle";
  Error::abortRun(msg);
}

///////////////////////////////
// FloatMatrixParticle Class
///////////////////////////////

static FloatMatrixParticle fmpproto;
static Plasma FloatMatrixPlasma(fmpproto);
static PlasmaGate FloatMatrixGate(FloatMatrixPlasma);

// FloatMatrixParticle methods

DataType FloatMatrixParticle::type() const { return FLOAT_MATRIX;}

FloatMatrixParticle::FloatMatrixParticle() : data(0) {}

FloatMatrixParticle::operator FloatSubMatrix* () const { 
  return data; }

// Setup the data field.  Should only be called once.
void FloatMatrixParticle::initMatrix(int nRows, int nCols, int rowDelays,
				     int colDelays) {
  LOG_NEW; data = new FloatSubMatrix(nRows,nCols);
  int row, col;
  for(row = 0; row < rowDelays; row++)
    for(col = 0; col < nCols; col++)
      (*data)[row][col] = 0.0;
  for(row = rowDelays; row < nRows; row++)
    for(col = 0; col < colDelays; col++)
      (*data)[row][col] = 0.0;
}

// Set the data field to the given SubMatrix
void FloatMatrixParticle::initialize(PtMatrix *m, MatrixParticle *p) {
  // should we check the type?
  if(data) {
    parent->die();
    LOG_DEL; delete data;
  }
  data = (FloatSubMatrix*)m;
  parent = p;
}

// Reset the data and parent pointers to NULL
Particle& FloatMatrixParticle::initialize() {
  if(data) {
    parent->die();
    LOG_DEL; delete data;
  }
  data = NULL;
  parent = NULL;
  return *this; 
}

StringList FloatMatrixParticle::print() const { return data->print(); }

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void FloatMatrixParticle::operator << (const ComplexMatrix&) { 
  errorAssign("ComplexMatrix"); }
void FloatMatrixParticle::operator << (const FixMatrix&) {
  errorAssign("FixMatrix"); }
void FloatMatrixParticle::operator << (const IntMatrix&) {
  errorAssign("IntMatrix"); }

void FloatMatrixParticle::operator << (int i) { 
  (FloatMatrix)*data = i; }
void FloatMatrixParticle::operator << (double f) { 
  (FloatMatrix)*data = f; }
void FloatMatrixParticle::operator << (const Complex& c) { 
  (FloatMatrix)*data = abs(c);}
void FloatMatrixParticle::operator << (const Fix& x) { 
  (FloatMatrix)*data = double(x); }
void FloatMatrixParticle::operator << (const FloatMatrix& m) { 
  (FloatMatrix)*data = m; }

// Create a subMatrix using the data as the parent.  Takes
// the starting row and col, and the size of the submatrix desired.
PtMatrix* FloatMatrixParticle::subMatrix(int startRow, int startCol,
				       int numRows, int numCols) {
  return (new FloatSubMatrix(*data,startRow,startCol,numRows,numCols));
}

// particle copy, this->data needs to be allocated.
Particle& FloatMatrixParticle::operator = (const Particle& p) {
  if (compareType(p)) {
    *data = *(FloatSubMatrix*)p;
  }
  return *this;
}

// particle compare
int FloatMatrixParticle :: operator == (const Particle& p) {
  if (!typesEqual(p)) return 0;
  return *data == *(FloatSubMatrix*)p;
}

// clone, useNew, die analogous to other particles.

Particle* FloatMatrixParticle::clone() const {
  Particle * p = FloatMatrixPlasma.get();
  *(((FloatMatrixParticle*)p)->data) = *data;
  return p;
}

Particle* FloatMatrixParticle::useNew() const {
  LOG_NEW; return new FloatMatrixParticle;
}

// recycle this particle
void FloatMatrixParticle::die() {
  // if no parent, then we have the original copy, kill it 
  if(!parent)
    data->killParent();
  delete data;
  data = NULL; parent = NULL;
  FloatMatrixPlasma.put(this);
}

void FloatMatrixParticle::errorAssign(const char* argType) const {
  StringList msg = "Attempt to assign type ";
  msg += argType;
  msg += " to a FloatMatrixParticle";
  Error::abortRun(msg);
}


//////////////////////////////
// IntMatrixParticle Class
//////////////////////////////

static IntMatrixParticle impproto;
static Plasma IntMatrixPlasma(impproto);
static PlasmaGate IntMatrixGate(IntMatrixPlasma);

// IntMatrixParticle methods

DataType IntMatrixParticle::type() const { return INT_MATRIX;}

IntMatrixParticle::IntMatrixParticle() : data(0) {}

IntMatrixParticle::operator IntSubMatrix* () const {
  return data; }

// Setup the data field.  Should only be called once.
void IntMatrixParticle::initMatrix(int nRows, int nCols, int rowDelays,
				   int colDelays) {
  LOG_NEW; data = new IntSubMatrix(nRows,nCols);
  int row, col;
  for(row = 0; row < rowDelays; row++)
    for(col = 0; col < nCols; col++)
      (*data)[row][col] = 0;
  for(row = rowDelays; row < nRows; row++)
    for(col = 0; col < colDelays; col++)
      (*data)[row][col] = 0;
}

// Set the data field to the given SubMatrix and the pointer to the
// MatrixParticle containing the parent matrix
void IntMatrixParticle::initialize(PtMatrix *m, MatrixParticle* p) {
  // should we check the type?
  if(data) {
    parent->die();
    LOG_DEL; delete data;
  }
  data = (IntSubMatrix*)m;
  parent = p;
}

// Reset the data and parent pointers to NULL
Particle& IntMatrixParticle::initialize() {
  if(data) {
    parent->die();
    LOG_DEL; delete data;
  }
  data = NULL;
  parent = NULL;
  return *this; 
}

StringList IntMatrixParticle::print() const { return data->print(); }

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void IntMatrixParticle::operator << (const ComplexMatrix&) { 
  errorAssign("ComplexMatrix"); }
void IntMatrixParticle::operator << (const FixMatrix&) {
  errorAssign("FixMatrix"); }
void IntMatrixParticle::operator << (const FloatMatrix&) { 
  errorAssign("FloatMatrix"); }

void IntMatrixParticle::operator << (int i) { 
  (IntMatrix)*data = i; }
void IntMatrixParticle::operator << (double f) { 
  (IntMatrix)*data = int(f); }
void IntMatrixParticle::operator << (const Complex& c) { 
  (IntMatrix)*data = int(abs(c));}
void IntMatrixParticle::operator << (const Fix& x) { 
  (IntMatrix)*data = int(x); }
void IntMatrixParticle::operator << (const IntMatrix& m) { 
  (IntMatrix)*data = m; }

// Create a subMatrix using the data as the parent.  Takes
// the starting row and col, and the size of the submatrix desired.
// Assumes the data for this is of type Matrix and not SubMatrix.
PtMatrix* IntMatrixParticle::subMatrix(int startRow, int startCol,
				     int numRows, int numCols) {
  return (new IntSubMatrix(*data,startRow,startCol,numRows,numCols));
}

// particle copy, this->data needs to be allocated.
Particle& IntMatrixParticle::operator = (const Particle& p) {
  if (compareType(p)) {
    *data = *(IntSubMatrix*)p;
  }
  return *this;
}

// particle compare
int IntMatrixParticle :: operator == (const Particle& p) {
  if (!typesEqual(p)) return 0;
  return *data == *(IntSubMatrix*)p;
}

// clone, useNew, die analogous to other particles.

Particle* IntMatrixParticle::clone() const {
  Particle * p = IntMatrixPlasma.get();
  *(((IntMatrixParticle*)p)->data) = *data;
  return p;
}

Particle* IntMatrixParticle::useNew() const {
  LOG_NEW; return new IntMatrixParticle;
}

// recycle this particle
void IntMatrixParticle::die() {
  if(!parent)
    data->killParent();
  delete data;
  data = NULL; parent = NULL;
  IntMatrixPlasma.put(this);
}

void IntMatrixParticle::errorAssign(const char* argType) const {
  StringList msg = "Attempt to assign type ";
  msg += argType;
  msg += " to a IntMatrixParticle";
  Error::abortRun(msg);
}

