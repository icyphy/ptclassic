/*  Version $Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
    Date of creation:	7/22/93

    Definitions of domain-specific PortHole classes.
****************************************************************/

#ifndef _MDSDFPortHole_h
#define _MDSDFPortHole_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "SDFPortHole.h"
#include "MatrixParticle.h"
#include "MDSDFGeodesic.h"
#include "ParticleStack.h"

/*****************************************************************
MDSDF: MultiDimensional Synchronous Data Flow

This is a common special case that is handled differently
from other cases:

	Each PortHole promises to consume or generate a fixed
	number of Particles each time the Star is invoked.
	This number is stored in the PortHole and can be accessed
	by the SDFScheduler

	The incrementing of time is forced by the SDFScheduler,
	and not by the Star itself. Incrementing time is effected
	by consuming or generating Particles
****************************************************************/

#define ANYSIZE 0

        //////////////////////////////////////////
        // class MDSDFPortHole
        //////////////////////////////////////////

// MultiDimensional Synchronous dataflow MDSDFPortHole
class MDSDFPortHole : public DFPortHole
{
  friend class MDSDFGeodesic;    // allow MDSDFGeodesic to access myPlasma
 public:
  // constructor
  MDSDFPortHole();

  // destructor
  ~MDSDFPortHole();

  // class identification
  int isA(const char*) const;

  // replace PortHole::initialize()
  /*virtual*/ void initialize();

  // Returns a subMatrix inside a particle
  /*virtual*/ Particle& operator % (int);

  // functions to return the dimensions of the tokens transferred 
  //   at each firing
  int numRowXfer() const { return numRows; }
  int numColXfer() const { return numCols; }

  // functions to return the number of past tokens in each dimension
  //   needed for input at each firing
//  int pastRowTokens() const { return maxPastRowTokens; }
//  int pastColTokens() const { return maxPastColTokens; }

  // funtions to return the index of the last valid row and column
  //   on the geodesic buffer connected to this porthole
  int lastValidRowOnGeo() const { 
    return ((MDSDFGeodesic*)myGeodesic)->lastValidRow(); }
  int lastValidColOnGeo() const { 
    return ((MDSDFGeodesic*)myGeodesic)->lastValidCol(); }

  // functions to return the index of the last valid row and column
  //   needed by this PortHole on input
  int neededValidRow(int rowIndex) { return ((rowIndex+1)*numRows-1); }
  int neededValidCol(int colIndex) { return ((colIndex+1)*numCols-1); }

  // Update the last valid row and column in the Geodesic
  void setGeodesicValid(int row, int col) { 
    ((MDSDFGeodesic*)myGeodesic)->setValid(row,col); }

  // set the numRows and numCols.  Usually used by the scheduler to
  // change the value of portholes which can transmit ANYSIZE
  // data (which is interpreted as numRows == 0 and numCols == 0
  void setRowXfer(int nRows) { numRows = nRows; }
  void setColXfer(int nCols) { numCols = nCols; }

  // User called in the setup procedure of the user defined star.  Setting
  // numRows and numCols to zero implies that ANYSIZE, ie. the output
  // has the same dimensions as the input ??????
  virtual PortHole& setMDSDFParams(unsigned rowDimensions,
				   unsigned colDimensions,
				   unsigned maxPastRowTokens = 0,
				   unsigned maxPastColTokens = 0);

  int rowFiringsPerIteration();
  int colFiringsPerIteration();

  virtual Geodesic* allocateGeodesic();

  // get the location of single value inputs and outputs
  virtual double getFloatInput(int rowDelay = 0, int colDelay = 0);
  virtual double& getFloatOutput();

  // get submatrix inputs and outputs
  virtual Matrix* getInput(int rowDelay = 0, int colDelay = 0);
  virtual Matrix* getOutput();

 protected:
  // the number of matrix particles (specified in terms of a row & column
  // pair) that are consumed at each firing
//  int numRowTokens;
//  int numColTokens;

  // the dimensions of matrix particles that flow through this porthole
  int numRows;
  int numCols;

  // the number of past matrix particles read in each dimension
//  int maxPastRowTokens;
//  int maxPastColTokens;

  // replacement buffer, this is a simple particle stack for keeping
  // track of particles that were created and need to be deleted later
  ParticleStack myBuffer;

  // the dimensions of the buffer
  int rowsInBuffer;
  int colsInBuffer;

  // allocate new 2D buffer
//  /* virtual */ void allocateBuffer();
};

	///////////////////////////////////////////
	// class InMDSDFPort
	//////////////////////////////////////////

// MultiDimensional Synchronous dataflow InMDSDFPort
class InMDSDFPort : public MDSDFPortHole {
 public:
  int isItInput() const;     // returns TRUE

  // get submatrix inputs and outputs
  /*virtual*/ inline Matrix* getInput(int rowDelay = 0, int colDelay = 0);
  /*virtual*/ inline double getFloatInput(int rowDelay = 0, int colDelay = 0);

  // Get Particles from input Geodesic
  void receiveData();

  // Used in scheduler simulation, decrease the count of the number of
  //  particles ready to be removed from the geodesic by this inPorthole.
//    virtual void decCount(int n);

};

	////////////////////////////////////////////
	// class OutMDSDFPort
	////////////////////////////////////////////

class OutMDSDFPort : public MDSDFPortHole {
 public:
  int isItOutput() const;       // returns TRUE

  // Initialize the particles in the buffer to hold matrices.  Should
  // be called before the go() method.
  virtual void receiveData();

  // Put the Particles that we generated into the output MDSDFGeodesic 
  // and then tell MDSDFGeodesic to resize the data held in the Particles --
  // this method is invoked by the MDSDFScheduler after go()
  void sendData();

  /*virtual*/ inline Matrix* getOutput();
  /*virtual*/ inline double& getFloatOutput();

  // Used in scheduler simulation, increment the count of the number of
  //  particles put into the geodesic by this porthole.
  // Redefined from DFPortHole.h
//    virtual void incCount(int n);

  // called by Geodesic::initialize()
//  virtual void initializeBuffer(MatrixParticle*,int rowDelay,int colDelay); 

//private:
//  int numParticlesInBuffer;

};

        //////////////////////////////////////////
        // class MultiMDSDFPort
        //////////////////////////////////////////

// MultiDimensional Synchronous dataflow MultiPortHole: same as DFPortHole

class MultiMDSDFPort : public MultiDFPort {
 public:
  MultiPortHole& setMDSDFParams(unsigned rowDimensions,
				unsigned colDimensions,
				unsigned maxPastRowTokens = 0,
				unsigned maxPastColTokens = 0);
};
 
        //////////////////////////////////////////
        // class MultiInMDSDFPort
        //////////////////////////////////////////

class MultiInMDSDFPort : public MultiMDSDFPort
{
 public:
  // Input/output identification.
  int isItInput() const;
 
  // Add a new physical port to the MultiPortHole list.
  PortHole& newPort();
};
 
        //////////////////////////////////////////
        // class MultiOutMDSDFPort
        //////////////////////////////////////////

class MultiOutMDSDFPort : public MultiMDSDFPort
{     
 public:
  // Input/output identification.
  int isItOutput() const;

  // Add a new physical port to the MultiPortHole list.
  PortHole& newPort();
};

#endif
