/* -*- C++ -*- */

#ifndef _SRScheduler_h
#define _SRScheduler_h

/*  Version @(#)SRScheduler.h	1.1 3/19/96

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
    Created:	9 April 1996

*/

#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"

class SRScheduler : public Scheduler
{
public:
  SRScheduler();

  // Domain identification.
  /*virtual*/ const char* domain() const;

  // Initialization.
  /*virtual*/ void setup();


  // Set the stopping time
  /*virtual*/ void setStopTime(double);

  // Set the stopping time when inside a Wormhole.
  /*virtual*/ void resetStopTime(double);

  // Return the stopping time.
  /*virtual*/ double getStopTime() { return double(numInstants); }



  // Run (or continue) the simulation
  /*virtual*/ int run();

  // Run the simulation for an instant
  virtual void runOneInstant();


  // The "time" of each instant, used when interfacing with a timed domain
  double schedulePeriod;

protected:
  // Number of instants to execute
  int numInstants;

  // Number of instants already executed
  int numInstantsSoFar;

};

#endif
