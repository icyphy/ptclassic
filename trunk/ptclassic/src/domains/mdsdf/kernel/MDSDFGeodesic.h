#ifndef _MDSDFGeodesic_h
#define _MDSDFGeodesic_h 1
/*******************************************************************
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

 Programmer: Mike J. Chen

 Geodesic for MultiDimensional Synchronous Dataflow
 In addition to normal Geodesic functions, this adds the ability
 to resize the matrix data accumulated in a stream of Particles
 to fit the demands of different outputs.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "Geodesic.h"
#include "SubMatrix.h"
#include "MatrixParticle.h"

class MDSDFGeodesic : public Geodesic {
public:
        // constructor
	MDSDFGeodesic();

        // initializes the delays, and calls initBuffer
        virtual void initialize();

	// Access the valid dimensions of the Geodesic's buffer
        int lastValidRow() { return lastRow; }
        int lastValidCol() { return lastCol; }

        // Update the last valid row and column in the Geodesic
        void setValid(int row, int col);

        // Access the location of a single double value stored in the
        // Geodesic's buffer, a quick hack
	double getFloatInput(int rowFiringIndex, int colFiringIndex,
			     int rowDelay = 0, int colDelay = 0);
	double& getFloatOutput(int rowFiringIndex, int colFiringIndex) {
	  if(motherParticle->type() != FLOAT_MATRIX) {
            Error::abortRun("attempt to use getFloatInput on non FLOAT_MATRIX output");
	    return *(double*)0;
	  }
	  int rowIndex = rowFiringIndex+rowDelays;
	  int colIndex = (colFiringIndex+colDelays)%mNumCols;
	  return (*((FloatSubMatrix*)(*((FloatMatrixParticle*)motherParticle))))[rowIndex][colIndex];
	}  
 
        // Access a submatrix of the mothermatrix stored in the Geodesic
        PtMatrix* getInput(int rowFiringIndex, int colFiringIndex,
                         int rowDelay = 0, int colDelay = 0);
        PtMatrix* getOutput(int rowFiringIndex, int colFiringIndex);

        MatrixParticle* mainParticle() { return motherParticle; }

	/*virtual*/ void resetBufferValues() {};

 protected:
	int lastRow;     // The index of the last valid row and columns
	int lastCol;     // in the buffer.
   
        MatrixParticle* motherParticle;

        int rowDelays;   // The number of delays in each dimension
        int colDelays;

	int mNumRows;  // The dimensions of the motherMatrix
	int mNumCols;

        int originalNumCols; // The number of cols of the motherMatrix if
	                     // there are no column delays.
};

#endif
