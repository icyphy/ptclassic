static const char file_id[] = "MDSDFGeodesic.cc";

/*  $Id$

Copyright (c) 1990, 1991, 1992, 1993 The Regents of the University of
                                      California.
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

    Programmer:		Mike J. Chen
    Date of creation:	7/23/93

    Code for domain-specific Geodesic class.
****************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MDSDFGeodesic.h"
#include "MDSDFPortHole.h"
#include "Plasma.h"
#include "PtGate.h"
#include "Fraction.h"  // need lcm definition

// constructor
MDSDFGeodesic::MDSDFGeodesic() : lastRow(-1), lastCol(-1), motherParticle(0) {}

// destructor
MDSDFGeodesic::~MDSDFGeodesic() {
}

// Allocate a motherMatrix data structure, create a local buffer of
// MatrixParticles holding subMatrices that point to the motherMatrix.
// This function should only be called once, before the universe runs.
// Subsequent requests for new buffer space should be made by calling
// reInit().  This function will create a local buffer myBuffer with
// predicate buffer size.  reInit() will be used to maintain a possibly
// smaller steady-state buffer size.
//
// Buffer size is a function of balancing the destination and source
// dimensions, as well as the need for the destination to access past
// samples, and any initial tokens on the buffer.  Because we are modelling
// the source as independent parallel executors, they must access past
// tokens as stored in the Geodesic and not in their own local input
// buffers (which is unlike how a single processer system can simply
// store past samples in its local buffer because they are contiguous
// to subsequent firings of that star).
void MDSDFGeodesic::initialize() {
  // the number of row and column firings for the destination as determined
  // by the balance equations
  int destNumRowsPerIter = ((MDSDFPortHole*)destinationPort)->rowFiringsPerIteration();
  int destNumColsPerIter = ((MDSDFPortHole*)destinationPort)->colFiringsPerIteration();
  // dimensions of the matrices the destination port expects
  int dNumRows = ((MDSDFPortHole*)destinationPort)->numRowXfer();
  int dNumCols = ((MDSDFPortHole*)destinationPort)->numColXfer();
  // number of past tokens in each dimension that the destination port wants
//  int maxPastRowTokens = ((MDSDFPortHole*)destinationPort)->pastRowTokens();
//  int maxPastColTokens = ((MDSDFPortHole*)destinationPort)->pastColTokens();
  // dimensions of the matrices the originating port produces
  int oNumRows = ((MDSDFPortHole*)originatingPort)->numRowXfer();
  int oNumCols = ((MDSDFPortHole*)originatingPort)->numColXfer();
  // dimensions of the mother matrix
//  mNumRows = (destNumRowsPerIter + maxPastRowTokens) * dNumRows;
//  mNumCols = (destNumColsPerIter + maxPastColTokens) * dNumCols;
  mNumRows = destNumRowsPerIter * dNumRows;
  mNumCols = destNumColsPerIter * dNumCols;

  rowDelays = 0;
  colDelays = 0;
  lastRow = -1;
  lastCol = -1;

  // clear the buffer of boundary subMatrices (created by backward references
  //  with negative indices)
  pstack.freeup();

  // figure out delays, for now delays must be specified as a floatArrayState,
  // the first two entries are the row and column delays respectively, all
  // else is ignored
  if(initValues && *initValues != 0) {
    FloatArrayState initDelays;
    initDelays.setState("initDelays",parent(),initValues);
    initDelays.initialize();
    if(initDelays.size() < 2) {
      Error::abortRun("not enough delay arguements to describe 2-D delay");
      return;
    }
    rowDelays = (int)initDelays[0] * oNumRows;
    colDelays = (int)initDelays[1] * oNumCols;

    // for now, the row and column delays must not be greater than the
    // size of the mother matrix
    if(rowDelays > mNumRows || colDelays > mNumCols) {
      Error::abortRun("row and column delay specifications too large");
      return;
    }

    if(rowDelays > 0 && colDelays > 0) {
      lastRow = rowDelays-1;
      lastCol = colDelays-1;
    }
    else if(colDelays > 0) {
      lastRow = mNumRows;
      lastCol = colDelays-1;
    }
    else if(rowDelays > 0) {
      lastRow = rowDelays-1;
      lastCol = mNumCols;
    }

    mNumRows += rowDelays; // add extra rows for useless data
    mNumCols += mNumCols;  // double the number of columns
  }

  // get motherParticle
  motherParticle = (MatrixParticle*)((MDSDFPortHole*)originatingPort)->myPlasma->get();

  // tell the particle to create a data matrix (of appropriate type) with
  // the given (row,col) dimensions.
  motherParticle->initMatrix(mNumRows, mNumCols, rowDelays, colDelays);

  pstack.putTail(motherParticle);

}

/*void MDSDFGeodesic::reInit() {
  // the number of row and column firings for the destination as determined
  // by the balance equations
  int destNumRowsPerIter = ((MDSDFPortHole*)destinationPort)->rowFiringsPerIteration();
  int destNumColsPerIter = ((MDSDFPortHole*)destinationPort)->colFiringsPerIteration();
  // dimensions of the matrices the destination port expects
  int dNumRows = ((MDSDFPortHole*)destinationPort)->numRowXfer();
  int dNumCols = ((MDSDFPortHole*)destinationPort)->numColXfer();
  // dimensions of the matrices the originating port produces
  int oNumRows = ((MDSDFPortHole*)originatingPort)->numRowXfer();
  int oNumCols = ((MDSDFPortHole*)originatingPort)->numColXfer();
  // dimensions of the mother matrix
//  int mNumRows = destNumRowsPerIter * dNumRows;
//  int mNumCols = destNumColsPerIter * dNumCols;
  mNumRows = destNumRowsPerIter * dNumRows;
  mNumCols = destNumColsPerIter * dNumCols;

  // Remove any Particles already on the Geodesic and return them to the Plasma
  myBuffer.freeup();

  // allocate a motherMatrix
  MatrixParticle* motherParticle = (MatrixParticle*)((MDSDFPortHole*)originatingPort)->myPlasma->get();
  // tell the particle to create a data matrix (of appropriate type) with
  // the given (row,col) dimensions.
  motherParticle->initMatrix(mNumRows, mNumCols);

  // fill the Geodesic buffer with MatrixParticles holding submatrices.
  // These particles will be used by the destination porthole.
  for(int row = 0; row < mNumRows; row += dNumRows) {
    for(int col = 0; col < mNumCols; col += dNumCols) {
      Matrix* child = motherParticle->subMatrix(row,col,dNumRows,dNumCols);
      Particle* p = ((MDSDFPortHole*)originatingPort)->myPlasma->get();
      ((MatrixParticle*)p)->initialize(child,motherParticle);
      pstack.putTail(p);
    }
  }
  // handling row delays, column delays only handled once
  for(int row = 0; row < maxPastRowTokens; row++)
    motherParticle->setRow(row,0);  // set data values of entire row to 0

  ((OutMDSDFPort*)originatingPort)->initializeBuffer(motherParticle,
						     rowDelay,0);
  inputCount = 0;


}
*/
// Increment the count of the number of input tokens received in each
// dimension.  The output tokens in the buffer are not valid until
// enough input tokens have been received.
/*void MDSDFGeodesic::incReceiveCount(int numRowTokens, int numColTokens) {
  CriticalSection region(gate);
  rowInputCount += numRowTokens;
  colInputCount += numColTokens;
  if(rowInputCount >= rowInputsRequired && 
     colInputCount >= colInputsRequired) {
    rowInputCount -= rowInputsRequired;
    colInputCount -= colInputsRequired;
//    sz += ((DFPortHole*)destinationPort)->parentReps();
//    sz += numOutputsGenerated;
    bufferRowSize += numRowOutputsGenerated;
    bufferColSize += numColOutputsGenerated;*/
