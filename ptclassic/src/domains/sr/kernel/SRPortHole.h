/* -*- c++ -*- */

#ifndef _SRPortHole_h
#define _SRPortHole_h

/*  Version $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

    Author:	S. A. Edwards
    Created:	14 April 1996

    Definition of the SR domain's PortHole classes.
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "PortHole.h"

	//////////////////////////////
	// class SRPortHole
	//////////////////////////////

class SRPortHole : public PortHole {
public:
  // Class identification
  virtual int isA(const char* className) const;

  // Return TRUE if the particle in the port hole is known (present or absent)
  virtual int known() const;

  // Return TRUE if the particle in the port hole is (known as) absent
  virtual int absent() const;

  // Return TRUE if this input is (known as) present
  virtual int present() const;

  // Return the particle being emitted
  virtual Particle & get() const;

};

	//////////////////////////////
	// class InSRPort
	//////////////////////////////

class InSRPort : public SRPortHole {
public:

  // Input/output identification.
  virtual int isItInput() const;

  // Return the particle being emitted
  Particle & get() const;


  /*virtual*/ int known() const;
  /*virtual*/ int absent() const;
  /*virtual*/ int present() const;
  
};

	//////////////////////////////
	// class OutSRPort
	//////////////////////////////

class OutSRPort : public SRPortHole {

  friend class InSRPort;

private:

  // The particle emitted by this Port Hole.  0 denotes unknown,
  // 1 denotes absent, and everything else is a present particle.
  Particle * emittedParticle;

public:

  // Destroy the particle in the port, if any, resetting it to "unknown"
  void clearPort();

  // Input/output identification.
  virtual int isItOutput() const;

  // Make the particle known absent
  void makeAbsent();

  // Emit a particle
  Particle & emit();

  /*virtual*/ int known() const;
  /*virtual*/ int absent() const;
  /*virtual*/ int present() const;

  // Return the particle being emitted
  /*virtual*/ Particle & get() const;

  void initialize();

};

	//////////////////////////////
	// class MultiSRPort
	//////////////////////////////
 
class MultiSRPort : public MultiPortHole {
};

	//////////////////////////////
	// class MultiInSRPort
	//////////////////////////////

class MultiInSRPort : public MultiSRPort {
public:
  // Input/output identification.
  virtual int isItInput() const;
 
  // Add a new physical port to the MultiPortHole list.
  virtual PortHole& newPort();
};

	//////////////////////////////
	// class MultiOutSRPort
	//////////////////////////////
 
class MultiOutSRPort : public MultiSRPort {     
public:
  // Input/output identification.
  virtual int isItOutput() const;

  // Add a new physical port to the MultiPortHole list.
  virtual PortHole& newPort();
};

#endif
