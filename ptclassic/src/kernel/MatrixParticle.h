#ifndef _MatrixParticle_h
#define _MatrixParticle_h 1

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

 This file defines the implementation of the MatrixParticle classes.
 These are not full Particles in the normal sense, but act as a
 source of SubMatrices for the MDSDF domain.

**************************************************************************/

#include "SubMatrix.h"
#include "Particle.h"

extern const DataType COMPLEX_MATRIX;
extern const DataType FIX_MATRIX;
extern const DataType FLOAT_MATRIX;
extern const DataType INT_MATRIX;

/////////////////////////////
//  MatrixParticle classes
/////////////////////////////

// MatrixParticle base class
class MatrixParticle : public Particle {
 public:
        // Inherited from Particle class
        //
        // identify what type of particle this is
        // virtual DataType type() const = 0;
        //
        // print particle
        // virtual StringList print() const = 0;
        //
        // copy a particle
        // virtual Particle& operator = (const Particle&) = 0;
        //
        // compare two particles
	// virtual int operator == (const Particle&) = 0;
        //
        // clone the particle, and remove clone
        // virtual Particle* clone() const = 0;
        // virtual Particle* useNew() const = 0;
        // virtual void die();

        // /* virtual */ DataType type() const;

        // Constructor
        MatrixParticle() : parent(0) {}

        // All these conversions return as an error.
	operator int () const;
	operator float () const;
	operator double () const;
	operator Complex () const;
	operator Fix () const;

        // These conversions also return an error unless derived classes
        // redefine them.
        virtual operator ComplexSubMatrix* () const;
        virtual operator FixSubMatrix* () const;
        virtual operator FloatSubMatrix* () const;
	virtual operator IntSubMatrix* () const;

        // setup the data field by creating a matrix with the given dimensions
	virtual void initMatrix(int numRows, int numCols, int rowDelays,
				int colDelays) = 0;

        // Set the data field to the given SubMatrix
        virtual void initialize(PtMatrix *m, MatrixParticle *p) = 0;

        // Create a subMatrix using the data as the parent.  Takes
        // the starting row and col, and the size of the submatrix desired.
        virtual PtMatrix* subMatrix(int startRow, int startCol,
				  int numRows, int numCols) = 0;
 
 protected:
        // some error functions
        virtual void errorAssign(const char*) const = 0;
        int errorConvert(const char*) const;

        // The MatrixParticle that contains the actual allocated
        // mothermatrix, allows more than one MatrixParticle to reference
        // same data, usually null, meaning this particle has the data
        MatrixParticle *parent;
};

class ComplexMatrixParticle : public MatrixParticle { 
public:
 	DataType type() const;

	// fill in remaining functions for Particle classes
	ComplexMatrixParticle();

        // setup the data field with a matrix of the given dimensions
	void initMatrix(int numRows, int numCols, int rowDelays,
			int colDelays);

        // Set the data field to the given SubMatrix
        virtual void initialize(PtMatrix *m, MatrixParticle *p);

        // Reset the data and parent pointers to NULL
        /* virtual */ Particle& initialize();

        StringList print() const;

        /* virtual */ operator ComplexSubMatrix* () const;

	// these return an error
	void operator << (const FixMatrix& m);
	void operator << (const FloatMatrix& m);
	void operator << (const IntMatrix& m);

        void operator << (int i);
	void operator << (double f);
	void operator << (const Complex& c);
	void operator << (const Fix& x);
	void operator << (const ComplexMatrix& m);

        // Create a subMatrix using the data as the parent.  Takes
        // the starting row and col, and the size of the submatrix desired.
        virtual PtMatrix* subMatrix(int startRow, int startCol,
				  int numRows, int numCols);

	// particle copy
	Particle& operator = (const Particle& p);

	// particle compare
	int operator == (const Particle& p);

	// clone, useNew, die analogous to other particles.
	Particle* clone() const;
	Particle* useNew() const;
	void die();

private:
	void errorAssign(const char*) const;
	ComplexSubMatrix *data;
};

class FixMatrixParticle : public MatrixParticle {
 public:
        // Tell the world that this particle contains an FixMatrix
	DataType type() const;

	// constructors
	FixMatrixParticle();