//    maxBufLength += sz;  /* is this needed any longer? */
/*  }
}*/

// Functions for simulating
//void MDSDFGeodesic::incCount(int n) {
//  incReceiveCount(n);
//}

// Accessing the location of a single scalar value, no type checking done
double MDSDFGeodesic::getFloatInput(int rowFiringIndex,int colFiringIndex,
				    int rowDelay, int colDelay) {
  if(motherParticle->type() != FLOAT_MATRIX) {
    Error::abortRun("attempt to use getFloatInput on non FLOAT_MATRIX input");
    return 0.0;
  }
  rowFiringIndex = rowFiringIndex+rowDelay;
  if(rowFiringIndex < 0 || rowFiringIndex >= mNumRows)
    return 0.0;
  colFiringIndex = colFiringIndex%mNumCols + colDelay;
  if(colFiringIndex < 0 || colFiringIndex >= mNumCols)
    return 0.0;
  return (*((FloatSubMatrix*)(*((FloatMatrixParticle*)motherParticle))))[rowFiringIndex][colFiringIndex];
}

// Accessing submatrices of the mothermatrix, used by stars to read and
// write directly into the mothermatrix buffer
// NOTE: Star should delete submatrix when done
Matrix* MDSDFGeodesic::getInput(int rowFiringIndex,int colFiringIndex,
				int rowDelay, int colDelay) {
  Matrix* result;
  int destRows = ((MDSDFPortHole*)destinationPort)->numRowXfer();
  int destCols = ((MDSDFPortHole*)destinationPort)->numColXfer();
  rowFiringIndex = (rowFiringIndex+rowDelay)*destRows;
  if(rowFiringIndex < 0 || rowFiringIndex >= mNumRows)
    rowFiringIndex = -1;
  colFiringIndex = colFiringIndex*destCols%mNumCols;
  if(colDelay)
    colFiringIndex += colDelay*destCols;
  if(colFiringIndex < 0 || colFiringIndex >= mNumCols)
    colFiringIndex = -1;
  if(rowFiringIndex == -1 || colFiringIndex == -1) {
    MatrixParticle* backParticle = (MatrixParticle*)((MDSDFPortHole*)originatingPort)->myPlasma->get();
    backParticle->initMatrix(destRows,destCols,destRows,destCols);
    pstack.putTail(backParticle);
    return backParticle->subMatrix(0,0,destRows,destCols);
  }
  else
    result = motherParticle->subMatrix(rowFiringIndex, colFiringIndex,
				       destRows, destCols);
  return result;
}

Matrix* MDSDFGeodesic::getOutput(int rowFiringIndex, int colFiringIndex) {
  int srcRows = ((MDSDFPortHole*)originatingPort)->numRowXfer();
  int srcCols = ((MDSDFPortHole*)originatingPort)->numColXfer();
  Matrix* result;
 
  result=motherParticle->subMatrix(rowFiringIndex*srcRows+rowDelays,
				   (colFiringIndex*srcCols+colDelays)%mNumCols,
				   srcRows,
				   srcCols);
  return result;
}  
