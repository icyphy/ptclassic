/* -*- C++ -*- */

#ifndef _SRRecursiveScheduler_h
#define _SRRecursiveScheduler_h

/* Version $Id$

@Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

    Author:     S. A. Edwards
    Created:    23 October 1996

 */

#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"

class SRDependencyGraph;
class SRRecursiveSchedule;
class Set;

/**********************************************************************

 The recursive scheduler

 @Description

 **********************************************************************/
class SRRecursiveScheduler : public Scheduler
{
public:
  SRRecursiveScheduler();
  ~SRRecursiveScheduler();

  const char* domain() const;

  void setup();

  void setStopTime(double);

  void resetStopTime(double);

  // Return the stopping time of the simulation
  double getStopTime() { return double(numInstants); }

  int run();

  virtual void runOneInstant();

  // The "time" of each instant, used when interfacing with a timed domain
  double schedulePeriod;

  StringList displaySchedule();  

protected:
  // Number of instants to execute
  int numInstants;

  // Number of instants already executed
  int numInstantsSoFar;

  virtual int computeSchedule( Galaxy & );

  int mincost( Set &, int, SRRecursiveSchedule &, int, int );

private:

  // The dependency graph of the galaxy
  SRDependencyGraph * dgraph;

  // The schedule of the galaxy
  SRRecursiveSchedule * schedule;

  SequentialList * SCCsOf(Set &);

  void fDFSVisit( int, Set &, int &, int * );
  void bDFSVisit( int, Set &, Set &);

  void destroySCCs( SequentialList * );

};

#endif
