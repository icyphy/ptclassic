static const char file_id[] = "MDSDFGeodesic.cc";

/*  $Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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
  // dimensions of the matrices the originating port produces
  /*int oNumRows = ((MDSDFPortHole*)originatingPort)->numRowXfer();*/
  int oNumCols = ((MDSDFPortHole*)originatingPort)->numColXfer();
  mNumRows = destNumRowsPerIter * dNumRows;
  mNumCols = destNumColsPerIter * dNumCols;
  originalNumCols = mNumCols;

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
    rowDelays = (int)initDelays[0];
    colDelays = (int)initDelays[1];

    // for now, the column delays must be a multiple of the
    // originating porthole's dimensions
    if(colDelays%oNumCols != 0) {
      Error::abortRun("column delay specification must be a multiple of the column dimension of the input porthole");
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
    
    // add extra rows for useless data
    mNumRows += rowDelays; 
    // add extra cols, must be multiples of original mother matrix dimensions
    mNumCols += (1 + (colDelays-1)/mNumCols)*mNumCols;
  }

  // get motherParticle
  motherParticle = (MatrixParticle*)((MDSDFPortHole*)originatingPort)->myPlasma->get();

  // tell the particle to create a data matrix (of appropriate type) with
  // the given (row,col) dimensions.
  motherParticle->initMatrix(mNumRows, mNumCols, rowDelays, colDelays);

  pstack.putTail(motherParticle);

}

void MDSDFGeodesic::setValid(int row, int col) { 
  lastRow = row + rowDelays;
  lastCol = col + colDelays;
  if(colDelays && lastCol%originalNumCols == 0) {
    lastRow++;
    lastCol = colDelays-1;
  }
}
    

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
PtMatrix* MDSDFGeodesic::getInput(int rowFiringIndex,int colFiringIndex,
				int rowDelay, int colDelay) {
  PtMatrix* result;
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

PtMatrix* MDSDFGeodesic::getOutput(int rowFiringIndex, int colFiringIndex) {
  int srcRows = ((MDSDFPortHole*)originatingPort)->numRowXfer();
  int srcCols = ((MDSDFPortHole*)originatingPort)->numColXfer();
  PtMatrix* result;
 
  result=motherParticle->subMatrix(rowFiringIndex*srcRows+rowDelays,
				   (colFiringIndex*srcCols+colDelays)%mNumCols,
				   srcRows,
				   srcCols);
  return result;
}  
