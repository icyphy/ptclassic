#ifndef _MDSDFGeodesic_h
#define _MDSDFGeodesic_h 1
/*******************************************************************
Version identification:
 $Id$

 Copyright (c) 1990-1994 The Regents of the University of California,
                         All Rights Reserved.

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
        void setValid(int row, int col) { 
          lastRow = row + rowDelays;
          lastCol = col + colDelays; }

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
        Matrix* getInput(int rowFiringIndex, int colFiringIndex,
                         int rowDelay = 0, int colDelay = 0);
        Matrix* getOutput(int rowFiringIndex, int colFiringIndex);

        MatrixParticle* mainParticle() { return motherParticle; }

 protected:
	int lastRow;     // The index of the last valid row and columns
	int lastCol;     // in the buffer.
   
        MatrixParticle* motherParticle;

        int rowDelays;   // The number of delays in each dimension
        int colDelays;

	int mNumRows;  // The dimensions of the motherMatrix
	int mNumCols;
};

#endif
