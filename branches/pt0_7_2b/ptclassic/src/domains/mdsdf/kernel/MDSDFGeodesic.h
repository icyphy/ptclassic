#ifndef _MDSDFGeodesic_h
#define _MDSDFGeodesic_h 1
/*******************************************************************
Version identification:
 $Id$

 Copyright (c) 1993 The Regents of the University of California,
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

// I had it privately derived before, why?
class MDSDFGeodesic : public Geodesic {
public:
        // constructor
	MDSDFGeodesic();

        // destructor
	~MDSDFGeodesic();

        // initializes the delays, and calls initBuffer
        virtual void initialize();

        // initializes the motherMatrix, the Geodesic's buffer, and the
        // originating port's buffer
//	void initBuffer();

        // increment the count of the number of input tokens received
//        void incReceiveCount(int n);

        // functions in Geodesic that are no longer used
        // void put(Particle* p);

        // functions that replace those from the Geodesic class
//        virtual void incCount(int);

        // inherited from Geodesic
        Geodesic::get;

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

        // set delays in two dimensions
        void setDelay(int numPastRowTokens, int numPastColTokens) {
          maxPastRowTokens = numPastRowTokens;
          maxPastColTokens = numPastColTokens; }

        MatrixParticle* mainParticle() { return motherParticle; }

 protected:
	int lastRow;     // The index of the last valid row and columns
	int lastCol;     // in the buffer.
   
        int maxPastRowTokens;
        int maxPastColTokens;

        MatrixParticle* motherParticle;

        int rowDelays;   // The number of delays in each dimension
        int colDelays;

//        int rowInputCount;    // The number of data matrices in each
//        int colInputCount;    // dimension in the buffer.

//	int rowInputsRequired; // The number of inputs in each dimension
//	int colInputsRequired; // needed before output data is "valid"
       
//	int rowOutputsGenerated; // The number of outputs in each dimension
//	int colOutputsGenerated; // generated once there are enough inputs

	int mNumRows;  // The dimensions of the motherMatrix
	int mNumCols;
};

#endif
