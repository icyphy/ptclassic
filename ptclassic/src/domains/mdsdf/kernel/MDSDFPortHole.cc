static const char file_id[] = "MDSDFPortHole.cc";

/**************************************************************************
  Version $Id$

Copyright (c) 1990-1994
   The Regents of the University of California. All rights reserved.

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
    Date of creation:	7/22/93

    Code for domain-specific PortHole classes.
****************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MDSDFPortHole.h"
#include "MDSDFGeodesic.h"
#include "CircularBuffer2D.h"
#include "Plasma.h"
#include "MDSDFStar.h"
#include "miscFuncs.h"

	///////////////////////////////////////////
	// class MDSDFPortHole
	//////////////////////////////////////////

// constructor
MDSDFPortHole::MDSDFPortHole() : numRows(0), numCols(0), rowsInBuffer(0),
                                 colsInBuffer(0), myBuffer(0) {};

// destructor
MDSDFPortHole::~MDSDFPortHole() {
  myBuffer.freeup();
//  LOG_DEL; delete myBuffer;
}

// This function is called by the scheduler to set the number of row
// and column firing repetitions per iteration.
//void MDSDFPortHole::setRepetitions(int row, int col) {
//  rowFiringsPerIteration = row;
//  colFiringsPerIteration = col;
//}

// Function to set the number of new tokens (in terms of columns and
// rows) read at each firing, the dimensions of the tokens, 
// and the number of past tokens needed in each dimension.
PortHole& MDSDFPortHole::setMDSDFParams(unsigned rowDimensions,
					unsigned colDimensions,
					unsigned numPastRows,
					unsigned numPastCols) {
  numRows = rowDimensions;
  numCols = colDimensions;
//  numRowTokens = nRowTokens;
//  numColTokens = nColTokens;
  ((MDSDFGeodesic*)myGeodesic)->setDelay(numPastRows,numPastCols);
//  maxPastRowTokens = numPastRows;
//  maxPastColTokens = numPastCols;
//  if(myBuffer)
//    initialize();   // only re-initialize a connected PortHole
//  Doesn't seem like the above is needed since we don't initPorts until
//  after this is done, as long as setMDSDFParams is only called in the
//  setup() method of each star and not in the go() method.
  return *this; 
}

int MDSDFPortHole::rowFiringsPerIteration() {
  return int(((MDSDFStar*)parent())->rowRepetitions);
}

int MDSDFPortHole::colFiringsPerIteration() {
  return int(((MDSDFStar*)parent())->colRepetitions);
}

//void MDSDFPortHole::allocateBuffer() {
  // If there is a buffer, release its memory
//  LOG_DEL; delete myBuffer;

  // Allocate new buffer, and fill it with MatrixParticles
//  LOG_NEW; myBuffer = new CircularBuffer2D(rowsInBuffer,colsInBuffer);
//  for(int i = myBuffer->size(); i>0; i--) {
//    Particle** p = myBuffer->next();
//    *p = myPlasma->get();
//  }
//}

Geodesic* MDSDFPortHole::allocateGeodesic() {
  char buf[80];
  strcpy(buf, "Node_");
  strcat(buf, name());
  LOG_NEW; MDSDFGeodesic *g = new MDSDFGeodesic;
  g->setNameParent(hashstring(buf),parent());
  return (Geodesic*)g;
}

Matrix* MDSDFPortHole::getInput(int, int) {
  Error::abortRun("getInput() called on a porthole which is not an InMDSDFPort");
  return (Matrix*)0;
}

Matrix* MDSDFPortHole::getOutput() {
  Error::abortRun("getOutput() called on a porthole which is not an OutMDSDFPort");
  return (Matrix*)0;
}

double MDSDFPortHole::getFloatInput(int rowDelay, int colDelay) {
  Error::abortRun("getFloatInput() called on a porthole which is not an InMDSDFPort");
  return 0.0;
}

double& MDSDFPortHole::getFloatOutput() {
  Error::abortRun("getFloatOutput() called on a porthole which is not an OutMDSDFPort");
  return *(double*)0;
}

// Initialize the porthole.
void MDSDFPortHole::initialize() {
  if(!setResolvedType()) {
    Error::abortRun(*this,"can't determine DataType");
    return;
  }

  if(!allocatePlasma()) return;

  // clear the buffer, this buffer is needed  for some backward
  // compatibility problems, notably the fact that TclStarIfc requires
  // particles, we need a place to put the particles so that we can 
  // delete them later
  myBuffer.freeup();

  // If this is an output PortHole (or connected to an input porthole),
  // initialize myGeodesic
  if (far() && myGeodesic && (isItOutput() || (!asEH() && atBoundary())))
    myGeodesic->initialize();
}


// Returns a subMatrix inside a particle, argument is not used, only gets
// the current particle (ie, as if delay was 0)
Particle& MDSDFPortHole::operator % (int) {
  MatrixParticle *p = (MatrixParticle*)myPlasma->get();
  MDSDFStar* s = (MDSDFStar*)parent();
  p->initialize(((MDSDFGeodesic*)myGeodesic)->getInput(s->rowIndex,s->colIndex)
		,((MDSDFGeodesic*)myGeodesic)->mainParticle());
  myBuffer.put(p);
  return *p;
}

	///////////////////////////////////////////
	// class InMDSDFPort
	//////////////////////////////////////////

int InMDSDFPort::isItInput() const { return TRUE; }

inline Matrix* InMDSDFPort::getInput(int rowDelay, int colDelay) { 
  int rowIndex = ((MDSDFStar*)parent())->rowIndex;
  int colIndex = ((MDSDFStar*)parent())->colIndex;
  return ((MDSDFGeodesic*)myGeodesic)->getInput(rowIndex, colIndex,
						rowDelay, colDelay);
}

inline double InMDSDFPort::getFloatInput(int rowDelay, int colDelay) {
  int rowIndex = ((MDSDFStar*)parent())->rowIndex;
  int colIndex = ((MDSDFStar*)parent())->colIndex;
  return ((MDSDFGeodesic*)myGeodesic)->getFloatInput(rowIndex, colIndex,
						     rowDelay, colDelay);
}

// Get particles from the Geodesic and call die() for all the particles
// currently in the buffer.  Is is assumed this routine is called before
// a star executes.  It gets numRowTokens by numColTokens of MatrixParticles
// from the Geodesic and stores them in the buffer, then sets the current
// time to the last Particle input

// Needs to be changed to two dimensional!, buffer of a porthole might
// need to be bigger to accomodate delayed particles
void InMDSDFPort::receiveData() { 
//  for(int row = numRowTokens; row > 0; row--) {
//    for(int col = numColTokens; col > 0; col--) {
//  for(int i = numberTokens; i > 0; i--) {
      // Move the current time pointer in the buffer
      // Get a pointer to the next Particle* in the buffer.
//      MatrixParticle** pOld = (MatrixParticle**)myBuffer->next();
  
      // Get another MatrixParticle from the Geodesic.
//      MatrixParticle* pNew = (MatrixParticle*)myGeodesic->get();
  
//      if(!pNew) {
//        Error::abortRun(*this, "Attempt to read from empty geodesic");
//        return;
//      }
  
      // Release the MatrixParticle data by calling die() and put the
      // MatrixParticle back into the Plasma.
//      pOld->die();
  
      // Put the new particle into the buffer.
//      *pOld = pNew;
//    }
//  }
}

//void InMDSDFPort::decCount(int n) {
//  // used in schedule simulation when an outPortHole fires
//  (MDSDFGeodesic*)myGeodesic->decOutCount(n);
//}


	////////////////////////////////////////////
	// class OutMDSDFPort
	////////////////////////////////////////////

int OutMDSDFPort::isItOutput() const { return TRUE; }

inline Matrix* OutMDSDFPort::getOutput() { 
  int rowIndex = ((MDSDFStar*)parent())->rowIndex;
  int colIndex = ((MDSDFStar*)parent())->colIndex;
  return ((MDSDFGeodesic*)myGeodesic)->getOutput(rowIndex, colIndex);
}

inline double& OutMDSDFPort::getFloatOutput() {
  int rowIndex = ((MDSDFStar*)parent())->rowIndex;
  int colIndex = ((MDSDFStar*)parent())->colIndex;
  return ((MDSDFGeodesic*)myGeodesic)->getFloatOutput(rowIndex,colIndex);
}

// Is is assumed this routine is called before a star executes.
// Prepares the particles in the portHole's buffer to be used when the
// star executes.  Requests the Geodesic to fill the buffer, only if the
// buffer is empty!
// In the first model of static buffering, this star will be fired the
// total number of repetitions times that it is scheduled for in one
// iteration consequtively, before the receiving porthole can fire.  To
// simplify things, myGeodesic->initialize() is called at the start of each
// iteration and will fill the buffer with all the particles needed
// for the firing repetitions in one iteration.
void OutMDSDFPort::receiveData() {
//  if(numParticlesInBuffer == 0)
//    myGeodesic->initialize();
}

void OutMDSDFPort::sendData() { 
//  numParticlesInBuffer -= numRowTokens*numColTokens;

  // Increment the MDSDFGeodesic counter of the number of input tokens
  // received.  The geodesic's output tokens are not "valid" until
  // the required number of input tokens have been received.
  int rIndex = ((MDSDFStar*)parent())->rowIndex;
  int cIndex = ((MDSDFStar*)parent())->colIndex;
  
  ((MDSDFGeodesic*)myGeodesic)->setValid((rIndex+1)*numRows-1,
					 (cIndex+1)*numCols-1);

  // Advance buffer to next token
//  myBuffer->next();
}

//void OutMDSDFPort::incCount(int rown) {
//  // used in schedule simulation when an outPortHole fires
//  myGeodesic->incCount(n);
//}

// Initialize the porthole.  The Geodesic connected to this PortHole
// must be initialized first so we call myGeodesic->initialize().  The
// Geodesic will in turn call initializeBuffer().
// is already initialized, nothing needs to be done.
//void OutMDSDFPort::initialize() {
//  if(!setResolvedType()) {
//    Error::abortRun(*this,"can't determine DataType");
//    return;
//  }
//  if(!allocatePlasma()) return;
//
//  // If this is an output PortHole (or connected to an input porthole),
//  // initialize myGeodesic
//  if (far() && myGeodesic && (isItOutput() || (!asEH() && atBoundary())))
//    myGeodesic->initialize();
//}
    
/*void OutMDSDFPort::initializeBuffer(MatrixParticle* motherMatrix,
				    int rowDelay, int colDelay) {
  // This function should be called after Geodesic::initialize(), and after
  // the scheduler has computed a schedule.

  rowsInBuffer = rowFiringsPerIteration();
  colsInBuffer = colFiringsPerIteration();
//  numParticlesInBuffer = rowsInBuffer * colsInBuffer;
//  if(myBuffer == NULL || myBuffer->rowSize() != rowsInBuffer ||
//     myBuffer->colSize() != colsInBuffer)
//    allocateBuffer();

  // initialize buffer, creates the complete number of MatrixParticles
  // that will used during this iteration, there is an offset given by the
  // paramenters rowDelay and colDelay

//  MatrixParticle** p = (MatrixParticle**)myBuffer->here();
//  for(int row = rowDelay-1; row < numRows*rowsInBuffer; row += numRows) {
//    for(int col = colDelay-1; col < numCols*colsInBuffer; col+= numCols) {
//      SubMatrix* child = motherMatrix->subMatrix(row,col,numRows,numCols);
//      if(*p)
//        // set data of MatrixParticle to child
//        (*p)->initialize(child,motherMatrix);   
//      else {
//        *p = (MatrixParticle*)myPlasma->get();
//        (*p)->initialize(child,motherMatrix);  
//      }
//      p = (MatrixParticle**)myBuffer->next();
//    }
//  }
}
*/
        //////////////////////////////////////////
        // class MultiMDSDFPort
        //////////////////////////////////////////