        // setup the data field with a matrix of the given dimensions
	void initMatrix(int numRows, int numCols, int rowDelays,
			int colDelays);

        // Set the data field to the given SubMatrix
        virtual void initialize(PtMatrix *m, MatrixParticle *p);

        // Reset the data and parent pointers to NULL
        /* virtual */ Particle& initialize();

        StringList print() const;

        /* virtual */ operator FixSubMatrix* () const;

	// these return an error
	void operator << (const ComplexMatrix& m);
	void operator << (const FloatMatrix& m);
	void operator << (const IntMatrix& m);

        void operator << (int i);
	void operator << (double f);
	void operator << (const Complex& c);
	void operator << (const Fix& x);
	void operator << (const FixMatrix& m);

        // Create a subMatrix using the data as the parent.  Takes
        // the starting row and col, and the size of the submatrix desired.
        virtual PtMatrix* subMatrix(int startRow, int startCol,
				  int numRows, int numCols);

	// particle copy
	Particle& operator = (const Particle& p);

	// particle compare
	int operator == (const Particle& p);

	// clone, useNew, die analogous to other particles.
	Particle* clone() const;
	Particle* useNew() const;
	void die();

private:
	void errorAssign(const char*) const;
	FixSubMatrix *data;
};

class FloatMatrixParticle : public MatrixParticle {
public:
	DataType type() const;

	// fill in remaining functions for Particle classes
	FloatMatrixParticle();

        // setup the data field with a matrix of the given dimensions
	void initMatrix(int numRows, int numCols, int rowDelays,
			int colDelays);

        // Set the data field to the given SubMatrix
        virtual void initialize(PtMatrix *m, MatrixParticle *p);

        // Reset the data and parent pointers to NULL
        /* virtual */ Particle& initialize();

        StringList print() const;

        /* virtual */ operator FloatSubMatrix* () const;

	// these return an error
	void operator << (const ComplexMatrix& m);
	void operator << (const FixMatrix& m);
	void operator << (const IntMatrix& m);

        void operator << (int i);
	void operator << (double f);
	void operator << (const Complex& c);
	void operator << (const Fix& x);
	void operator << (const FloatMatrix& m);

        // Create a subMatrix using the data as the parent.  Takes
        // the starting row and col, and the size of the submatrix desired.
        virtual PtMatrix* subMatrix(int startRow, int startCol,
				  int numRows, int numCols);

	// particle copy
	Particle& operator = (const Particle& p);

	// particle compare
	int operator == (const Particle& p);

	// clone, useNew, die analogous to other particles.
	Particle* clone() const;
	Particle* useNew() const;
	void die();

private:
	void errorAssign(const char*) const;
	FloatSubMatrix *data;
};

// Particle classes used to transmit Matrices
class IntMatrixParticle : public MatrixParticle {
 public:
        // Tell the world that this particle contains an IntMatrix
	DataType type() const;

	// constructors
	IntMatrixParticle();

        // setup the data field with a matrix of the given dimensions
	void initMatrix(int numRows, int numCols, int rowDelays,
			int colDelays);

        // Set the data field to the given SubMatrix
        virtual void initialize(PtMatrix *m, MatrixParticle *p);

        // Reset the data and parent pointers to NULL
        /* virtual */ Particle& initialize();

        StringList print() const;

        /* virtual */ operator IntSubMatrix* () const;

	// these return an error
	void operator << (const ComplexMatrix& m);
	void operator << (const FixMatrix& m);
	void operator << (const FloatMatrix& m);

        void operator << (int i);
	void operator << (double f);
	void operator << (const Complex& c);
	void operator << (const Fix& x);
	void operator << (const IntMatrix& m);

        // Create a subMatrix using the data as the parent.  Takes
        // the starting row and col, and the size of the submatrix desired.
        virtual PtMatrix* subMatrix(int startRow, int startCol,
				  int numRows, int numCols);

	// particle copy
	Particle& operator = (const Particle& p);

	// particle compare
	int operator == (const Particle& p);

	// clone, useNew, die analogous to other particles.
	Particle* clone() const;
	Particle* useNew() const;
	void die();

private:
	void errorAssign(const char*) const;
	IntSubMatrix *data;
};

#endif
