static const char file_id[] = "MatlabPtIfc.cc";

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

 Programmer:  Brian L. Evans
 Date of creation: 5/21/96
 Revisions:

 Base class for the Ptolemy interface to Matlab.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MatlabPtIfc.h"
#include "MatlabIfcFuns.h"

Matrix* MatlabPtIfc :: PtolemyToMatlab(
	Particle& particle, DataType portType, int* errflag) {
    // can't use a switch because enumerated data types
    // are assigned to strings and not to integers
    Real* realp = 0;
    Real* imagp = 0;
    Matrix* matlabMatrix = 0;
    if ( portType == INT || portType == FLOAT || portType == FIX ) {
	matlabMatrix = mxCreateFull(1, 1, MXREAL);
	realp = mxGetPr(matlabMatrix);
	*realp = double(particle);
    }
    else if ( portType == COMPLEX ) {
	matlabMatrix = mxCreateFull(1, 1, MXCOMPLEX);
	realp = mxGetPr(matlabMatrix);
	imagp = mxGetPi(matlabMatrix);
	Complex temp = particle;
	*realp = real(temp);
	*imagp = imag(temp);
    }
    else if ( portType == COMPLEX_MATRIX_ENV ) {
	Envelope Apkt;
	particle.getMessage(Apkt);
	const ComplexMatrix& Amatrix = *(const ComplexMatrix *)Apkt.myData();

	// allocate a Matlab matrix and name it
	int rows = Amatrix.numRows();
	int cols = Amatrix.numCols();
	matlabMatrix = mxCreateFull(rows, cols, MXCOMPLEX);

	// copy values in the Ptolemy matrix to the Matlab matrix
	// Matlab stores values in column-major order like Fortran
	Real* realp = mxGetPr(matlabMatrix);
	Real* imagp = mxGetPi(matlabMatrix);
	for ( int icol = 0; icol < cols; icol++ ) {
	    for ( int irow = 0; irow < rows; irow++ ) {
		Complex temp = Amatrix[irow][icol];
		*realp++ = real(temp);
		*imagp++ = imag(temp);
	    }
	}
    }
    else if ( portType == FIX_MATRIX_ENV ) {
	Envelope Apkt;
	particle.getMessage(Apkt);
	const FixMatrix& Amatrix = *(const FixMatrix *)Apkt.myData();

	// allocate a Matlab matrix and name it
	int rows = Amatrix.numRows();
	int cols = Amatrix.numCols();
	matlabMatrix = mxCreateFull(rows, cols, MXREAL);

	// copy values in the Ptolemy matrix to the Matlab matrix
	// Matlab stores values in column-major order like Fortran
	Real* realp = mxGetPr(matlabMatrix);
	for ( int icol = 0; icol < cols; icol++ ) {
	    for ( int irow = 0; irow < rows; irow++ ) {
		*realp++ = double(Amatrix[irow][icol]);
	    }
	}
    }
    else if ( portType == FLOAT_MATRIX_ENV ) {
	Envelope Apkt;
	particle.getMessage(Apkt);
	const FloatMatrix& Amatrix = *(const FloatMatrix *)Apkt.myData();

	// allocate a Matlab matrix and name it
	int rows = Amatrix.numRows();
	int cols = Amatrix.numCols();
	matlabMatrix = mxCreateFull(rows, cols, MXREAL);

	// copy values in the Ptolemy matrix to the Matlab matrix
	// Matlab stores values in column-major order like Fortran
	Real* realp = mxGetPr(matlabMatrix);
	for ( int icol = 0; icol < cols; icol++ ) {
	    for ( int irow = 0; irow < rows; irow++ ) {
		*realp++ = Amatrix[irow][icol];
	    }
	}
    }
    else if ( portType == INT_MATRIX_ENV ) {
	Envelope Apkt;
	particle.getMessage(Apkt);
	const IntMatrix& Amatrix = *(const IntMatrix *)Apkt.myData();

	// allocate a Matlab matrix and name it
	int rows = Amatrix.numRows();
	int cols = Amatrix.numCols();
	matlabMatrix = mxCreateFull(rows, cols, MXREAL);

	// copy values in the Ptolemy matrix to the Matlab matrix
	// Matlab stores values in column-major order like Fortran
	Real* realp = mxGetPr(matlabMatrix);
	for ( int icol = 0; icol < cols; icol++ ) {
	    for ( int irow = 0; irow < rows; irow++ ) {
		*realp++ = double(Amatrix[irow][icol]);
	    }
	}
    }
    else {
	*errflag = TRUE;
	matlabMatrix = mxCreateFull(1, 1, MXREAL);
	Real* realp = mxGetPr(matlabMatrix);
	*realp = 0.0;
    }

    return matlabMatrix;
}

// Returns 1 for Failure and 0 for Success
// copy a Matlab output matrix to a Ptolemy matrix
int MatlabPtIfc :: MatlabToPtolemy(
	Particle& particle, DataType portType,
	Matrix* matlabMatrix, int* warnflag, int* errflag) {
    // error and warning flags
    *errflag = FALSE;
    int incompatibleOutportPort = FALSE;
    *warnflag = FALSE;

    // allocate a Ptolemy matrix
    int rows = mxGetM(matlabMatrix);
    int cols = mxGetN(matlabMatrix);

    if ( mxIsFull(matlabMatrix) ) {

	// for real matrices, imagp will be null
	Real* realp = mxGetPr(matlabMatrix);
	Real* imagp = mxGetPi(matlabMatrix);

	// copy Matlab matrices (in column-major order) to Ptolemy
	if ( portType == COMPLEX_MATRIX_ENV ) {
	    ComplexMatrix& Amatrix = *(new ComplexMatrix(rows, cols));
	    for ( int jcol = 0; jcol < cols; jcol++ ) {
		for ( int jrow = 0; jrow < rows; jrow++ ) {
		    double realValue = *realp++;
		    double imagValue = ( imagp ) ? *imagp++ : 0.0;
		    Amatrix[jrow][jcol] = Complex(realValue, imagValue);
		}
	    }
	    // write the matrix to output port j
	    // do not delete Amatrix: particle class handles that
	    particle << Amatrix;
	}
	else if ( portType == FLOAT_MATRIX_ENV ) {
	    FloatMatrix& Amatrix = *(new FloatMatrix(rows, cols));
	    if ( mxIsComplex(matlabMatrix) ) {
		*warnflag = TRUE;
		warningString =
		    "\nImaginary components ignored for the Matlab matrix ";
	    }
	    for ( int jcol = 0; jcol < cols; jcol++ ) {
		for ( int jrow = 0; jrow < rows; jrow++ ) {
		    Amatrix[jrow][jcol] = *realp++;
		}
	    }
	    // write the matrix to output port j
	    // do not delete Amatrix: particle class handles that
	    particle << Amatrix;
	}
	// this situation should never be encountered since the
	// setup method should have checked for output data type
	else {
	    incompatibleOutportPort = TRUE;
	}
    }
    else {
	*errflag = TRUE;
    }

    return incompatibleOutportPort;
}