MultiPortHole& MultiMDSDFPort :: setMDSDFParams(unsigned rowDimensions,
						unsigned colDimensions,
						unsigned numPastRows,
						unsigned numPastCols) {
  MPHIter next(*this);
  PortHole *p;
  while((p = next++) != 0)
    ((MDSDFPortHole*)p)->setMDSDFParams(rowDimensions, colDimensions,
					numPastRows, numPastCols);
  return *this;
}

        //////////////////////////////////////////
        // class MultiInMDSDFPort
        //////////////////////////////////////////
  
// Input/output identification.
int MultiInMDSDFPort::isItInput() const { return TRUE;}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInMDSDFPort::newPort() 
{
	LOG_NEW; MDSDFPortHole& p = *new InMDSDFPort;
//        p.setMDSDFParams(1,1,0,0);  // set default to 1x1 matrix
	return installPort(p);
}

        //////////////////////////////////////////
        // class MultiOutMDSDFPort
        //////////////////////////////////////////

// Input/output identification.
int MultiOutMDSDFPort::isItOutput() const { return TRUE;}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutMDSDFPort::newPort()
{
	LOG_NEW; MDSDFPortHole& p = *new OutMDSDFPort;
//        p.setMDSDFParams(1,1,0,0);  // set default to 1x1 matrix
	return installPort(p);
}

// isA
ISA_FUNC(MDSDFPortHole,DFPortHole);
