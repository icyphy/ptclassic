static const char file_id[] = "MDSDFPortHole.cc";

/**************************************************************************
  $Id$

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
#include "Plasma.h"
#include "MDSDFStar.h"
#include "miscFuncs.h"

	///////////////////////////////////////////
	// class MDSDFPortHole
	//////////////////////////////////////////

// constructor
MDSDFPortHole::MDSDFPortHole() : numRows(0), numCols(0), myBuffer(0) {};

// destructor
MDSDFPortHole::~MDSDFPortHole() {
  myBuffer.freeup();
}

// Function to set the number of new tokens (in terms of columns and
// rows) read at each firing, the dimensions of the tokens, 
// and the number of past tokens needed in each dimension.
PortHole& MDSDFPortHole::setMDSDFParams(unsigned rowDimensions,
					unsigned colDimensions) {

  numRows = rowDimensions;
  numCols = colDimensions;
  return *this; 
}

int MDSDFPortHole::rowFiringsPerIteration() {
  // Use a two casts because Fraction does not have a direct cast to int.
  return int( double( ((MDSDFStar*)parent())->rowRepetitions ));
}

int MDSDFPortHole::colFiringsPerIteration() {
  // Use a two casts because Fraction does not have a direct cast to int.
  return int( double( ((MDSDFStar*)parent())->colRepetitions ) );
}

PtMatrix* MDSDFPortHole::getInput(int, int) {
  Error::abortRun("getInput() called on a porthole which is not an InMDSDFPort");
  return (PtMatrix*)0;
}

PtMatrix* MDSDFPortHole::getOutput() {
  Error::abortRun("getOutput() called on a porthole which is not an OutMDSDFPort");
  return (PtMatrix*)0;
}

double MDSDFPortHole::getFloatInput(int /*rowDelay*/, int /*colDelay*/) {
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

PtMatrix* InMDSDFPort::getInput(int rowDelay, int colDelay) { 
  int rowIndex = ((MDSDFStar*)parent())->rowIndex;
  int colIndex = ((MDSDFStar*)parent())->colIndex;
  return ((MDSDFGeodesic*)myGeodesic)->getInput(rowIndex, colIndex,
						rowDelay, colDelay);
}

double InMDSDFPort::getFloatInput(int rowDelay, int colDelay) {
  int rowIndex = ((MDSDFStar*)parent())->rowIndex;
  int colIndex = ((MDSDFStar*)parent())->colIndex;
  return ((MDSDFGeodesic*)myGeodesic)->getFloatInput(rowIndex, colIndex,
						     rowDelay, colDelay);
}

	////////////////////////////////////////////
	// class OutMDSDFPort
	////////////////////////////////////////////

int OutMDSDFPort::isItOutput() const { return TRUE; }

PtMatrix* OutMDSDFPort::getOutput() { 
  int rowIndex = ((MDSDFStar*)parent())->rowIndex;
  int colIndex = ((MDSDFStar*)parent())->colIndex;
  return ((MDSDFGeodesic*)myGeodesic)->getOutput(rowIndex, colIndex);
}

double& OutMDSDFPort::getFloatOutput() {
  int rowIndex = ((MDSDFStar*)parent())->rowIndex;
  int colIndex = ((MDSDFStar*)parent())->colIndex;
  return ((MDSDFGeodesic*)myGeodesic)->getFloatOutput(rowIndex,colIndex);
}

        //////////////////////////////////////////
        // class MultiMDSDFPort
        //////////////////////////////////////////

MultiPortHole& MultiMDSDFPort :: setMDSDFParams(unsigned rowDimensions,
						unsigned colDimensions) {
  MPHIter next(*this);
  PortHole *p;
  while((p = next++) != 0)
    ((MDSDFPortHole*)p)->setMDSDFParams(rowDimensions, colDimensions);
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
	return installPort(p);
}

// isA
ISA_FUNC(MDSDFPortHole,DFPortHole);
