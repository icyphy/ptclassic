/* -*- c++ -*- */

#ifndef _SRPortHole_h
#define _SRPortHole_h

/*  Version @(#)SRPortHole.h	1.4 5/7/96

Copyright (c) 1990- The Regents of the University of California.
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

/**********************************************************************

  Port Hole for the SR domain

  @Description Communication in the SR domain is done through
  unbuffered wires that take on one value at the end of each instant,
  although the value of each wire evolves monotonically during each instant.

  <P> The value on a wire (and hence a port) may be unknown, absent,
  or present with some value.  The status (unknown, known absent, or
  known present) of both input and output ports may be tested.

 **********************************************************************/
class SRPortHole : public PortHole {
public:
  // Identify the class
  int isA(const char* className) const;

  virtual int known() const;

  virtual int absent() const;

  virtual int present() const;

  virtual Particle & get() const;

  PortHole& newConnection ();

  void connect(GenericPort &, int, const char* = 0);

  SRPortHole * undoAliases();

  SRPortHole * doAliases();

protected:
  virtual void setFarPort( SRPortHole * p );
};


/**********************************************************************

  Input Port Hole for the SR domain

 **********************************************************************/
class InSRPort : public SRPortHole {
public:

  virtual int isItInput() const;

  Particle & get() const;

  int known() const;
  int absent() const;
  int present() const;  

protected:
  void setFarPort( SRPortHole * );
  
};

/**********************************************************************

  Output Port Hole for the SR domain

 **********************************************************************/
class OutSRPort : public SRPortHole {

  friend class InSRPort;

private:

  // The particle emitted on this Port Hole.
  //
  // @Description 0 denotes unknown, 1 denotes absent, and everything
  // else is a pointer to present particle.

  Particle * emittedParticle;

public:

  // Destroy the particle in the port, if any, resetting it to
  // "unknown"

  void clearPort();

  // Input/output identification.
  virtual int isItOutput() const;

  // Make the particle known absent
  void makeAbsent();

  // Emit a particle
  Particle & emit();

  int known() const;
  int absent() const;
  int present() const;

  // Return the particle being emitted
  Particle & get() const;

  void initialize();

};

class MultiInSRPort : public MultiPortHole {
public:
  // Input/output identification.
  virtual int isItInput() const;
 
  // Add a new physical port to the MultiPortHole list.
  virtual PortHole& newPort();
};

class MultiOutSRPort : public MultiPortHole {     
public:
  // Input/output identification.
  virtual int isItOutput() const;

  // Add a new physical port to the MultiPortHole list.
  virtual PortHole& newPort();
};

#endif
